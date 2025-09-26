/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:21 by tafocked          #+#    #+#             */
/*   Updated: 2025/09/26 10:45:57 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "Request.hpp"

class Response
{
	private :
		int _fd;
		Request	_req;
		std::string _response;
		std::string _status_line;
		std::string _headers_string;
		bool _file_error;
		std::map<std::string, std::string> _headers;
		std::string _body;
		Config _config;


	public :
		// Constructor and Destructor
		Response(const int fd, Request &req);
		~Response();

		// Getters
		int get_fd() const { return _fd; }
		const std::string& get_response() const { return _response; }
		const std::string& get_headers_string() const { return _headers_string; }

		// Setters
		void set_response(const std::string& response) { _response = response; }

		// Public methods
		void process_get_request();
		void process_post_request();
		void process_delete_request();
		void build_response(std::fstream &path);
		void handle_single_part_post();
		void handle_multipart_post();
		int get_dir_content(std::string dir, std::vector<std::string> &files);
		void print_dir_listing(std::string &dir_path);
		void get_dir();
		void get_file();
		const std::string get_http_date();
		void handle_405();
		void set_error_page(std::string nb, std::string name, std::string header);
		bool is_allowed_method(const std::string& method);
};
