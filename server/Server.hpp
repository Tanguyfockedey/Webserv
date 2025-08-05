/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:01:39 by tafocked          #+#    #+#             */
/*   Updated: 2025/08/05 18:46:48 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "../config/Config.hpp"
#include "../request/Request.hpp"
#include "../request/Response.hpp"
#include "Client.hpp"

class Client;
class Response;

class Server
{
	private:
		Config _config;
		std::vector<struct sockaddr_in> _sin; //list of socket adresses/open ports
		std::vector<struct pollfd> _poll_fds;
		std::vector<Client> _clients;
		std::vector<Request> _requests;
		std::vector<Response> _response;

	public:
		// Constructor and Destructor
		Server(const Config& config);
		~Server();
				
		// Methods
		void polling();
		
	private:
		void init_socket();
		void add_client(int i);
		void remove_client(int fd);
		void read_request(pollfd &poll);
		void process_request(pollfd &poll);
		void send_response(pollfd &poll);
		void update_client_timeout(int fd);
		void check_clients_timeout();
		void check_requests_timeout();
		bool pending_response(int fd) const;
		// Client* find_client(int fd);
		Request* find_request(int fd);
};
