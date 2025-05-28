/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:01:39 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/28 18:23:59 by tafocked         ###   ########.fr       */
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
		std::string _server_name;
		size_t _nb_ports;
	protected:

	public:
		// Constructor and Destructor
		server(uint16_t* port, uint32_t addr);
		server(uint16_t* port, uint32_t addr, std::string server_name);
		~server();
		
		// Getters and Setters
		int get_socket_fd() const { return _poll_fds[0].fd; }
		int get_port() const { return ntohs(_sin.sin_port); }
		const std::string &get_server_name() const { return _server_name; }
		
		// Methods
		void init_socket(uint16_t* port, uint32_t addr);
		void polling();
		void add_client();
		void read_request(int client_fd);
};

