/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:31:56 by jrichir           #+#    #+#             */
/*   Updated: 2025/06/18 16:33:11 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

class Response
{
private:
	int fd;
	time_t _timestamp;
	std::string _raw_response;
	std::string _method;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::string _body;

public:
	// Constructors and Destructor
	Response(const int fd, const std::string raw_response);
	~Response();

	// Getters
	const std::string& get_raw_response() const { return _raw_response; }
	const std::string& get_method() const { return _method; }
	const std::string& get_uri() const { return _uri; }
	const std::string& get_version() const { return _version; }
	const std::map<std::string, std::string>& get_headers() const { return _headers; }
	const std::string& get_body() const { return _body; }
	time_t get_timestamp() const { return _timestamp; }

	// Setters
	void set_method(const std::string &method) { _method = method; }
	void set_uri(const std::string &uri) { _uri = uri; }
	void set_version(const std::string &version) { _version = version; }
	void set_headers(const std::map<std::string, std::string> &headers) { _headers = headers; }
	void set_body(const std::string &body) { _body = body; }
};

