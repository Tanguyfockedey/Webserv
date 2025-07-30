/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:21 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/30 13:55:58 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "Request.hpp"

class Response
{
private:
	int _fd;
	Request	_req;
	std::string _response;
	std::string _status_line;
	std::string _headers_string;
	std::map<std::string, std::string> _headers;
	std::string _body;
	Config _config;


public:
	// Constructors and Destructor
	Response(const int fd, Request &req);
	~Response();

	// Getters
	int get_fd() const { return _fd; }
	const std::string& get_response() const { return _response; }
	const std::string& get_status_line() const { return _status_line; }
	const std::string& get_headers_string() const { return _headers_string; }
	const std::map<std::string, std::string>& get_headers() const { return _headers; }
	const std::string& get_body() const { return _body; }
	const Request& get_req() const { return _req; }

	// Setters
	void set_response(const std::string& response) { _response = response; }
	void set_status_line(const std::string& status_line) { _status_line = status_line; }
	void set_headers(const std::map<std::string, std::string>& headers) { _headers = headers; }
	void set_body(const std::string& body) { _body = body; }
	void set_req(const Request& req) { _req = req; }

	// Methods
	void process_get_request();
	void process_post_request();
	void process_delete_request();
	void build_response(std::fstream &path);
	void handle_single_part_post();
	void handle_multipart_post();
	const std::string get_http_date();
};
