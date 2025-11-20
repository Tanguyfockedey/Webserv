/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 14:48:10 by tafocked          #+#    #+#             */
/*   Updated: 2025/11/20 05:44:38 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const int fd, const std::string raw_request, const Config &server_config)
	: _fd(fd), _error_code(0), _headers_parsed(false), _timestamp(time(NULL)), _raw_request(raw_request), _actual_body_length(0), _config(server_config)
{}

Request::~Request()
{}

std::string Request::work_out_path()
{
	std::string res_path;

	std::string common = common_path(_raw_uri, _config.get_location_path(_raw_uri));
	size_t common_length = common.length();

	
	std::string program_path = server_path();
	
	std::string server_root_path, location_root_path;

	if (_raw_uri.find("/cgi-bin/") != std::string::npos)
	{
		server_root_path = "";
		location_root_path = "";
		res_path = join_paths(program_path, _raw_uri);
		_uri = res_path;
		_uri_is_directory = false;
		_uri_is_regular_file = true;
		return res_path;
	}
	else
	{
		server_root_path = _config.get_token("/", "root");
		server_root_path = join_paths("/", server_root_path);
		server_root_path = join_paths(server_root_path, "/");
	
		location_root_path = _config.get_token(_raw_uri, "root");
		location_root_path = join_paths("/", location_root_path);
		location_root_path = join_paths(location_root_path, "/");
	}
	std::string common2 = common_path(server_root_path, location_root_path);
	size_t common2_length = common2.length();
	std::string unique_part_of_location_path = location_root_path.substr(common2_length);
	
	std::string unique_part_of_uri = _raw_uri;
	if (!unique_part_of_location_path.empty())
		unique_part_of_uri = _raw_uri.substr(common_length);

	res_path = join_paths(program_path, server_root_path);
	res_path = join_paths(res_path, unique_part_of_location_path);
	res_path = join_paths(res_path, unique_part_of_uri);
	
	if (is_directory(res_path))
	{
		std::string location_index = _config.get_token(_uri, "index");
		if (!location_index.empty())
		{
			_uri_is_directory = false;
			_uri_is_regular_file = true;
			res_path = join_paths(res_path, _config.get_token(_uri, "index"));
		}
		else if (_raw_uri.substr(0, _raw_uri.find_first_of('?')) == "/" || _raw_uri.substr(0, _raw_uri.find_first_of('?')).empty())
		{
			_uri_is_directory = false;
			_uri_is_regular_file = true;
			res_path = join_paths(res_path, "index.html");
		}
		else if (_raw_uri.length() >= 1 && _raw_uri[_raw_uri.length() - 1] == '/')
		{
			if (is_regular_file(join_paths(res_path, "index.html")))
			{
				_uri_is_directory = false;
				_uri_is_regular_file = true;
				res_path = join_paths(res_path, "index.html");
			}
			else
			{
				_uri_is_directory = true;
				_uri_is_regular_file = false;
			}
		}
		else
		{
			_uri_is_directory = true;
			_uri_is_regular_file = false;
		}
	}
	return res_path;
}

void Request::parse_request_line()
{
	// Trim leading whitespaces/newlines
	_raw_request = _raw_request.substr(_raw_request.find_first_not_of("\r\n\t "));

	if (_raw_request.empty())
	{
		std::cerr << "Empty request received." << std::endl;
		set_error_code(400);
		throw std::runtime_error("400 Bad Request");
	}

	try
	{
		if (_raw_request.find("\r\n") == std::string::npos)
		{
			_request_line = _raw_request;
			_one_line_request = 1;
		}
		else
		{
			_request_line = _raw_request.substr(0, _raw_request.find("\r\n"));
			_one_line_request = 0;
		}
		std::stringstream ss(_request_line);
		ss >> _method >> _uri >> _version;
		if (_method.empty() || _uri.empty() || _version.empty())
		{
			std::cerr << "Malformed request line: " << _request_line << std::endl;
			set_error_code(400);
			throw std::runtime_error("400 Bad Request");
		}
		if (_version != "HTTP/1.1" && _version != "HTTP/1.0" && _version != "undefined")
		{
			std::cerr << "Unsupported HTTP version: " << _version << std::endl;
			set_error_code(505);
			throw std::runtime_error("505 HTTP Version Not Supported");
		}
		if (_method != "GET" && _method != "POST" && _method != "DELETE")
		{
			std::cerr << "Unsupported HTTP method: " << _method << std::endl;
			set_error_code(501);
			throw std::runtime_error("501 Not Implemented");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

void Request::parse_uri()
{
	std::string stripped_uri;
	size_t separator;

	try 
	{
		if (_uri.find("../") != std::string::npos || _uri.find("..\\") != std::string::npos)
		{
			std::cerr << "The requested URL was rejected: " << _uri << std::endl;
			set_error_code(400);;// verifier si c'est 403 ou 400, ... ?
			throw std::runtime_error("400 Bad Request");
		}
		else if (_uri.length() > MAX_URI_LENGTH)
		{
			std::cerr << "The requested URL is too long: " << _uri << std::endl;
			_error_code = 414;
			throw std::runtime_error("414 URI Too Long");
		}
		else if (_uri.find(" ") != std::string::npos) // verifier si les espaces sont vmt invalides
		{
			std::cerr << "The requested URL contains spaces: " << _uri << std::endl;
			set_error_code(400);
			throw std::runtime_error("400 Bad Request");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return ;
	}
	separator = _uri.find('?');
	if (separator != std::string::npos)
	{
		stripped_uri = _uri.substr(0, separator);
		if (separator + 1 < _uri.length())
		{
			_uri_query = _uri.substr(separator + 1);
			separator = _uri_query.find('#');
			if (separator != std::string::npos)
			{
				if (separator + 1 < _uri_query.length())
					_uri_fragment = _uri_query.substr(separator + 1);
				else
					_uri_fragment = "";
				_uri_query = _uri_query.substr(0, separator);
			}
			else
			{
				_uri_fragment = "";
			}
		}
		else
			_uri_query = "";
		_uri = stripped_uri;
	} else { // no query part
		separator = _uri.find('#');
		if (separator != std::string::npos)
		{
			stripped_uri = _uri.substr(0, separator);
			if (separator + 1 < _uri.length())
				_uri_fragment = _uri.substr(separator + 1);
			_uri = stripped_uri;
		}
	}
	_uri_query = _uri_query;
	_uri_fragment = _uri_fragment;
}

void Request::normalize_uri()
{
	_raw_uri = get_uri();
	_computed_path = work_out_path();
}

std::string Request::get_uri_type(std::string path)
{
	if (is_directory(path))
		return "directory";
	else if (is_regular_file(path))
		return "regular_file";
	else
		return "nonexistent";
}

void Request::extract_resource_info()
{
	std::string extension, mime_type;

	// Get extension
	size_t dot_pos = _uri.find_last_of('.');
	if (dot_pos == std::string::npos || dot_pos == _uri.length() - 1)
	{
		extension = "";
		mime_type = "";
		_resource_info.insert(std::make_pair("extension", extension));
		_resource_info.insert(std::make_pair("mime_type", mime_type));
		return ;
	}
	else
	{
		extension = _uri.substr(dot_pos + 1);
		// Make extension lowercase
		for (size_t x = 0; x < extension.length(); x++)
			extension[x] = tolower(extension[x]);
	}
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

	_resource_info.insert(std::make_pair("extension", extension));
	_resource_info.insert(std::make_pair("mime_type", mime_type));
}

void Request::parse_headers()
{
	size_t pos = _raw_request.find("\r\n");
	size_t end_pos = _raw_request.find("\r\n\r\n");
	if (_method == "POST" && REQUIRE_HEADERS && (std::string::npos || end_pos == std::string::npos || end_pos <= pos))
	{
		std::cerr << "Malformed request: missing headers" << std::endl;
		set_error_code(400);
		return ;
	}
	if (pos == std::string::npos || end_pos == std::string::npos || end_pos <= pos)
	{
		std::cerr << "Malformed request: missing headers" << std::endl;
		set_error_code(400);
		return ;
	}
	_headers_string = _raw_request.substr(pos + 2, end_pos - (pos + 2));
	if (_headers_string.length() > MAX_HEADER_LENGTH)
	{
		std::cerr << "Headers too long: " << _headers_string.length() << " bytes" << std::endl;
		set_error_code(431);
		return ;
	}
	if (_headers_string.empty() && _method == "POST" && REQUIRE_HEADERS)
	{
		std::cerr << "Malformed request: empty headers" << std::endl;
		set_error_code(400);
		return ;
	}
	std::istringstream iss(_headers_string);
	std::string line;

	while (std::getline(iss, line))
	{
		if (line.empty())
			continue; // skip empty lines
		size_t pos = line.find(':');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1, line.find_last_not_of("\r\n") - pos);
			value.erase(0, value.find_first_not_of(" \t")); // trim leading whitespace
			_headers.insert(std::make_pair(key, value));
		}
	}
	_headers_string = _raw_request.substr(0, end_pos);
}

void Request::parse_body()
{
	size_t pos = _raw_request.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		std::cerr << "Malformed request: missing body" << std::endl;
		set_error_code(400);
		return ;
	}
	_body = _raw_request.substr(pos + 4);
	if (_body.length() > MAX_BODY_LENGTH)
	{
		std::cerr << "Body too long: " << _body.length() << " bytes" << std::endl;
		set_error_code(413);
	}
}

void Request::set_boundary()
{
	if (_raw_request.find("boundary=") == std::string::npos)
		_boundary = "";
	else
	{
		_boundary = _raw_request.substr(_raw_request.find("boundary=") + 9);
		_boundary = _boundary.substr(0, _boundary.find_first_of('\r'));
	}
}

void Request::set_actual_body_length(void)
{
	int content_length = 0;

	if (_headers.find("Content-Length") != _headers.end())
	{
		std::istringstream iss(_headers.find("Content-Length")->second);
		iss >> content_length;
	}
	else
	{
		std::cerr << "Content-Length header not found." << std::endl;
	}
	
	std::string begin_boundary, end_boundary, multipart_headers;
	
	begin_boundary = "--" + _boundary + "\r\n";
	end_boundary   = "--" + _boundary + "--\r\n";
	
	size_t begin_pos = _body.find(begin_boundary);
	if (begin_pos == std::string::npos)
	{
		_multipart_data = _body;
		multipart_headers = "";
		_actual_body_length = content_length;
		return ;
	}
	multipart_headers = _body.substr(begin_pos + begin_boundary.length());
	multipart_headers = multipart_headers.substr(0, multipart_headers.find("\r\n\r\n") + 4);
	
	begin_pos = _body.find("\r\n\r\n");
	if (begin_pos == std::string::npos)
	{
		_multipart_data = _body;
		_actual_body_length = content_length;
		return ;
	}
	_multipart_data = _body.substr(begin_pos + 4);

	_actual_body_length  = content_length - (begin_boundary.length() + end_boundary.length());
	_actual_body_length -= multipart_headers.length();
	_actual_body_length -= 2; // for the \r\n before the closing boundary
}

bool Request::is_complete()
{
	if (!_headers_parsed)
	{
		if (_raw_request.find("\r\n\r\n") == std::string::npos)
			return false;
		else
		{
			parse_request_line();
			parse_uri();
			normalize_uri();
			if (get_uri_type(_computed_path) == "directory")
				set_uri_is_directory(true);
			else
			{
				set_uri_is_directory(false);
				if (get_uri_type(_computed_path) == "regular_file")
				{
					set_uri_is_regular_file(true);
					extract_resource_info();
				}
				else
					set_uri_is_regular_file(false);
			}
			if (!_one_line_request)
				parse_headers();
			else
				_actual_body_length = 0;
			_headers_parsed = true;
		}
	}
	if (_headers_parsed)
	{
		if (_headers.find("Content-Length") == _headers.end())
			return true;
		else
		{
			std::string body = _raw_request.substr(_raw_request.find("\r\n\r\n") + 4);
			size_t content_length = 0;
			std::istringstream iss(_headers.find("Content-Length")->second);
			iss >> content_length;

			size_t client_body_size;
			std::string client_body_size_str = _config.get_token(_uri, "client_body_size");
			if (client_body_size_str.empty())
				client_body_size = MAX_BODY_LENGTH;
			else
			{
				std::istringstream cbss(_config.get_token(_uri, "client_body_size"));
				cbss >> client_body_size;
			}
			if (content_length > client_body_size)
			{
				std::cerr << "Client body size limit: " << client_body_size << " bytes" << std::endl;
				std::cerr << "Content-Length exceeds client_body_size limit: " << content_length << " bytes" << std::endl;
				_error_code = 413;
				return true;
			}
			size_t body_length = body.size();
			if (body_length < content_length)
				return false;
			else if (body_length > content_length)
			{
				std::cerr << "Request body too long: " << body_length << " bytes" << std::endl;
				set_error_code(413);
				return true;
			}
			else
			{
				parse_body();
				if (_method == "POST" && _headers.find("Content-Length") != _headers.end())
				{
					set_boundary();
					set_actual_body_length();
				}
				else
					_actual_body_length = 0;
				return true;
			}
		}
	}
	return true;
}

void Request::set_error_code(int error_code)
{
	// set error code only if no other error was previously encountered
	if (_error_code == 0)
		_error_code = error_code;
}
