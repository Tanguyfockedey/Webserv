/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 14:48:10 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/24 21:02:36 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void Request::set_boundary()
{
	std::string boundary;
	
	if (_raw_request.find("boundary=") == std::string::npos)
		_boundary = "";
	else
	{
		boundary = _raw_request.substr(_raw_request.find("boundary=") + 9);
		boundary = boundary.substr(0, boundary.find_first_of('\r'));
		_boundary = boundary;
	}
}

void Request::set_actual_body_length(void)
{
	int content_length = 0;

	if (get_headers().find("Content-Length") != get_headers().end())
	{
		std::istringstream iss(get_headers().find("Content-Length")->second);
		iss >> content_length;
	}
	else
	{
		std::cerr << "Content-Length header not found." << std::endl;
	}
	
	std::string begin_boundary, end_boundary, multipart_headers, multipart_data;
	
	begin_boundary = "--" + _boundary + "\r\n";
	end_boundary   = "--" + _boundary + "--\r\n";
	
	size_t begin_pos = get_body().find(begin_boundary);
	if (begin_pos == std::string::npos)
	{
		multipart_data = _body;
		multipart_headers = "";
		_actual_body_length = content_length;
		return ;
	}
	multipart_headers = _body.substr(begin_pos + begin_boundary.length());
	multipart_headers = multipart_headers.substr(0, multipart_headers.find("\r\n\r\n") + 4);
	
	begin_pos = _body.find("\r\n\r\n");
	if (begin_pos == std::string::npos)
	{
		multipart_data = _body;
		_actual_body_length = content_length;
		return ;
	}
	_multipart_data = _body.substr(begin_pos + 4);

	_actual_body_length  = content_length - (begin_boundary.length() + end_boundary.length());
	_actual_body_length -= multipart_headers.length();
	_actual_body_length -= 2; // for the \r\n before the closing boundary
}

