/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:00:55 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/24 15:31:52 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

server::server(int port)
{
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		std::cerr << "Error: Could not create socket." << std::endl;
		exit(EXIT_FAILURE);
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = INADDR_ANY;

	if (!bind(socket_fd, (struct sockaddr *)&sin, sizeof(sin)))
	{
		std::cerr << "Error: Could not bind socket." << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}

}