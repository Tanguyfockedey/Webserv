/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:22:08 by tafocked          #+#    #+#             */
/*   Updated: 2025/08/11 16:07:03 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

class Client
{
	private:
		int _fd;
		time_t _last_activity;
		// bool _waiting_request;

	public:
		// Constructor and Destructor
		Client(int fd);
		~Client();

		// Getters and Setters
		int get_fd() const {return _fd;}
		time_t get_last_activity() const {return _last_activity;}
		void set_last_activity() {_last_activity = time(NULL);}
		// bool get_waiting_request() const {return _waiting_request;}
		// void set_waiting_request(bool waiting) {_waiting_request = waiting;}
};