void Request::parse_uri()
{
	std::string uri, stripped_uri, query, fragment;
	size_t separator;

	uri = get_uri();

	try 
	{
		if (uri.find("../") != std::string::npos || uri.find("..\\") != std::string::npos)
		{
			//DEBUG - A FIX, PAS CORRECT ACTUELLEMENT, "../" ne devrait etre un probleme
			// que si ca entraine une sortie du repertoire racine
			std::cerr << "The requested URL was rejected: " << uri << std::endl;
			set_error_code(400);
			throw std::runtime_error("400 Bad Request");
		}
		else if (uri.length() > MAX_URI_LENGTH)
		{
			std::cerr << "The requested URL is too long: " << uri << std::endl;
			set_error_code(414);
			throw std::runtime_error("414 URI Too Long");
		}
		else if (uri.find(" ") != std::string::npos)
		{
			std::cerr << "The requested URL contains spaces: " << uri << std::endl;
			set_error_code(400);
			throw std::runtime_error("400 Bad Request");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return ;
	}
	separator = uri.find('?');
	if (separator != std::string::npos)
	{
		stripped_uri = uri.substr(0, separator);
		if (separator + 1 < uri.length())
		{
			query = uri.substr(separator + 1);
			separator = query.find('#');
			if (separator != std::string::npos)
			{
				if (separator + 1 < query.length())
					fragment = query.substr(separator + 1);
				else
					fragment = "";
				query = query.substr(0, separator);
			}
			else
			{
				fragment = "";
			}
		}
		else
			query = "";
	} else { // no query part
		separator = uri.find('#');
		if (separator != std::string::npos)
		{
			stripped_uri = uri.substr(0, separator);
			if (separator + 1 < uri.length())
				fragment = uri.substr(separator + 1);
		}
	}
	_uri = stripped_uri;
	_uri_query = query;
	_uri_fragment = fragment;
}


void Request::normalize_uri()
{
	std::string root, uri, normalized_uri;

	root = get_config().get_root();
	if (root.empty())
	{
		root = "/";
	}

	uri = get_uri();
	if (uri == "/" || uri.empty())
	{
		if (get_config().get_index().empty())
			uri = "/index.html";
		else
			uri = get_config().get_index();
	}
	set_uri(join_paths(root, uri));
}


void Request::parse_headers()
{
	std::map<std::string, std::string> headers_map;
	
	std::string raw_request = get_raw_request();
	size_t pos = raw_request.find("\r\n");
	size_t end_pos = raw_request.find("\r\n\r\n");
	if (get_method() == "POST" && REQUIRE_HEADERS && (std::string::npos || end_pos == std::string::npos || end_pos <= pos))
	{
		std::cerr << "Malformed request: missing headers" << std::endl;
		set_error_code(400);
		return ;
	}
	pos = raw_request.find("\r\n");
	end_pos = raw_request.find("\r\n\r\n");
	if (pos == std::string::npos || end_pos == std::string::npos || end_pos <= pos)
	{
		std::cerr << "Malformed request: missing headers" << std::endl;
		set_error_code(400);
		return ;
	}
	std::string headers_string = raw_request.substr(pos + 2, end_pos + 4);
	if (headers_string.length() > MAX_HEADER_LENGTH)
	{
		std::cerr << "Headers too long: " << headers_string.length() << " bytes" << std::endl;
		set_error_code(431);
		return ;
	}
	if (headers_string.empty() && get_method() == "POST" && REQUIRE_HEADERS)
	{
		std::cerr << "Malformed request: empty headers" << std::endl;
		set_error_code(400);
		return ;
	}
	set_headers_string(headers_string);
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
	set_headers(headers_map);
}


void Request::extract_resource_info()
{
	std::string uri, extension, mime_type;
	std::map<std::string, std::string> resource_info;
	
	uri = get_uri();

	// Get extension
	size_t dot_pos = uri.find_last_of('.');
	if (dot_pos == std::string::npos || dot_pos == uri.length() - 1)
	{
		extension = "";
		mime_type = "";
		resource_info.insert(std::make_pair("extension", extension));
		resource_info.insert(std::make_pair("mime_type", mime_type));
		set_resource_info(resource_info);
		return ;
	}
	else
	{
		extension = uri.substr(dot_pos + 1);
		// Make extension lowercase
		for (size_t x = 0; x < extension.length(); x++)
			extension[x] = tolower(extension[x]);
	}
	// Set MIME type
	if (extension == "html" || extension == "htm")
		mime_type = "text/html";
	else if (extension == "css")
		mime_type = "text/css";
	else if (extension == "js")
		mime_type = "application/javascript";
	else if (extension == "php")
		mime_type = "application/x-httpd-php";
	else if (extension == "json")
		mime_type = "application/json";
	else if (extension == "xml")
		mime_type = "application/xml";
	else if (extension == "pdf")
		mime_type = "application/pdf";
	else if (extension == "doc" || extension == "docx")
		mime_type = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	else if (extension == "xls" || extension == "xlsx")
		mime_type = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	else if (extension == "ppt" || extension == "pptx")
		mime_type = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	else if (extension == "csv")
		mime_type = "text/csv";
	else if (extension == "rtf")
		mime_type = "application/rtf";
	else if (extension == "epub")
		mime_type = "application/epub+zip";
	else if (extension == "odt")
		mime_type = "application/vnd.oasis.opendocument.text";
	else if (extension == "odp")
		mime_type = "application/vnd.oasis.opendocument.presentation";
	else if (extension == "ods")
		mime_type = "application/vnd.oasis.opendocument.spreadsheet";
	else if (extension == "jsonp")
		mime_type = "application/javascript";
	else if (extension == "jsonld")
		mime_type = "application/ld+json";
	else if (extension == "wasm")
		mime_type = "application/wasm";
	else if (extension == "woff" || extension == "woff2")
		mime_type = "font/woff";
	else if (extension == "ttf")
		mime_type = "font/ttf";
	else if (extension == "otf")
		mime_type = "font/otf";
	else if (extension == "svgz")
		mime_type = "image/svg+xml"; // Compressed SVG
	else if (extension == "webmanifest")
		mime_type = "application/manifest+json";
	else if (extension == "mpd")
		mime_type = "application/dash+xml";
	else if (extension == "flv")
		mime_type = "video/x-flv";
	else if (extension == "mkv")
		mime_type = "video/x-matroska";
	else if (extension == "mov" || extension == "qt")
		mime_type = "video/quicktime";
	else if (extension == "wmv")
		mime_type = "video/x-ms-wmv";
	else if (extension == "mpegts" || extension == "ts")
		mime_type = "video/mp2t";
	else if (extension == "3gp" || extension == "3gpp")
		mime_type = "video/3gpp";
	else if (extension == "3g2" || extension == "3gp2")
		mime_type = "video/3gpp2";
	else if (extension == "zip")
		mime_type = "application/zip";
	else if (extension == "tar")
		mime_type = "application/x-tar";
	else if (extension == "gz" || mime_type == "gzip")
		mime_type = "application/gzip";
	else if (extension == "bz2")
		mime_type = "application/x-bzip2";
	else if (extension == "7z")
		mime_type = "application/x-7z-compressed";
	else if (extension == "ra	void set_actual_body_length(void);r")
		mime_type = "application/vnd.rar";
	else if (extension == "exe")
		mime_type = "application/x-msdownload";
	else if (extension == "apk")
		mime_type = "application/vnd.android.package-archive";
	else if (extension == "sh")
		mime_type = "application/x-sh";
	else if (extension == "ps" || extension == "eps")
		mime_type = "application/postscript";
	else if (extension == "txt" || extension == "text" || extension == "conf" || extension == "log" || extension == "md")
		mime_type = "text/plain";
	else if (extension == "mp3")
		mime_type = "audio/mpeg";
	else if (extension == "wav")
		mime_type = "audio/wav";
	else if (extension == "ogg")
		mime_type = "audio/ogg";
	else if (extension == "flac")
		mime_type = "audio/flac";
	else if (extension == "midi" || extension == "mid")
		mime_type = "audio/midi";
	else if (extension == "aac")
		mime_type = "audio/aac";
	else if (extension == "m4a")
		mime_type = "audio/x-m4a";
	else if (extension == "wma")
		mime_type = "audio/x-ms-wma";
	else if (extension == "mp4a")
		mime_type = "audio/mp4";
	else if (extension == "aiff")
		mime_type = "audio/aiff";
	else if (extension == "opus")
		mime_type = "audio/opus";
	else if (extension == "ico")
		mime_type = "image/x-icon";
	else if (extension == "png")
		mime_type = "image/png";
	else if (extension == "jpg" || extension == "jpeg")
		mime_type = "image/jpeg";
	else if (extension == "gif")
		mime_type = "image/gif";
	else if (extension == "webp")
		mime_type = "image/webp";
	else if (extension == "bmp")
		mime_type = "image/bmp";
	else if (extension == "tiff" || extension == "tif")
		mime_type = "image/tiff";
	else if (extension == "avif")
		mime_type = "image/avif";
	else if (extension == "mpg" || extension == "mpeg")
		mime_type = "video/mpeg";
	else if (extension == "avi")
		mime_type = "video/x-msvideo";
	else if (extension == "mp4")
		mime_type = "video/mp4";
	else if (extension == "webm")
		mime_type = "video/webm";
	else if (extension == "azw") // Amazon Kindle eBook format
		mime_type = "application/vnd.amazon.ebook";
	else if (extension == "svg")
		mime_type = "image/svg+xml";
	else
	{
		mime_type = "application/octet-stream"; // Default MIME type
	}

	resource_info.insert(std::make_pair("extension", extension));
	resource_info.insert(std::make_pair("mime_type", mime_type));
	set_resource_info(resource_info);
}


void Request::parse_body()
{
	std::string body;
	size_t pos = _raw_request.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		std::cerr << "Malformed request: missing body" << std::endl;
		set_error_code(400);
		return ;
	}
	body = _raw_request.substr(pos + 4);
	if (body.empty() && get_method() == "POST")
	{
		std::cerr << "Empty body in request." << std::endl;
		set_error_code(400);
	}
	else if (body.length() > MAX_BODY_LENGTH)
	{
		std::cerr << "Body too long: " << body.length() << " bytes" << std::endl;
		set_error_code(413);
	}
	set_body(body);
}

