/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:22:08 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/09 10:29:31 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

class Client
{
	private:
		int _fd;
		time_t _last_activity;

	public:
		// Constructor and Destructor
		Client(int fd);
		~Client();

		// Getters and Setters
		int get_fd() const {return _fd;}
		time_t get_last_activity() const {return _last_activity;}
		void set_last_activity() {_last_activity = time(NULL);}
};
