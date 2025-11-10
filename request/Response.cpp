/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcygan <mcygan@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/11/10 15:18:04 by mcygan           ###   ########.fr       */
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

void Response::get_directory(std::string &dir_path)
{
	std::vector<std::string> files;
	
	std::string body = "<html><head><title>Index of " + _req.get_raw_uri() + "</title></head><body>";
	if (getdir(dir_path, files) != -1)
	{
		_status_line = "200 OK";
		body += "<h1>Index of " + _req.get_raw_uri() + "</h1>\n";
		for (size_t i = 0; i < files.size(); i++)
		{
			if (files[i] == ".")
				continue ;
			body += "<p><a href=\"" + files[i] + "\">" + files[i] + "</a></p>\n";
		}
	}
	else
	{
		_status_line = "500 Internal Server Error";
		std::cerr << "Failed to open directory: " << dir_path << std::endl;
		body += "Failed to open directory: " + dir_path + "\n";
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

	_config = _req.get_config();
	dir_path = join_paths(root_directory(), _req.get_uri());

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
				get_directory(dir_path);
				return ;
			}
			else
			{
				error_msg = "Index file does not exist: " + index_path + "\n";
				error_page = "error_403.html";
				_status_line = "403 Forbidden";
				_file_error = true;
				std::cerr << error_msg;
				std::string err_path;
				err_path = join_paths(root_directory(), "/data/error_pages/");
				err_path = join_paths(err_path, error_page);
				std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
				build_response(file_err);
				return ;
			}
		}
		else
		{
			std::fstream file(index_path.c_str(), std::ios::in | std::ios::binary);
			if (file.fail())
			{
				error_msg = "Failed to open index file: " + index_path + "\n";
				error_page = "error_403.html";
				_status_line = "403 Forbidden";
				std::cerr << error_msg;
				std::string err_path;
				err_path = join_paths(root_directory(), "/data/error_pages/");
				err_path = join_paths(err_path, error_page);
				std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
				build_response(file_err);
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
			get_directory(dir_path);
			_status_line = "200 OK";
		}
		else
		{
			// autoindex off, error 403
			error_msg = "Autoindex is off for this directory: " + _req.get_uri() + "\n";
			error_page = "error_403.html";
			_status_line = "403 Forbidden";
			std::cerr << error_msg;
			std::string err_path;
			err_path = join_paths(root_directory(), "/data/error_pages/");
			err_path = join_paths(err_path, error_page);
			std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
			build_response(file_err);
		}
	}
}