Request::Request(const int fd, const std::string raw_request, Config &server_config)
	: _fd(fd), _error_code(0), _timestamp(time(NULL)), _raw_request(raw_request), _config(server_config)
{
	parse_request_line();
	parse_uri();
	normalize_uri();
	
	
	extract_resource_info();

	if (!_one_line_request)
	{
		parse_headers();
		parse_body();
		if (get_method() == "POST" && get_headers().find("Content-Length") != get_headers().end())
		{
			set_boundary();
			set_actual_body_length();
		}
		else
		{
			_boundary = "";
			_actual_body_length = 0;
		}
	}
	_headers_string = "";
	_body = "";
	_boundary = "";
	_actual_body_length = 0;
}


Request::~Request() {}


void Request::parse_request_line()
{
	std::string method, uri, version;

	if (_raw_request.empty())
	{
		std::cerr << "Empty request received." << std::endl;
		_error_code = 400;
		throw std::runtime_error("400 Bad Request");
	}
	
	try
	{
		if (_raw_request.find("\r\n") == std::string::npos)
		{
			_request_line = _raw_request; // _request_line = _raw_request + "\r\n";
			_one_line_request = 1;
		}
		else
			_request_line = _raw_request.substr(0, _raw_request.find("\r\n"));
		std::istringstream iss(_request_line);
		iss >> method >> uri >> version;
		set_method(method);
		set_uri(uri);
		set_version(version);
		if (method.empty() || uri.empty() || version.empty())
		{
			std::cerr << "Malformed request line: " << _request_line << std::endl;
			_error_code = 400;
			throw std::runtime_error("400 Bad Request");
		}
		if (version != "HTTP/1.1" && version != "HTTP/1.0" && version != "undefined")
		{
			std::cerr << "Unsupported HTTP version: " << version << std::endl;
			_error_code = 505;
			throw std::runtime_error("505 HTTP Version Not Supported");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		std::cerr << "Unsupported HTTP method: " << method << std::endl;
		_error_code = 501;
		throw std::runtime_error("501 Not Implemented");
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
