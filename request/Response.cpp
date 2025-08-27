/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/08/27 15:55:32 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

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
			
			// redirect to error page if needed
			return ;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return ;
	}

	// HERE : process redirection to CGI if needed (based on requested resource extension ?)

	if (_req.get_method() == "GET")
		process_get_request();
	else if (_req.get_method() == "POST")
		process_post_request();
	else if (_req.get_method() == "DELETE")
		process_delete_request();
}

Response::~Response()
{}

void Response::get_directory()
{
	std::string path = _req.get_uri();
	std::vector<std::string> files;
	
	if (getdir(path, files) != -1)
	{
		std::string body;
		for (size_t i = 0; i < files.size(); i++)
			body += files[i] + "\n";
		_body = body;
	}
	else
	{
		std::cerr << "Failed to open directory: " << path << std::endl;
		_body = "Failed to open directory: " + path + "\n";
	}
}

void Response::process_get_request()
{
	std::cerr << "Entered process_get_request()" << std::endl;//DEBUG
	bool is_dir = _req.get_uri_is_directory();
	bool is_file = _req.get_uri_is_regular_file();
	
	if (is_dir)
	{
		std::cout << "Requested resource is a directory: " << _req.get_uri() << std::endl;
		//return ;
	}
	else if (is_file)
		std::cout << "Requested resource is a regular file: " << _req.get_uri() << std::endl;
	else
		std::cout << "Requested resource is neither a directory nor a regular file: " << _req.get_uri() << std::endl;
	
	std::string path, error_msg, error_page;
	// std::string status_line;
	
	//                    server root       , config root + path
	path = join_paths(root_directory(), _req.get_uri());
	std::fstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (file.fail())
	{
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
			std::cout << "Failed opening a directory" << std::endl;//DEBUG
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
		err_path = join_paths(root_directory(), "/data/error_pages/"); // to do : aussi gerer pages d erreur custom de la config
		err_path = join_paths(err_path, error_page);
		std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
		build_response(file_err);
	}
	else
	{
		_status_line = "200 OK";
		build_response(file);
	}
	std::cerr << "Exited process_get_request()" << std::endl;//DEBUG
}

void Response::process_post_request()
{
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
	std::cerr << "Entered build_response()" << std::endl;//DEBUG
	if (path.is_open())
	{
		// std::string body;
		
		_body = std::string((std::istreambuf_iterator<char>(path)), std::istreambuf_iterator<char>());
		std::stringstream response;

		response << "HTTP/1.1 " << _status_line << "\r\n";
		response << "Host: " << _config.get_token("", "server_name") << "\r\n";
		response << "Date: " << get_http_date() << "\r\n";
		response << "Content-Type:" << _req.get_resource_info().find("mime_type")->second << "\r\n";
		response << "Content-Length: " << _body.length() << "\r\n";
		//response << "Connection: keep-alive\r\n";
		//response << "Cache-Control: no-store\r\n";
		_headers_string = response.str();
		response << "\r\n";
		response << _body;
		_response = response.str();
	}
	else
	{
		std::cerr << "Failed to open file for reading." << std::endl;
		_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
	}
	path.close();
	std::cerr << "Exited build_response()" << std::endl;//DEBUG
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

// TO BE TESTED
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
