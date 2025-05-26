/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:01:39 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/26 16:16:24 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

class server
{
	private:
		int socket_fd;
		struct sockaddr_in sin;
		
	protected:

	public:
		server(int port);
		server(const server &copy); // Disable copy constructor
		~server();

		server &operator=(const server &copy); // Disable copy assignment operator

		void sockaddr_init(uint16_t port, uint32_t addr);
		int get_socket_fd() const { return socket_fd; }

};