void Response::get_file()
{
	std::string index, path, error_msg, error_page;
	
	//                  server root   , config root + path
	path = join_paths(root_directory(), _req.get_uri());
	std::fstream file(path.c_str(), std::ios::in | std::ios::binary);
	_file_error = false;
	if (file.fail())
	{
		_file_error = true;
		if (errno == 2) // No such file or directory (404)
		{
			error_msg = "File Not Found: " + path + "\n";
			error_page = "error_404.html";
			_status_line = "404 Not Found";
		}
		else if (errno == 13) // Permission denied (403)
		{
			error_msg = "Permission Denied: " + path + "\n";
			error_page = "error_403.html";
			_status_line = "403 Forbidden";
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
		err_path = join_paths(root_directory(), "/data/error_pages/");
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

bool Response::is_cgi()
{
	if ((_req.get_uri()).rfind("./cgi-bin/", 0) == 0)
		return true;
	else if ((_req.get_uri()).rfind("/cgi-bin/", 0) == 0)
		return true;
	else if ((_req.get_uri()).rfind("cgi-bin/", 0) == 0)
		return true;
	return false;
}

void Response::run_script()
{
	CgiHandler			cgi(_req);
	std::string			body;
	std::stringstream	response;
	
	body = cgi.executeCgi(_req.get_uri());
	if (body == "500 Internal server error\r\n\r\n")
	{
		_response = body;
		return;
	}
	response << "HTTP/1.1 200 OK\r\n";
	/* response << "Host: " << _config.get_token("", "server_name") << "\r\n";
	response << "Date: " << get_http_date() << "\r\n"; */
	response << "Content-Type:" << "text/html" << "\r\n";
	response << "Content-Length: " << body.length() << "\r\n\r\n";
	response << body;
	_response = response.str();
}

void Response::process_get_request()
{
	std::string allowed_methods = _config.get_token(_req.get_raw_uri(), "method");
	
	//std::cout << "HERE: " + _req.get_uri() << std::endl;
	if (!allowed_methods.empty() && allowed_methods.find("GET") == std::string::npos)
	{
		_status_line = "405 Method Not Allowed";
		_response = "HTTP/1.1 " + _status_line + "\r\n\r\n";
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
		if (this->is_cgi())
			this->run_script();
		else
			get_file();
	}
	else
	{
		std::string error_page = "error_404.html";
		_status_line = "404 Not Found";
		_file_error = true;
		std::string err_path;
		err_path = join_paths(root_directory(), "/data/error_pages/");
		err_path = join_paths(err_path, error_page);
		std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
		build_response(file_err);
	}
}

void Response::process_post_request()
{
	std::string path, error_msg, error_page;
	
	if (!_config.is_allowed("POST"))
	{
		errno = 405;
		_status_line = "405 Method not allowed";
		std::cerr << "405 Method not allowed" << std::endl;
		std::string err_path = 
			root_directory() + "/data/error_pages/error_" + _status_line.std::string::substr(0, 3) + ".html"; 
		std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
		build_response(file_err);
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
	std::string path = join_paths(root_directory(), _req.get_uri());
	
	if (!_config.is_allowed("DELETE"))
	{
		errno = 405;
		_status_line = "405 Method not allowed";
		std::cerr << "405 Method not allowed" << std::endl;
		std::string err_path = 
			root_directory() + "/data/error_pages/error_" + _status_line.std::string::substr(0, 3) + ".html"; 
		std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
		build_response(file_err);
		return ;
	}

	if (_req.get_uri_is_directory())
	{
		// temp
		_status_line = "204 No content";
		_response = "HTTP/1.1" +_status_line + "\r\n\r\n";
	}
	else
	{
		if (remove(path.c_str()))
		{
			_status_line = "204 No content";
			_response = "HTTP/1.1" +_status_line + "\r\n\r\n";
		}
		else
		{
			_status_line = "200 OK";
			build_response_delete(_req.get_uri());
		}
	}
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
		_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
	}
}

void Response::build_response_delete(std::string path)
{	
	std::stringstream response;
	
	std::string	body;
	if (_status_line == "200 OK")
		body = "<html lang=\"en-US\">\n\t<body>\n\t\t<h1>File " + path + " deleted.</h1>\n\t</body>\n</html>";
	else
		body = "<html lang=\"en-US\">\n\t<body>\n\t\t<h1>File " + path + " not deleted.</h1>\n\t</body>\n</html>";

	response << "HTTP/1.1 " << _status_line << "\r\n";
	response << "Host: " << _config.get_token("", "server_name") << "\r\n";
	response << "Date: " << get_http_date() << "\r\n";
	response << "Content-Type:" << _req.get_resource_info().find("mime_type")->second << "\r\n";
	response << "Content-Length: " << _body.length() << "\r\n";
	_headers_string = response.str();
	response << "\r\n";
	response << _body;
	_response = response.str();
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

int Response::getdir (std::string dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
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
		_status_line = "400 Bad Request";
		_response = "HTTP/1.1 " + _status_line + "\r\n\r\n";
		return ;
	}

	path = join_paths(root_directory(), _config.get_token(_req.get_uri(), "root"));

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
		std::cerr << "Failed to open file for writing: " << path << std::endl;
		_status_line = "500 Internal Server Error";
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
