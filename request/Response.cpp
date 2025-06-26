/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/26 16:18:07 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(const int fd, Request &req): _fd(fd), _req(req)
{
	std::string uri;
	std::string host;
	std::string dir_path;

	uri = _req.get_raw_request().substr(0, _req.get_raw_request().find_first_of('\n'));
	uri = uri.substr(uri.find_first_of(' '));
	uri = uri.substr(1, uri.find_last_of(' '));

	host = _req.get_raw_request().substr(_req.get_raw_request().find_first_of('\n'));
	host = host.substr(1);
	host = host.substr(0, host.find_first_of('\n'));
	host = host.substr(host.find_first_of(':'));
	host = host.substr(2);
	host.erase( std::remove(host.begin(), host.end(), '\r'), host.end() );
	host = "http://" + host + "/www"; // Later, will need to manage https as well !

	if (uri.length() < 5)
		uri = "/index.html";
	//std::cout << "HEY HO : " << _req.get_raw_request() << std::endl; // TEMP: print path to console
	//_req.set_uri(_req.get_raw_request().substr(_req.get_raw_request().find_first_of("/"), std::size_t( _req.get_raw_request().find_first_of("/").find_first_of(" ") - _req.get_raw_request().find_first_of("/") )));
	//std::cout << "HEY URL : " << uri << std::endl; // TEMP: print path to console
	//dir_path.append("/../www"); // /../www
	dir_path = "../www" + uri;
	//dir_path.append(uri);
//	dir_path.append(uri.c_str());
	//dir_path.append(uri);
	std::cout << "Path : " << dir_path << std::endl; // TEMP: print path to console
	std::cout << "Len : " << dir_path.length() << std::endl; // TEMP: print path to console
	std::fstream file(dir_path.c_str(), std::ios::in | std::ios::binary);
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