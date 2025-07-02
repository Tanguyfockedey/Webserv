/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/02 01:42:36 by jrichir          ###   ########.fr       */
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

	_raw_response = ""; // TEST
	uri.clear();
	
	uri = _req.get_raw_request().substr(0, _req.get_raw_request().find_first_of('\n'));
	uri = uri.substr(uri.find_first_of(' '));
	uri = uri.substr(1, uri.find_last_of(' '));
	uri.erase(uri.find_last_not_of(" \t\r\n") + 1); // Remove trailing whitespace
	uri.erase(0, uri.find_first_not_of(" \t\r\n")); // Remove leading whitespace

	// Get extension
	std::string extension = uri.substr(uri.find_last_of('.') + 1);

	// Set MIME type
	std::string mime_type;
	if (extension == "html")
		mime_type = "text/html";
	else if (extension == "ico")
		mime_type = "image/x-icon";
	else if (extension == "txt")
		mime_type = "text/plain";

	host = _req.get_raw_request().substr(_req.get_raw_request().find_first_of('\n'));
	host = host.substr(1);
	host = host.substr(0, host.find_first_of('\n'));
	host = host.substr(host.find_first_of(':'));
	host = host.substr(2);
	host.erase( std::remove(host.begin(), host.end(), '\r'), host.end() );
	host = "http://" + host + "/www"; // Later, will need to manage https as well !

	if (uri.length() < 5)
	{
		std::cout << "Too short URI, replaced by index.html" << std::endl;
		uri = "/index.html";
	}
	dir_path = std::string(get_current_dir_name()) + "/www" + uri;
	std::fstream file(dir_path.c_str(), std::ios::in | std::ios::binary);
	if(file.is_open())
	{
		std::cout << "requested : " << dir_path << std::endl;
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response.clear();
		response << "HTTP/1.1 200 OK\r\n";
		response << "Content-Type: " + mime_type + "\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file.close();
		file.clear();
		uri.clear();
		body.clear();
		response.clear();
		return;
	}
	std::string err_path = std::string(get_current_dir_name()) + "/pages/error_404.html";
	std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
	if(file_err.is_open())
	{
		std::cout << "requested : " << dir_path << std::endl;
		std::string body = std::string((std::istreambuf_iterator<char>(file_err)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 404 Not Found\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file_err.close();
		file_err.clear();
		uri.clear();
		body.clear();
		response.clear();
		return;
	}
	std::cout << "Failed path : " << dir_path << std::endl;
	std::cout << "File Not Found" << std::endl;
	
	for (size_t i = 0; i < dir_path.length(); ++i)
	{
		std::cout << "[" << dir_path[i] << "](" << (int)dir_path[i] << ") ";
	}
	std::cout << std::endl;
}

Response::~Response()
{}

// // Send an HTML file as the response
// Response& html(const std::string& path){
// 	header("Content-Type", "text/html");
// 	std::fstream file(path, std::ios::in | std::ios::binary);
// 	if(file.is_open()){
// 		body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
// 		file.close();
// 	} else {
// 		status_code(404) << "File Not Found";
// 	}
// 	return *this;
// }