/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 14:48:10 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/18 16:23:34 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

static void parse_uri(Request *req)
{
	std::string uri = req->get_uri();

	try 
	{
		if (uri.find("../") != std::string::npos || uri.find("..\\") != std::string::npos)
		{
			std::cerr << "The requested URL was rejected: " << uri << std::endl;
			req->set_error_code(400);
			throw std::runtime_error("400 Bad Request");
		}
		else if (uri.length() > MAX_URI_LENGTH)
		{
			std::cerr << "The requested URL is too long: " << uri << std::endl;
			req->set_error_code(414);
			throw std::runtime_error("414 URI Too Long");
		}
		else if (uri.find(" ") != std::string::npos && uri.find("HTTP/") == std::string::npos)
		{
			std::cerr << "The requested URL is too long: " << uri << std::endl;
			req->set_error_code(414);
			throw std::runtime_error("414 URI Too Long");
		}
		else if (uri.find(" ") != std::string::npos)
		{
			std::cerr << "The requested URL contains spaces: " << uri << std::endl;
			req->set_error_code(400);
			throw std::runtime_error("400 Bad Request");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return ;
	}
}


static void normalize_uri(Request *req)
{
	std::string uri = req->get_uri();

	if (uri == "/" || uri.empty())
	{
		if (req->get_config().get_index().empty())
			req->set_uri("index.html");
		else
			req->set_uri(req->get_config().get_index());
	}
	else if (req->get_uri()[0] == '/')
		req->set_uri(uri.substr(1)); // remove leading slash
}


static void parse_headers(Request *req)
{
	std::map<std::string, std::string> headers_map;
	
	std::string raw_request = req->get_raw_request();
	size_t pos = raw_request.find("\r\n");
	size_t end_pos = raw_request.find("\r\n\r\n");
	if (pos == std::string::npos || end_pos == std::string::npos || end_pos <= pos)
	{
		std::cerr << "Malformed request: missing headers" << std::endl;
		req->set_error_code(400);
		return ;
	}
	std::string headers_string = raw_request.substr(raw_request.find("\r\n") + 2, raw_request.find("\r\n\r\n") + 4);
	if (headers_string.length() > MAX_HEADER_LENGTH)
	{
		std::cerr << "Headers too long: " << headers_string.length() << " bytes" << std::endl;
		req->set_error_code(431);
		return ;
	}
	if (headers_string.empty())
	{
		std::cerr << "Malformed request: empty headers" << std::endl;
		req->set_error_code(400);
		return ;
	}
	std::istringstream iss(headers_string);
	std::string line;

	while (std::getline(iss, line))
	{
		if (line.empty())
			continue; // skip empty lines
		size_t pos = line.find(':');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			value.erase(0, value.find_first_not_of(" \t")); // trim leading whitespace
			headers_map.insert(std::make_pair(key, value));
		}
	}
	req->set_headers(headers_map);
}


Request::Request(const int fd, const std::string raw_request, Config &server_config)
	: _fd(fd), _error_code(0), _timestamp(time(NULL)), _raw_request(raw_request), _config(server_config)
{
	parse_request_line();
	parse_uri(this);
	normalize_uri(this);
	parse_headers(this);
}


Request::~Request() {}


void Request::parse_request_line()
{
	std::string request_line, method, uri, version;

	try
	{
		request_line = _raw_request.substr(0, _raw_request.find("\r\n"));
		std::istringstream iss(request_line);
		iss >> method >> uri >> version;

		this->set_method(method);
		this->set_uri(uri);
		this->set_version(version);

		if (method.empty() || uri.empty() || version.empty())
		{
			std::cerr << "Malformed request line: " << request_line << std::endl;
			_error_code = 400;
			throw std::runtime_error("400 Bad Request");
		}
		if (version != "HTTP/1.1" && version != "HTTP/1.0" && version != "undefined")
		{
			std::cerr << "Unsupported HTTP version: " << version << std::endl;
			_error_code = 505;
			throw std::runtime_error("505 HTTP Version Not Supported");
		}
		this->set_request_line(request_line);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		std::cerr << "Unsupported HTTP method: " << method << std::endl;
		throw std::runtime_error("501 Not Implemented");
		_error_code = 501;
		return ;
	}
	else if (!is_allowed_method())
	{
		std::cerr << "Method not allowed: " << method << std::endl;
		throw std::runtime_error("405 Method Not Allowed");
		_error_code = 405;
		return ;
	}
}


int Request::is_allowed_method() const
{
	// based on uri path, check if the location settings in the config file allow for that method in that path / location
	
	// temporarily, return 1
	return 1;
}
