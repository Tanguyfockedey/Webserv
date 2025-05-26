/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:59:00 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/26 16:56:28 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "server/server.hpp"
#include <errno.h>
#include  <string.h>


int main(int argc, char **argv)
{
	// if (argc < 2)
	// {
	// 	std::cerr << "Error: No configuration file provided." << std::endl;
	// 	return (1);
	// }
	// if (argc > 2)
	// {
	// 	std::cerr << "Error: Too many arguments provided." << std::endl;
	// 	return (1);
	// }
	(void)argc; // To avoid unused parameter warning
	(void)argv; // To avoid unused parameter warning
	try
	{
		server serv(5000);
		int client_fd;
		char *buffer = new char[1024]; // Buffer for receiving data, not used in this example
		if (!buffer)
		{
			std::cerr << "Memory allocation failed." << std::endl;
			return (1);
		}
		while (true)
		{
			client_fd = accept(serv.get_socket_fd(), NULL, NULL); // Accept incoming connections
			if (client_fd < 0)
			{
				std::cerr << "Error accepting connection." << std::endl;
				continue; // Continue to accept next connection
			}
			std::cout << "Client connected." << std::endl;
			send(client_fd, "HTTP/1.1 200 OK\r\n", 50, 0); // Send a simple HTTP response
			recv(client_fd, buffer, 1024, 0); // Receive data from the client (not used here)
			close(client_fd); // Close the client socket after handling
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		std::cerr << strerror(errno) << std::endl; // Print the error number
		return (1);
	}
	return (0);
}
