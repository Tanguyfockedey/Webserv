/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:00:55 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/09 18:41:49 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(uint16_t* port, uint32_t addr): _server_name("DefaultServer")
{
	init_socket(port, addr);
	std::cout << "Server '" << _server_name << "' is listening on port(s) : ";
	for (size_t i = 0; port[i]; i++)
	{
		if (port[i + 1])
			std::cout << port[i] << ", ";
		else
			std::cout << port[i] << std::endl;
	}
}

Server::Server(uint16_t* port, uint32_t addr, std::string server_name): _server_name(server_name)
{
	init_socket(port, addr);
	std::cout << "Server '" << _server_name << "' is listening on port(s) : ";
	for (size_t i = 0; port[i]; i++)
	{
		if (port[i + 1])
			std::cout << port[i] << ", ";
		else
			std::cout << port[i] << std::endl;
	}
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

void Server::init_socket(uint16_t* port, uint32_t addr)
{
	pollfd socket_fd;
	sockaddr_in sin;
	
	for (uint16_t i = 0; port[i]; ++i)
	{
		_sin.push_back(sin);
		_sin[i].sin_family = AF_INET;
		_sin[i].sin_port = htons(port[i]);
		_sin[i].sin_addr.s_addr = htonl(addr);
		
		_poll_fds.push_back(socket_fd);
		_poll_fds[i].events = POLLIN;
		_poll_fds[i].revents = 0;
		
		if ((_poll_fds[i].fd = socket(_sin[i].sin_family, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Socket creation failed");
		
		if (bind(_poll_fds[i].fd, (struct sockaddr *)&_sin[i], sizeof(_sin[i])) < 0)
		throw std::runtime_error("Socket binding failed");
		
		if (listen(_poll_fds[i].fd, 10) < 0)
		throw std::runtime_error("Socket listening failed");
		
		if (fcntl(_poll_fds[i].fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Setting socket to non-blocking mode failed");
		
		// const int opt = 1;
		// const int* optval = &opt;
		// if (setsockopt(socket_fd.fd, SOL_SOCKET, SO_REUSEADDR, optval, sizeof(int)) == -1) // Set socket options to allow reuse of the address
		// 	throw std::runtime_error("Setting socket options failed");
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
		_requests.insert(std::make_pair(poll.fd, str));
	else
		_requests[poll.fd].append(str);
	if (bytes_read < static_cast<int>(sizeof(buffer) - 1))
		process_request(poll);
}

void Server::process_request(pollfd &poll)
{
	std::string response;

	std::cout << "Request received: " << _requests[poll.fd] << std::endl;
	response.append("HTTP/1.1 200 ok\r\n\r\n");
	_response.insert(std::make_pair(poll.fd, response));
	poll.events |= POLLOUT;
	_requests.erase(poll.fd);
}

void Server::send_request(pollfd &poll)
{
	write(poll.fd, _response[poll.fd].c_str(), _response[poll.fd].size());
	_response.erase(poll.fd);
	poll.events ^= POLLOUT;
}
