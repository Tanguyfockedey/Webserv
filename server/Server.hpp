/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:01:39 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/14 20:20:16 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "../config/Config.hpp"

class Server
{
	private:
		Config _config;
		std::vector<struct sockaddr_in> _sin; //list of socket adresses/open ports
		std::vector<struct pollfd> _poll_fds;
		std::map<int, std::string> _requests;
		std::map<int, std::string> _response;

	public:
		// Constructor and Destructor
		Server(Config config);
		~Server();
		
		// Getters and Setters
		const std::string &get_server_name() const { return _config.get_server_name(); }
		
		// Methods
		void polling();
		
	private:
		void init_socket();
		void add_client(int i);
		void remove_client(pollfd &poll);
		void read_request(pollfd &poll);
		void process_request(pollfd &poll);
		void send_request(pollfd &poll);
};
