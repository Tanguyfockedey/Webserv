/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/30 15:43:10 by jrichir          ###   ########.fr       */
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
	uri = uri.substr(1); // strip initial slash

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
		uri = "index.html";
	}
	std::cout << "uri : " << uri << std::endl;
	std::string fullpath = "http://localhost/home/jrichir/git/jgithub/rank5/webserv/www/";
	if (uri == "favicon.ico")
	{
		fullpath = "http://localhost/home/jrichir/git/jgithub/rank5/webserv/";
	}
	dir_path = "./www/" + uri;
	std::fstream file(dir_path.c_str(), std::ios::in | std::ios::binary);
	if(file.is_open())
	{
		std::cout << "PAF!" << std::endl;
		std::cout << "Path : " << dir_path << std::endl; // TEMP: print path to console
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		// Add a Date header
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file.close();
		uri.clear();
		body.clear();
		response.clear();
		return;
	}
	std::cout << "Failed path : " << dir_path << std::endl; // TEMP: print path to console
	dir_path = fullpath + uri;
	std::fstream file0(dir_path.c_str(), std::ios::in | std::ios::binary);
	if(file0.is_open())
	{
		std::cout << "ABSOL PATH" << std::endl;
		std::cout << "Path : " << dir_path << std::endl; // TEMP: print path to console
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file0.close();
		uri.clear();
		body.clear();
		response.clear();
		return;
	}
	std::cout << "Failed path : " << dir_path << std::endl; // TEMP: print path to console
	dir_path = "/" + uri;
	std::fstream file1(dir_path.c_str(), std::ios::in | std::ios::binary);
	if(file1.is_open())
	{
		std::cout << "ERROR 404 page" << std::endl;
		std::cout << "Path : " << dir_path << std::endl; // TEMP: print path to console
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file1.close();
		uri.clear();
		body.clear();
		response.clear();
		return;
	}
	std::cout << "Failed path : " << dir_path << std::endl; // TEMP: print path to console
	dir_path = "./" + uri;
	std::fstream file2(dir_path.c_str(), std::ios::in | std::ios::binary);
	if(file2.is_open())
	{
		std::cout << "BOUM" << std::endl;
		std::cout << "Path : " << dir_path << std::endl; // TEMP: print path to console
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file2.close();
		uri.clear();
		body.clear();
		response.clear();
		return;
	}
	std::cout << "Failed path : " << dir_path << std::endl; // TEMP: print path to console
	dir_path = uri;
	std::fstream file3(dir_path.c_str(), std::ios::in | std::ios::binary);
	if(file3.is_open())
	{
		std::cout << "TCHAK" << std::endl;
		std::cout << "Path : " << dir_path << std::endl; // TEMP: print path to console
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file3.close();
		uri.clear();
		body.clear();
		response.clear();
		return;
	}
	std::cout << "Failed path : " << dir_path << std::endl; // TEMP: print path to console
	dir_path = "../pages/error404.html";
	std::fstream file4(dir_path.c_str(), std::ios::in | std::ios::binary);
	if(file4.is_open())
	{
		std::cout << "ERROR 404 page" << std::endl;
		std::cout << "Path : " << dir_path << std::endl; // TEMP: print path to console
		std::string body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "\r\n";
		response << body;
		_raw_response = response.str();
		file4.close();
		uri.clear();
		body.clear();
		response.clear();
		return;
	}
	std::cout << "Failed path : " << dir_path << std::endl; // TEMP: print path to console
	std::cout << "File Not Found" << std::endl;
	// else {
	// 	std::cout << "File Not Found" << std::endl;
	// }
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