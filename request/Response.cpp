/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/20 16:40:33 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(const int fd): _fd(fd)
{
	_raw_response = "HTTP/1.1 200\r\n";
	_raw_response.append("content-type: text/html\r\n");
	_raw_response.append("content-length: 100\r\n\r\n");
	std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("/"));
	dir_path.append("/../www/index.html");
	std::fstream file(dir_path.c_str(), std::ios::in | std::ios::binary);
	std::cout << dir_path << std::endl;
	if(file.is_open()){
		_raw_response.append(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
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