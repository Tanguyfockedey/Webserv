/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/04 16:11:30 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


	// std::string method, path, version;

	// // Parse request line
	// std::string request_line = _req.get_raw_request().substr(0, _req.get_raw_request().find("\r\n"));
	// std::istringstream iss(request_line);
	// iss >> method >> path >> version;

	// // Normalize path
	// if (path == "/" || path.empty())
	// 	path = "index.html";
	// else if (path[0] == '/')
	// 	path = path.substr(1); // remove leading slash

Response::Response(const int fd, Request &req): _fd(fd), _req(req)
{
	std::string uri;
	std::string host;
	std::string dir_path;

	_raw_response.clear();// = "";
	uri.clear();
	
	uri = _req.get_raw_request().substr(0, _req.get_raw_request().find_first_of('\n'));
	uri = uri.substr(uri.find_first_of(' '));
	uri = uri.substr(1, uri.find_last_of(' '));
	uri.erase(uri.find_last_not_of(" \t\r\n") + 1); // Remove trailing whitespace
	uri.erase(0, uri.find_first_not_of(" \t\r\n")); // Remove leading whitespace

	if (uri == "/")
	{
		uri = "/index.html";
	}

	// Get extension
	std::string extension = uri.substr(uri.find_last_of('.') + 1);

	// Make extension lowercase
	for (size_t x = 0; x < extension.length(); x++)
		extension[x] = tolower(extension[x]);
	
	// Set MIME type
	std::string mime_type;
	if (extension == "html" || extension == "htm")
		mime_type = "text/html";
	else if (extension == "ico")
		mime_type = "image/x-icon";
	else if (extension == "txt")
		mime_type = "text/plain";
	else if (extension == "css")
		mime_type = "text/css";
	else if (extension == "js")
		mime_type = "application/javascript";
	else if (extension == "png")
		mime_type = "image/png";
	else if (extension == "jpg" || extension == "jpeg")
		mime_type = "image/jpeg";
	else if (extension == "gif")
		mime_type = "image/gif";
	else if (extension == "svg")
		mime_type = "image/svg+xml";
	else
	{
		std::cout << "Unknown extension: _" << extension << "_" << std::endl;
		mime_type = "application/octet-stream"; // Default MIME type
	}

	//std::cout << "MIME-type : " << mime_type << std::endl;
	
	// host = _req.get_raw_request().substr(_req.get_raw_request().find_first_of('\n'));
	// host = host.substr(1);
	// host = host.substr(0, host.find_first_of('\n'));
	// host = host.substr(host.find_first_of(':'));
	// host = host.substr(2);
	// host.erase( std::remove(host.begin(), host.end(), '\r'), host.end() );
	// host = "http://" + host + "/www"; // Later, will need to manage https as well !

	dir_path = std::string(get_current_dir_name()) + "/www" + uri;
	std::fstream file(dir_path.c_str(), std::ios::in | std::ios::binary);

	// Check if I have the permission to read the file
	if (file.fail())
	{
		std::cout << "Failed to open file: " << dir_path << std::endl;
		std::cout << "Error: " << strerror(errno) << std::endl;
		std::string err_path = std::string(get_current_dir_name()) + "/pages/error_403.html";
		// Try to open the error page
		std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
		if(file_err.is_open())
		{
			std::string body = std::string((std::istreambuf_iterator<char>(file_err)), std::istreambuf_iterator<char>());
			std::string date = get_http_date();
			std::stringstream response;
			response << "HTTP/1.1 403 Forbidden\r\n";
			response << "Date: " << date << "\r\n";
			response << "Content-Type: text/html\r\n";
			response << "Content-Length: " << body.length() << "\r\n";
			response << "Connection: keep-alive\r\n";// vs close
			response << "Cache-Control: no-store\r\n";
			response << "\r\n";
			response << body;
			_raw_response = response.str();
			file_err.close();
			response.str("");
			response.clear();
			uri.clear();
			body.clear();
			dir_path.clear();
			return;
		}
		std::cout << "Failed to open error page: " << err_path << std::endl;
		std::cout << "Error: " << strerror(errno) << std::endl;
		std::cout << "File Not Found" << std::endl;
	}
	if(file.is_open())
	{
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string date = get_http_date();
		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n";
		response << "Date: " << date << "\r\n";
		response << "Content-Type: " << mime_type << "\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "Connection: keep-alive\r\n";// vs close
		response << "Cache-Control: no-store\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file.close();
		response.str("");
		response.clear();
		uri.clear();
		body.clear();
		dir_path.clear();
		return;
	}
	std::string err_path = std::string(get_current_dir_name()) + "/pages/error_404.html";
	std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
	if(file_err.is_open())
	{
		std::string body = std::string((std::istreambuf_iterator<char>(file_err)), std::istreambuf_iterator<char>());
		std::string date = get_http_date();
		std::stringstream response;
		response << "HTTP/1.1 404 Not Found\r\n";
		response << "Date: " << date << "\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "Connection: keep-alive\r\n";
		response << "Cache-Control: no-store\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file_err.close();
		response.str("");
		response.clear();
		uri.clear();
		body.clear();
		dir_path.clear();
		return;
	}
	std::cout << "Failed path : " << dir_path << std::endl;
	std::cout << "File Not Found" << std::endl;
	
	// for (size_t i = 0; i < dir_path.length(); ++i)
	// {
	// 	std::cout << "[" << dir_path[i] << "](" << (int)dir_path[i] << ") ";
	// }
	// std::cout << std::endl;
}

Response::~Response()
{}

std::string Response::get_http_date() {
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
