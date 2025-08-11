/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:00:55 by tafocked          #+#    #+#             */
/*   Updated: 2025/08/09 13:56:20 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const Config& config): _config(config)
{
	std::cout << "Server '" << _config.get_token("", "server_name") << "' is listening on port(s) : ";
	init_socket();
	std::cout << std::endl;
}

Server::~Server()
{
	std::cout << "Server destructor called." << std::endl;
	for (size_t i = 0; i < _poll_fds.size(); i++)
	{
		if (_poll_fds[i].fd >= 0)
		{
			close(_poll_fds[i].fd);
			std::cout << "Closed socket fd: " << _poll_fds[i].fd << std::endl;
		}
	}
	_poll_fds.clear();
	_sin.clear();
}

void Server::init_socket()
{
	pollfd socket_fd;
	sockaddr_in sin;
	int j = 0;
	for (std::vector<uint16_t>::const_iterator i = _config.get_port().begin(); i < _config.get_port().end(); i++)
	{
		_sin.push_back(sin);
		_sin[j].sin_family = AF_INET;
		_sin[j].sin_port = htons(*i);
		_sin[j].sin_addr.s_addr = inet_addr(_config.get_addr()[j].c_str());
		
		_poll_fds.push_back(socket_fd);
		_poll_fds[j].events = POLLIN;
		_poll_fds[j].revents = 0;
		
		if ((_poll_fds[j].fd = socket(_sin[j].sin_family, SOCK_STREAM, 0)) < 0)
		{
			std::cerr << "Could not create socket: " << strerror(errno) << std::endl;
			_sin.pop_back();
			_poll_fds.pop_back();
			continue;
		}
		if (bind(_poll_fds[j].fd, (struct sockaddr *)&_sin[j], sizeof(_sin[j])) < 0)
		{
			close(_poll_fds[j].fd);
			std::cerr << "{Binding socket failed: " << strerror(errno) << "} ";
			_sin.pop_back();
			_poll_fds.pop_back();
			continue;
		}
		if (listen(_poll_fds[j].fd, 10) < 0)
		{
			close(_poll_fds[j].fd);
			std::cerr << "{Listening on socket failed: " << strerror(errno) <<  "} ";
			_sin.pop_back();
			_poll_fds.pop_back();
			continue;
		}
		std::cout << *i << "(" << _poll_fds[j].fd << ") ";
		j++;
	}
}

void Server::polling()
{
	int poll_count = poll(_poll_fds.data(), _poll_fds.size(), 1);
	if (poll_count < 0)
	{
		std::cerr << "Polling error: " << strerror(errno) << std::endl;
		return;
	}
	if (poll_count > 0)
	{
		size_t i = 0;
		while (i < _poll_fds.size())
		{
			if (_poll_fds[i].revents & POLLIN)
			{
				if (i < _sin.size())
				add_client(i);
				else
				read_request(_poll_fds[i]);
			}
			if (_poll_fds[i].revents & POLLOUT)
				send_response(_poll_fds[i]);
			i++;
		}
	}
	check_clients_timeout();
	check_requests_timeout();
}

void Server::add_client(int i)
{
	pollfd new_fd;

	new_fd.fd = accept(_poll_fds[i].fd, NULL, NULL);
	if (new_fd.fd < 0)
	{
		std::cerr << "Error accepting new client: " << strerror(errno) << std::endl;
		return;
	}
	new_fd.events = POLLIN;
	new_fd.revents = 0;
	_poll_fds.push_back(new_fd);
	_clients.push_back(Client(new_fd.fd));
}

void Server::remove_client(int fd)
{
	std::cout << YELLOW << "Client [" << fd << "] disconnected." << RESET << std::endl;
	close(fd);
	for (size_t i = 0; i < _poll_fds.size(); i++)
	{	
		if (_poll_fds[i].fd == fd)
		{
			_poll_fds.erase(_poll_fds.begin() + i);
			i--;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].get_fd() == fd)
		{
			_clients.erase(_clients.begin() + i);
			i--;
		}
	}
}

void Server::read_request(pollfd &poll)
{
	char buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);
	ssize_t bytes_read = recv(poll.fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (bytes_read < 0)
	{
		std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
		remove_client(poll.fd);
		return;
	}
	if (bytes_read == 0)
	{
		// std::cout << YELLOW << "Client [" << poll.fd << "] closed connection on read." << RESET << std::endl;
		if (!pending_response(poll.fd))
			remove_client(poll.fd);
		return;
	}
	update_client_timeout(poll.fd);
	std::string str(buffer, bytes_read);
	Request *request = find_request(poll.fd);

	if (!request)
	{
		_requests.push_back(Request(poll.fd, str, _config));
		request = &_requests.back();
		if (request->is_complete())
		{
			std::cout << PURPLE << "Complete request received [" << poll.fd << "]:\n" << RESET;
			std::cout << MAGENTA << _requests.back().get_raw_request() << RESET << std::endl;
			process_request(poll, request);
		}
		else
		{
			std::cout << PURPLE << "Partial request received [" << poll.fd << "]:\n" << RESET;
			std::cout << MAGENTA << bytes_read << RESET << std::endl;
		}
	}
	else
	{
		request->set_raw_request(request->get_raw_request() + str);
		if (request->is_complete())
		{
			std::cout << PURPLE << "Complete request received [" << poll.fd << "]:\n" << RESET;
			std::cout << MAGENTA << request->get_headers_string() << RESET << std::endl;
			process_request(poll, request);
		}
		else
		{
			std::cout << PURPLE << "Partial request received [" << poll.fd << "]:\n" << RESET;
			std::cout << MAGENTA << bytes_read << RESET << std::endl;
		}
	}
}

void Server::process_request(pollfd &poll, Request *request)
{
	_response.push_back(Response(poll.fd, *request));
	poll.events |= POLLOUT;
	for (std::vector<Request>::iterator it = _requests.begin(); it != _requests.end(); it++)
	{
		if (&(*it) == request)
		{
			_requests.erase(it);
			break;
		}	
	}
}

void Server::send_response(pollfd &poll)
{
	for (size_t i = 0; i < _response.size(); i++)
	{
		if (_response[i].get_fd() == poll.fd)
		{
			ssize_t bytes_sent = send(poll.fd, _response[i].get_response().data(), _response[i].get_response().size(), MSG_DONTWAIT);
			if (bytes_sent < 0)
			{
				std::cerr << "Error sending response to client: " << strerror(errno) << std::endl;
				_response.erase(_response.begin() + i);
				remove_client(poll.fd);
			}
			else if (bytes_sent == 0)
			{
				// std::cout << YELLOW << "Client [" << poll.fd << "] closed connection on send." << RESET << std::endl;
				_response.erase(_response.begin() + i);
				remove_client(poll.fd);
			}
			else if (bytes_sent == static_cast<ssize_t>(_response[i].get_response().size()))
			{
				std::cout << BLUE << "Complete response sent [" << poll.fd << "]:\n" << RESET;
				std::cout << CYAN << _response[i].get_headers_string() << RESET << std::endl;
				_response.erase(_response.begin() + i);
				poll.events ^= POLLOUT;
			}
			else
			{
				std::cout << BLUE << "Partial response sent [" << poll.fd << "]:\n" << RESET;
				std::cout << CYAN << bytes_sent << RESET << std::endl << std::endl;
				_response[i].set_response(_response[i].get_response().substr(bytes_sent));
			}
			return ;
		}
	}
}

void Server::update_client_timeout(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].get_fd() == fd)
		{
			_clients[i].set_last_activity();
			return;
		}
	}
}

void Server::check_clients_timeout()
{
	time_t current_time = time(NULL);
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (current_time - _clients[i].get_last_activity() >= CONNECTION_TIMEOUT)
		{
			std::cout << YELLOW << "Client [" << _clients[i].get_fd() << "] timeout, disconnecting." << RESET << std::endl;
			remove_client(_clients[i].get_fd());
			i--;
		}
	}
}

void Server::check_requests_timeout()
{
	time_t current_time = time(NULL);
	for (size_t i = 0; i < _requests.size(); i++)
	{
		if (current_time - _requests[i].get_timestamp() >= REQUEST_TIMEOUT)
		{
			std::cout << MAGENTA << "Request timeout, removing request." << RESET << std::endl;
			_requests.erase(_requests.begin() + i);
			i--;
		}
	}
}

bool Server::pending_response(int fd) const
{
	for (std::vector<Response>::const_iterator i = _response.begin(); i != _response.end(); i++)
	{
		if (i->get_fd() == fd)
			return true;
	}
	return false;
}

Request* Server::find_request(int fd)
{
	for (size_t i = 0; i < _requests.size(); i++)
	{
		if (_requests[i].get_fd() == fd)
			return &_requests[i];
	}
	return NULL;
}

