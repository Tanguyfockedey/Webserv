/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:01:39 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/06 17:01:59 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

class Server
{
	private:
		std::vector<struct sockaddr_in> _sin; //list of socket adresses
		std::vector<struct pollfd> _poll_fds; //fd's to poll
		std::string _server_name;
		
	protected:

	public:
		// Constructor and Destructor
		Server(uint16_t* port, uint32_t addr);
		Server(uint16_t* port, uint32_t addr, std::string server_name);
		~Server();
		
		// Getters and Setters
		const std::string &get_server_name() const { return _server_name; }
		
		// Methods
		void init_socket(uint16_t* port, uint32_t addr);
		void polling();
		void add_client(int i);
		void read_request(pollfd &poll);
		void send_request(pollfd &poll);
};
