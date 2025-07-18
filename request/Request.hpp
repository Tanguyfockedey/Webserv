/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 14:47:50 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/18 15:31:39 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "../config/Config.hpp"

class Config;

class Request
{
private:
	int _fd;
	int _error_code;
	time_t _timestamp;
	std::string _raw_request;
	std::string _request_line;
	std::string _method;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::string _body;
	Config _config;

public:
	// Constructors and Destructor
	Request(const int fd, const std::string raw_request, Config &server_config);
	~Request();

	// Getters
	int get_fd() const { return _fd; }
	int get_error_code() const { return _error_code; }
	const std::string& get_raw_request() const { return _raw_request; }
	const std::string& get_request_line() const { return _request_line; }
	const std::string& get_method() const { return _method; }
	const std::string& get_uri() const { return _uri; }
	const std::string& get_version() const { return _version; }
	const std::map<std::string, std::string>& get_headers() const { return _headers; }
	const std::string& get_body() const { return _body; }
	const Config& get_config() const { return _config; }
	time_t get_timestamp() const { return _timestamp; }

	// Setters
	void set_request_line(const std::string &request_line) { _request_line = request_line; }
	void set_method(const std::string &method) { _method = method; }
	void set_uri(const std::string &uri) { _uri = uri; }
	void set_version(const std::string &version) { _version = version; }
	void set_headers(const std::map<std::string, std::string> &headers) { _headers = headers; }
	void set_body(const std::string &body) { _body = body; }
	void set_error_code(int error_code) { _error_code = error_code; }

	void parse_request_line();
	int is_allowed_method() const;
};

