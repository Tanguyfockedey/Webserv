/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:00:55 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/14 20:24:13 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(Config config)
{
	_config = config;
	std::cout << "Server '" << config.get_server_name() << "' is listening on port(s) : ";
	init_socket();
	std::cout << std::endl;
}

Server::~Server()
{
	std::cout << "Server destructor called." << std::endl;
	for (size_t i = 0; i < _poll_fds.size(); ++i)
	{
		if (_poll_fds[i].fd >= 0)
		{
			close(_poll_fds[i].fd);
			std::cout << "Closed socket fd: " << _poll_fds[i].fd << std::endl;
		}
		_poll_fds.clear();
		_sin.clear();
	}
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
			j++;
			continue;
		}
		
		if (bind(_poll_fds[j].fd, (struct sockaddr *)&_sin[j], sizeof(_sin[j])) < 0)
		{
			close(_poll_fds[j].fd);
			std::cerr << "{Binding socket failed: " << strerror(errno) << "} ";
			j++;
			continue;
		}
		
		if (listen(_poll_fds[j].fd, 10) < 0)
		{
			close(_poll_fds[j].fd);
			std::cerr << "{Listening on socket failed: " << strerror(errno) <<  "} ";
			j++;
			continue;
		}
		
		if (fcntl(_poll_fds[j].fd, F_SETFL, O_NONBLOCK) < 0)
		{
			close(_poll_fds[j].fd);
			std::cerr << "{Setting socket to non-blocking mode failed: " << strerror(errno) << "} ";
			j++;
			continue;
		}
		std::cout << *i << " ";
		j++;
	}
}

void Server::polling()
{
		int poll_count = poll(_poll_fds.data(), _poll_fds.size(), 0);
		if (poll_count < 0)
		{
			std::cerr << "Polling error: " << strerror(errno) << std::endl;
			return;
		}
		for (size_t i = 0; i < _poll_fds.size(); ++i)
		{
			if (_poll_fds[i].revents & POLLIN)
			{
				if (i < _sin.size())
					add_client(i);
				else
					read_request(_poll_fds[i]);
			}
			if (_poll_fds[i].revents & POLLOUT)
				send_request(_poll_fds[i]);
		}
}

void Server::add_client(int i)
{
	pollfd new_client;
	new_client.fd = accept(_poll_fds[i].fd, NULL, NULL);
	if (new_client.fd < 0)
	{
		std::cerr << "Error accepting new client: " << strerror(errno) << std::endl;
		return;
	}
	new_client.events = POLLIN;
	new_client.revents = 0;
	_poll_fds.push_back(new_client);
	std::cout << "New client added with fd: " << new_client.fd << std::endl;
}

void Server::remove_client(pollfd &poll)
{
	std::cout << "Client disconnected." << std::endl;
	close(poll.fd);
	for (size_t i = 0; i < _poll_fds.size(); ++i)
	{
		if (_poll_fds[i].fd == poll.fd)
		{
			_poll_fds.erase(_poll_fds.begin() + i);
			return;
		}
	}
}

void Server::read_request(pollfd &poll)
{
	ssize_t buff_size = 65536;
	char buffer[65536];

	memset(buffer, 0, sizeof(buffer));
	ssize_t bytes_read = read(poll.fd, buffer, sizeof(buffer) - 1);
	if (bytes_read < 0)
	{
		std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
		remove_client(poll);
		return;
	}
	if (bytes_read == 0)
	{
		remove_client(poll);
		return;
	}
	std::string str = buffer;
	if (_requests.find(poll.fd) == _requests.end())
		_requests[poll.fd] = str;
	else
		_requests[poll.fd].append(str);
	if (bytes_read < (buff_size - 1))
		process_request(poll);
}

void Server::process_request(pollfd &poll)
{
	std::string response;

	std::cout << "Request received: " << _requests[poll.fd] << std::endl;
	response.append("HTTP/1.1 200 ok\r\n\r\n");
	_response[poll.fd] = response;
	poll.events |= POLLOUT;
	_requests.erase(poll.fd);
}

void Server::send_request(pollfd &poll)
{
	write(poll.fd, _response[poll.fd].c_str(), _response[poll.fd].size());
	_response.erase(poll.fd);
	poll.events ^= POLLOUT;
}
