/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:21 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/09 11:14:24 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

class Response
{
	private:
		int _fd;
		std::string _raw_response;
		std::string _status_line;
		std::map<std::string, std::string> _headers;
		std::string _body;

	public:
		// Constructors and Destructor
		Response(const int fd);
		~Response();

		// Getters
		int get_fd() const { return _fd; }
		const std::string& get_raw_response() const { return _raw_response; }
		const std::string& get_status_line() const { return _status_line; }
		const std::map<std::string, std::string>& get_headers() const { return _headers; }
		const std::string& get_body() const { return _body; }

		// Setters
		void set_raw_response(const std::string& raw_response) { _raw_response = raw_response; }
		void set_status_line(const std::string& status_line) { _status_line = status_line; }
		void set_headers(const std::map<std::string, std::string>& headers) { _headers = headers; }
		void set_body(const std::string& body) { _body = body; }
};
