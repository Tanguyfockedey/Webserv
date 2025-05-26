/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:00:55 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/26 19:52:36 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

server::server(int port)
{
	init_socket(port, INADDR_ANY);
	std::cout << "Server is listening on port " << port << std::endl;
}

server::~server()
{
	std::cout << "Server destructor called." << std::endl;
	if (_socket_fd >= 0)
	{
		close(_socket_fd);
	}
}

void server::init_socket(uint16_t port, uint32_t addr)
{
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
	_sin.sin_addr.s_addr = htonl(addr);

	if ((_socket_fd = socket(_sin.sin_family, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Socket creation failed");

	if (bind(_socket_fd, (struct sockaddr *)&_sin, sizeof(_sin)) < 0)
		throw std::runtime_error("Socket binding failed");

	if (listen(_socket_fd, 10) < 0)
		throw std::runtime_error("Socket listening failed");

	if (fcntl(_socket_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Setting socket to non-blocking mode failed");

	// if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) // Set socket options to allow reuse of the address
	// 	throw std::runtime_error("Setting socket options failed");

	pollfd socket_fd;
	socket_fd.fd = _socket_fd;
	socket_fd.events = POLLIN;
	socket_fd.revents = 0;
	_poll_fds.push_back(socket_fd);
}

void server::polling()
{
	while (true)
	{
		int poll_count = poll(_poll_fds.data(), _poll_fds.size(), 2000); // Wait for events on the socket
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
				if (i == 0)
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
	new_client.fd = accept(_socket_fd, NULL, NULL); // Accept a new client connection
	if (new_client.fd < 0)
	{
		std::cerr << "Error accepting new client: " << strerror(errno) << std::endl;
		return; // Return if accepting the client fails
	}
	new_client.events = POLLIN; // Set the events to listen for incoming data
	new_client.revents = 0;
	_poll_fds.push_back(new_client); // Add the new client to the poll_fds vector
	std::cout << "New client added with fd: " << new_client.fd << std::endl;
}

void server::read_request(int client_fd)
{
	char buffer[1024];
	ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1); // Read data from the client
	if (bytes_read < 0)
	{
		std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
		return; // Return if reading fails
	}
	buffer[bytes_read] = '\0'; // Null-terminate the buffer
	std::cout << "Received request: " << buffer << std::endl; // Print the received request
}