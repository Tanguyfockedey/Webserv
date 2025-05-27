/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:01:39 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/27 18:08:40 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <errno.h>
#include <string.h>

class server
{
	private:
		struct sockaddr_in _sin;
		std::vector<struct pollfd> _poll_fds;

	protected:

	public:
		server(int port);
		~server();
		
		void init_socket(uint16_t port, uint32_t addr);
		void polling();
		void add_client();
		void read_request(int client_fd);
};

