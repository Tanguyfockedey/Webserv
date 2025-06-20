/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/21 00:48:14 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(const int fd): _fd(fd)
{
	std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("/"));
	dir_path.append("/../www/index.html");
	std::fstream file(dir_path.c_str(), std::ios::in | std::ios::binary);
	std::cout << dir_path << std::endl; // TEMP: print path to console
	if(file.is_open())
	{
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		// Add a Date header
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		// _raw_response.append("HTTP/1.1 200 OK\r\n");
		// _raw_response.append("Content-Type: text/html\r\n");
		// _raw_response.append("Content-Length: ");
		// _raw_response.append(std::to_string(body.length()));
		// _raw_response.append("\r\n");
		// _raw_response.append("\r\n");
		// _raw_response.append(body);
		file.close();
	} else {
		std::cout << "File Not Found" << std::endl;
	}
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