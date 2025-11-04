/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/11/04 15:23:09 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../cgi/CgiHandler.hpp"

Response::Response(const int fd, Request &req): _fd(fd), _req(req)
{
	_config = req.get_config();

	try
	{
		if (_req.get_error_code() != 0)
		{
			std::string error_msg;
			std::stringstream ss(error_msg);
			ss << "HTTP/1.1 " << _req.get_error_code() << " Error\r\n\r\n";
			_response = ss.str();
			return ;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return ;
	}
	_file_error = false;
	if (_req.get_method() == "GET")
		process_get_request();
	else if (_req.get_method() == "POST")
		process_post_request();
	else if (_req.get_method() == "DELETE")
		process_delete_request();	
}

Response::~Response()
{}

void Response::print_dir_listing(std::string &dir_path)
{
	std::vector<std::string> files;
	
	std::string body = "<html><head><meta charset=\"UTF-8\"><title>Index of " + _req.get_raw_uri() + "</title></head><body>";
	if (get_dir_content(dir_path, files) != -1)
	{
		_status_line = "200 OK";
		body += "<h1>Index of " + _req.get_raw_uri() + "</h1>\n";
		for (size_t i = 0; i < files.size(); i++)
		{
			if (files[i] == ".")
				continue ;
			else if (files[i] == "..")
			{
				body += "<p><a href=\"..\">⬑ Parent Directory</a></p>\n";
				continue ;
			}
			body += "<p><a href=\"" + files[i] + "\">" + files[i] + "</a></p>\n";
		}
	}
	else
	{
		std::cerr << "Failed to open directory: " << dir_path << std::endl;
		set_error_page("500", "Internal Server Error", "");
		return;
	}
	body += "</body></html>\r\n\r\n";
	_body = body;
	std::string content_type = " text/html";
	std::stringstream response;
	response << "HTTP/1.1 " << _status_line << "\r\n";
	response << "Host: " << _config.get_token("", "server_name") << "\r\n";
	response << "Date: " << get_http_date() << "\r\n";
	response << "Content-Type: " << content_type << "\r\n";
	response << "Content-Length: " << _body.length() << "\r\n";
	_headers_string = response.str();
	response << "\r\n";
	response << _body;
	_response = response.str();
}

void Response::get_dir()
{
	std::string index, index_path, dir_path, error_msg, error_page;

	// build full directory path
	_config = _req.get_config();
	//dir_path = join_paths(server_path(), _req.get_uri());

	dir_path = join_paths(server_path(), _config.get_token(_req.get_uri(), "root"));
	//dir_path = join_paths(dir_path, _req.get_raw_uri());

	// subtract "root" part from uri to avoid duplication
	dir_path = join_paths(dir_path, _req.get_uri().substr(_config.get_token(_req.get_uri(), "root").length()));

	
	std::string file_type;

	file_type = get_file_type(dir_path);
	
	std::cout << "File type: " << file_type << std::endl;
	std::cout << "Full directory path: " << dir_path << std::endl;
	if (file_type == "nonexistent")
	{
		error_msg = "Directory does not exist: " + dir_path + "\n";
		std::cerr << error_msg;
		set_error_page("404", "Not Found", "");
		return ;
	}
	else
	{
		error_msg = "File type: " + file_type + "\n";
		std::cerr << error_msg;
	}
	//std::string location = _config.get_token(_req.get_uri(), "location");
	
	//std::string location = _config.get_locations().end()->first;
	//std::cout << "Location: " << location << std::endl;

	std::string server_path_part = server_path();
	std::string config_root_part = _config.get_token(_req.get_uri(), "root");
	std::string raw_uri_part = _req.get_raw_uri();
	
	// join config_root_part and raw_uri_part by removing duplicate directories if any
	// e.g. if config_root_part = /var/www/html and raw_uri_part = /html/images
	// then we remove /html from raw_uri_part to avoid /var/www/html/html/images
	// but if raw_uri_part = /images, we keep it as is
	if (raw_uri_part.find(config_root_part) == 0)
		raw_uri_part = raw_uri_part.substr(config_root_part.length());
	std::string pathtest = join_paths(server_path_part, config_root_part);
	pathtest = join_paths(pathtest, raw_uri_part);
	//std::cout << "Test Directory path : " << pathtest << std::endl;
	//std::cout << "-    -    -    -" << std::endl;
	//std::cout << "Directory path (part 1/3): " << server_path() << std::endl;
	//std::cout << "Directory path (part 2/3): " << _config.get_token(_req.get_uri(), "root") << std::endl;
	//std::cout << "Directory path (part 3/3): " << _req.get_raw_uri() << std::endl;
	//std::cout << "Full directory path: " << dir_path << std::endl;
	// check if index or location-specific index
	index = _config.get_token(_req.get_uri(), "index");
	if (!index.empty())
	{
		index_path = join_paths(dir_path, index);
		if (get_file_type(index_path) == "nonexistent")
		{
			
			// check for autoindex option
			std::string option = _config.get_token(_req.get_uri(), "autoindex");
			
			if (_config.get_token(_req.get_uri(), "autoindex") == "on")
			{
				print_dir_listing(dir_path);
				return ;
			}
			else
			{
				error_msg = "Index file does not exist: " + index_path + "\n";
				std::cerr << error_msg;
				set_error_page("403", "Forbidden", "");
				return ;
			}
		}
		else
		{
			std::fstream file(index_path.c_str(), std::ios::in | std::ios::binary);
			if (file.fail())
			{
				error_msg = "Failed to open index file: " + index_path + "\n";
				std::cerr << error_msg;
				set_error_page("403", "Forbidden", "");
				return ;
			}
			else
			{
				_status_line = "200 OK";
				_file_error = true;
				build_response(file);
				return ;
			}
		}
	}
	else
	{
		// check if autoindex on
		if (_config.get_token(_req.get_uri(), "autoindex") == "on")
		{
			// generate directory listing
			print_dir_listing(dir_path);
			_status_line = "200 OK";
		}
		else
		{
			// autoindex off, error 403
			error_msg = "Autoindex is off for this directory: " + _req.get_uri() + "\n";
			std::cerr << error_msg;
			set_error_page("403", "Forbidden", "");
		}
	}
}

void Response::get_file()
{
	std::string index, path, error_msg, error_page;
	
	//                  server root   , config root + path
	path = join_paths(server_path(), _req.get_uri());
	std::fstream file(path.c_str(), std::ios::in | std::ios::binary);
	_file_error = false;
	if (file.fail())
	{
		_file_error = true;
		if (errno == 2) // No such file or directory (404)
		{
			set_error_page("404", "Not Found", "");
			return;
		}
		else if (errno == 13) // Permission denied (403)
		{
			set_error_page("403", "Forbidden", "");
			return;
		}
		else if (errno == 21) // Is a directory (ou tenter 20, si c'est pas 21)
		{
			// Opening a directory failed ; aussi erreur 403 ?
			error_msg =  "Failed opening a directory";//DEBUG
			//_status_line = "403 Forbidden";//403???
		}
		else
		{
			error_msg = "Unknown error: " + path + "\n";
		}
		
		std::cerr << "errno : " << errno << std::endl;
		std::cerr << error_msg;
		
		std::string err_path;
		err_path = join_paths(server_path(), "/data/error_pages/");
		err_path = join_paths(err_path, error_page);
		std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
		build_response(file_err);
	}
	else
	{
		_status_line = "200 OK";
		build_response(file);
	}
}

void Response::process_get_request()
{
	std::string allowed_methods = _config.get_token(_req.get_raw_uri(), "method");
	
	if (!is_allowed_method("GET"))
	{
		handle_405();
		return ;
	}
	bool is_dir = _req.get_uri_is_directory();
	bool is_file = _req.get_uri_is_regular_file();
	if (is_dir)
	{
		get_dir();
	}
	else if (is_file)
	{
		get_file();
	}
	else
	{
		set_error_page("404", "Not Found", "");
		return;
	}
}

void Response::process_post_request()
{
	std::string path, error_msg, error_page;
	
	if (!is_allowed_method("POST"))
	{
		handle_405();
		return ;
	}
	
	if (_req.get_boundary().empty())
	{
		handle_single_part_post();
		return ;
	}
	std::string boundary;
	// status_line, response;
	
	boundary = _req.get_boundary();

	if (boundary.length() > 70)
	{
		std::cerr << "Boundary too long: " << boundary.length() << " characters" << std::endl;
		_status_line = "400 Bad Request";
		_response = "HTTP/1.1 " + _status_line + "\r\n\r\n";
		// _response = response;
		return ;
	}
	else if (boundary.length() > 0)
	{
		handle_multipart_post();
	}
}

void Response::process_delete_request()
{
	std::string path, error_msg, error_page;
	
	if (!is_allowed_method("DELETE"))
	{
		handle_405();
		return ;
	}

	// Basic implementation for now
    int status = remove(_req.get_uri().c_str());

    if (status != 0) {
        perror("Error deleting file");
    }
    else {
        std::cout << "File successfully deleted : " << _req.get_uri() << std::endl;
    }
	
	_response = "...";
}

void Response::build_response(std::fstream &path)
{
	std::string content_type;

	if (_file_error)
		content_type = "text/html";
	else
		content_type = _req.get_resource_info().find("mime_type")->second;
	
	if (path.is_open())
	{
		_body = std::string((std::istreambuf_iterator<char>(path)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 " << _status_line << "\r\n";
		response << "Host: " << _config.get_token("", "server_name") << "\r\n";
		response << "Date: " << get_http_date() << "\r\n";
		response << "Content-Type:" << content_type << "\r\n";
		response << "Content-Length: " << _body.length() << "\r\n";
		_headers_string = response.str();
		response << "\r\n";
		response << _body;
		_response = response.str();
		path.close();
	}
	else
	{
		std::cerr << "Failed to open file for reading." << std::endl;
		set_error_page("500", "Internal Server Error", "");
	}
}

void Response::handle_single_part_post()
{
	std::string body = _req.get_body();
	if (body.empty())
		return ;
	else
		for (size_t i = 0; i < body.length(); ++i)
			std::cout << body[i];
}

int Response::get_dir_content(std::string dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
		// Error 403 ? 500 ?
		return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

// fonctionne avec requete unique
void Response::handle_multipart_post()
{
	std::string post_data, filename, path;

	post_data = _req.get_multipart_data();

	filename = _req.get_raw_request().substr(_req.get_raw_request().find("filename=\"") + 10);
	filename = filename.substr(0, filename.find_first_of('"'));
	if (filename.empty())
	{
		std::cerr << "No filename provided in POST request" << std::endl;
		set_error_page("400", "Bad Request", "");
		return ;
	}

	path = join_paths(server_path(), _config.get_token(_req.get_uri(), "root"));

	path = join_paths(path, UPLOAD_PATH);

	mkdir(path.c_str(), 0755);

	path = join_paths(path, filename);

	std::fstream file(path.c_str(), std::ios::out | std::ios::binary);
	if (file.is_open())
	{
		for (size_t i = 0; i < _req.get_actual_body_length(); ++i)
		{
			file << post_data[i];
		}
		file.close();
		_status_line = "201 Created";
	}
	else
	{
		set_error_page("500", "Internal Server Error", "");
	}
	_response = "HTTP/1.1 " + _status_line + "\r\n\r\n";
}

const std::string Response::get_http_date() {
    const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    std::time_t now = std::time(NULL);
    std::tm* gmt = std::gmtime(&now);

    std::ostringstream oss;
    oss << days[gmt->tm_wday] << ", ";
    if (gmt->tm_mday < 10) oss << '0';
    oss << gmt->tm_mday << ' ';
    oss << months[gmt->tm_mon] << ' ';
    oss << (1900 + gmt->tm_year) << ' ';
    if (gmt->tm_hour < 10) oss << '0';
    oss << gmt->tm_hour << ':';
    if (gmt->tm_min < 10) oss << '0';
    oss << gmt->tm_min << ':';
    if (gmt->tm_sec < 10) oss << '0';
    oss << gmt->tm_sec << " GMT";

    return oss.str();
}

void Response::set_error_page(std::string nb, std::string name, std::string header)
{
	_status_line = nb + " " + name;
	std::string err_page = "err_page_";
	err_page += nb;
	std::string err_path = _config.get_token(_req.get_uri(), err_page.c_str());
	std::string webroot_path = _config.get_token(_req.get_uri(), "root");
	std::string path = join_paths(server_path(), webroot_path);
	path = join_paths(path, err_path);

	std::fstream file_err(path.c_str(), std::ios::in | std::ios::binary);
	if (!err_path.empty() && !file_err.fail() && file_err.is_open())
	{
		_body = std::string((std::istreambuf_iterator<char>(file_err)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 " << _status_line << "\r\n";
		response << "Host: " << _config.get_token("", "server_name") << "\r\n";
		response << "Date: " << get_http_date() << "\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << _body.length() << "\r\n";
		_headers_string = response.str();
		response << "\r\n";
		response << _body;
		_response = response.str();
		file_err.close();
	}
	else
	{
		_body = "<!DOCTYPE html>" \
			"<html lang=\"en\">" \
				"<head>" \
					"<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"favicon.ico\">" \
					"<title>" + nb + " - WS Homepage</title>" \
				"</head>" \
				"<body style=\"background-color: grey;\">" \
					"<h1 style=\"text-align: center;\">" \
						"<span style=\"text-decoration: underline;\">" \
							"<strong>GENERIC ERROR " + nb +"</strong>" \
						"</span>" \
					"</h1>" \
					"<p style=\"text-align: center;\">" \
						+ name + \
					"</p>" \
					"<p style=\"text-align: center;\">" \
						"<a href=\"/\">Go back to index page</a>" \
					"</p>" \
				"</body>" \
			"</html>" \
			;

		std::stringstream response;
		response << "HTTP/1.1 " << _status_line << "\r\n";
		response << "Host: " << _config.get_token("", "server_name") << "\r\n";
		response << "Date: " << get_http_date() << "\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << _body.length() << "\r\n";
		if (!header.empty())
			response << header << "\r\n";
		_headers_string = response.str();
		response << "\r\n";
		response << _body;
		_response = response.str();
	}
}

void Response::handle_405()
{
	std::string allowed;
	bool	sep = false;

	if (_config.get_token(_req.get_uri(), "GET") == "true")
	{
		allowed.append("GET");
		sep = true;
	}
	if (_config.get_token(_req.get_uri(), "POST") == "true")
	{
		if (sep)
			allowed.append(", ");
		allowed.append("POST");
		sep = true;
	}
	if (_config.get_token(_req.get_uri(), "DELETE") == "true")
	{
		if (sep)
			allowed.append(", ");
		allowed.append("DELETE");
	}

	set_error_page("405", "Method Not Allowed", "Allow : " + allowed);
}

bool Response::is_allowed_method(const std::string& method) {
	std::string allowed_method = _config.get_token(_req.get_uri(), method.c_str());

	if (allowed_method == "false")
		return false;
	return true;
}
