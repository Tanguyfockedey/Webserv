/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 14:48:10 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/18 17:57:51 by jrichir          ###   ########.fr       */
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
	std::string root, uri, normalized_uri;
	char root_last, uri_first;

	root = req->get_config().get_root();
	if (root.empty())
	{
		root = "/";
	}

	uri = req->get_uri();
	if (uri == "/" || uri.empty())
	{
		if (req->get_config().get_index().empty())
			uri = "/index.html";
		else
			uri = req->get_config().get_index();
	}

	root_last = root[root.length() - 1];
	uri_first = uri[0];
	if (root_last == '/' && uri_first == '/')
		uri = uri.substr(1);
	else if (root_last != '/' && uri_first != '/')
		uri = "/" + uri;
	
	normalized_uri = root + uri;
	req->set_uri(normalized_uri);
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
	req->set_headers_string(headers_string);
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


static void extract_resource_info(Request *req)
{
	std::string uri = req->get_uri();
	std::map<std::string, std::string> resource_info;

	// Get extension
	std::string extension = uri.substr(uri.find_last_of('.') + 1);

	// Make extension lowercase
	for (size_t x = 0; x < extension.length(); x++)
		extension[x] = tolower(extension[x]);
	
	// Set MIME type
	std::string mime_type;
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
		mime_type = "application/javascript"; // JSONP is often served as JavaScript
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
	else if (extension == "rar")
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
	else if (extension == "webp") // WebP Image
		mime_type = "image/webp";
	else if (extension == "bmp") // Bitmap Image
		mime_type = "image/bmp";
	else if (extension == "tiff" || extension == "tif") // TIFF Image
		mime_type = "image/tiff";
	else if (extension == "avif") // AV1 Image File Format
		mime_type = "image/avif";
	else if (extension == "mpg" || extension == "mpeg") // MPEG Video
		mime_type = "video/mpeg";
	else if (extension == "avi") // AVI: Audio Video Interleave
		mime_type = "video/x-msvideo";
	else if (extension == "mp4") // MPEG-4 Video
		mime_type = "video/mp4";
	else if (extension == "webm") // WebM Video
		mime_type = "video/webm";
	else if (extension == "azw") // Amazon Kindle eBook format
		mime_type = "application/vnd.amazon.ebook";
	else if (extension == "svg")
		mime_type = "image/svg+xml";
	else
	{
		std::cout << "Unknown extension: _" << extension << "_" << std::endl;
		mime_type = "application/octet-stream"; // Default MIME type
	}

	resource_info.insert(std::make_pair("extension", extension));
	resource_info.insert(std::make_pair("mime_type", mime_type));
	req->set_resource_info(resource_info);
}


static void parse_body(Request *req)
{
	std::string body;
	
	body = req->get_raw_request().substr(req->get_raw_request().find("\r\n\r\n") + 4);
	if (body.empty() && req->get_method() == "POST")
	{
		std::cerr << "Empty body in request." << std::endl;
		req->set_error_code(400);
	}
	else if (body.length() > MAX_BODY_LENGTH)
	{
		std::cerr << "Body too long: " << body.length() << " bytes" << std::endl;
		req->set_error_code(413);
	}
	req->set_body(body);
}

Request::Request(const int fd, const std::string raw_request, Config &server_config)
	: _fd(fd), _error_code(0), _timestamp(time(NULL)), _raw_request(raw_request), _config(server_config)
{
	parse_request_line();

	// DEBUG
	std::cout << "REQUEST LINE : " << _request_line << std::endl;

	parse_uri(this);
	normalize_uri(this);
	extract_resource_info(this);
	parse_headers(this);
	parse_body(this);
}


Request::~Request() {}


void Request::parse_request_line()
{
	std::string request_line, method, uri, version;

	if (_raw_request.empty())
	{
		std::cerr << "Empty request received." << std::endl;
		_error_code = 400;
		throw std::runtime_error("400 Bad Request");
	}
	
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
