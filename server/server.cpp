/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:00:55 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/26 16:21:37 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

server::server(int port)
{
	sockaddr_init(port, INADDR_ANY); // Initialize the sockaddr_in structure with the port number

	socket_fd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket
	if (socket_fd < 0)
		throw std::runtime_error("Socket creation failed");

	if (bind(socket_fd, (struct sockaddr *)&sin, sizeof(sin)) == -1) // Bind the socket to the address and port
	{
		close(socket_fd);
		throw std::runtime_error("Socket binding failed");
	}

	// const int enable = 1;
	// if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) // Set socket options to allow reuse of the address
	// {
	// 	close(socket_fd);
	// 	throw std::runtime_error("Setting socket options failed");
	// }

	if (listen(socket_fd, 10) < 0) // Listen for incoming connections
	{
		close(socket_fd);
		throw std::runtime_error("Socket listening failed");
	}
	std::cout << "Server is listening on port " << port << std::endl;
}

server::~server()
{
	std::cout << "Server destructor called." << std::endl;
	if (socket_fd >= 0)
	{
		close(socket_fd);
	}
}

void server::sockaddr_init(uint16_t port, uint32_t addr)
{
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(addr);
}