/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:00:55 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/28 18:27:39 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

server::server(uint16_t* port, uint32_t addr): _server_name("DefaultServer")
{
	init_socket(port, addr);
	std::cout << "Server '" << _server_name << "' is listening on port(s) : ";
	for (size_t i = 0; port[i]; i++)
		std::cout << port[i] << (port[i + 1] ? ", " : "");
	std::cout << std::endl;
}

server::server(uint16_t* port, uint32_t addr, std::string server_name): _server_name(server_name)
{
	init_socket(port, addr);
	std::cout << "Server '" << _server_name << "' is listening on port " << port << std::endl;
}

server::~server()
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
	}
}

void server::init_socket(uint16_t* port, uint32_t addr)
{
	pollfd socket_fd;
	struct sockaddr_in sin;

	_nb_ports = 0;
	sin.sin_family = AF_INET;
	
	for (uint16_t i = 0; port[i]; ++i)
	{
		sin.sin_port = htons(port[i]);
		sin.sin_addr.s_addr = htonl(addr);
		
		_poll_fds.push_back(socket_fd);
		_poll_fds[i].events = POLLIN;
		_poll_fds[i].revents = 0;
		
		if ((_poll_fds[i].fd = socket(sin.sin_family, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Socket creation failed");
		
		if (bind(_poll_fds[i].fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		throw std::runtime_error("Socket binding failed");
		
		if (listen(_poll_fds[i].fd, 10) < 0)
		throw std::runtime_error("Socket listening failed");
		
		if (fcntl(_poll_fds[i].fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Setting socket to non-blocking mode failed");
		
		// if (setsockopt(socket_fd.fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) // Set socket options to allow reuse of the address
		// 	throw std::runtime_error("Setting socket options failed");
	
		_nb_ports++;
	}
}

void server::polling()
{
	while (true)
	{
		int poll_count = poll(_poll_fds.data(), _poll_fds.size(), 2000);
		if (poll_count < 0)
		{
			std::cerr << "Polling error: " << strerror(errno) << std::endl;
			continue;
		}
		std::cout << "Polling returned " << poll_count << " events." << std::endl;
		for (size_t i = 0; i < _poll_fds.size(); ++i)
		{
			if (_poll_fds[i].revents & POLLIN)
			{
				if (i < _nb_ports)
					add_client();
				else
					read_request(_poll_fds[i].fd);
			}
			if (_poll_fds[i].revents & POLLOUT)
			{
				std::cout << "Socket " << _poll_fds[i].fd << " is ready for writing." << std::endl;
			}
		}
	}
}

void server::add_client()
{
	pollfd new_client;
	new_client.fd = accept(_poll_fds[0].fd, NULL, NULL);
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

void server::read_request(int client_fd)
{
	char buffer[5];
	ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
	if (bytes_read < 0)
	{
		std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
		return;
	}
	buffer[bytes_read] = '\0';
	std::cout << "Received request: " << buffer << std::endl;
	if (bytes_read == 0)
	{
		std::cout << "Client disconnected." << std::endl;
		close(client_fd);
		for (size_t i = 0; i < _poll_fds.size(); ++i)
		{
			if (_poll_fds[i].fd == client_fd)
			{
				_poll_fds.erase(_poll_fds.begin() + i);
				break;
			}
		}
	}
}
