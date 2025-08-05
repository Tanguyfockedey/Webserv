/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 14:47:50 by tafocked          #+#    #+#             */
/*   Updated: 2025/08/05 18:38:46 by tafocked         ###   ########.fr       */
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
	int	_one_line_request;
	bool _headers_parsed;
	time_t _timestamp;
	std::string _raw_request;
	std::string _request_line;
	std::string _method;
	std::string _uri;
	std::string _uri_query;    // ex: ?lang=php
	std::string _uri_fragment; // ex: #anchor
	std::string _version;
	std::string _headers_string;
	std::map<std::string, std::string> _headers;
	std::string _boundary;
	std::string _multipart_data;
	size_t _actual_body_length;
	std::string _body;
	std::map<std::string, std::string> _resource_info;
	Config _config;

public:
	// Constructors and Destructor
	Request(const int fd, const std::string raw_request, const Config &server_config);
	~Request();

	// Getters
	const int& get_fd() const { return _fd; }
	const int& get_error_code() const { return _error_code; }
	const std::string& get_raw_request() const { return _raw_request; }
	// const std::string& get_request_line() const { return _request_line; }
	const std::string& get_method() const { return _method; }
	const std::string& get_uri() const { return _uri; }
	// const std::string& get_uri_query() const { return _uri_query; }
	// const std::string& get_uri_fragment() const { return _uri_fragment; }
	// const std::string& get_version() const { return _version; }
	// const std::string& get_headers_string() const { return _headers_string; }
	// const std::map<std::string, std::string>& get_headers() const { return _headers; }
	const std::map<std::string, std::string>& get_resource_info() const { return _resource_info; }
	const std::string& get_body() const { return _body; }
	const std::string& get_boundary() const { return _boundary; }
	const std::string& get_multipart_data() const { return _multipart_data; }
	size_t get_actual_body_length() const { return _actual_body_length; }
	const Config& get_config() const { return _config; }
	time_t get_timestamp() const { return _timestamp; }

	// Setters
	void set_raw_request(const std::string &raw_request) { _raw_request = raw_request; }
	// void set_request_line(const std::string &request_line) { _request_line = request_line; }
	// void set_method(const std::string &method) { _method = method; }
	// void set_uri(const std::string &uri) { _uri = uri; }
	// void set_version(const std::string &version) { _version = version; }
	// void set_headers_string(const std::string &headers_string) { _headers_string = headers_string; }
	// void set_headers(const std::map<std::string, std::string> &headers) { _headers = headers; }
	// void set_resource_info(const std::map<std::string, std::string> &resource_info) { _resource_info = resource_info; }
	// void set_body(const std::string &body) { _body = body; }
	void set_boundary(void);
	void set_actual_body_length(void);
	// void set_error_code(int error_code) { _error_code = error_code; }

	// Methods
	void parse_request_line();
	void parse_uri();
	void normalize_uri();
	void extract_resource_info();
	void parse_headers();
	void parse_body();
	int is_allowed_method() const;
	bool is_complete();
};
