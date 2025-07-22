/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/22 15:48:31 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

static std::string join_paths(const std::string &path_left, const std::string &path_right)
{
	if (path_left.empty())
		return path_right;
	if (path_right.empty())
		return path_left;

	char left_last = path_left[path_left.length() - 1];
	char right_first = path_right[0];

	if (left_last == '/' && right_first == '/')
		return path_left + path_right.substr(1);
	else if (left_last != '/' && right_first != '/')
		return path_left + "/" + path_right;
	else
		return path_left + path_right;
}

static void build_response(Response *response_object, std::fstream &path, const std::string &status_line)
{
	if(path.is_open())
	{
		std::string body = std::string((std::istreambuf_iterator<char>(path)), std::istreambuf_iterator<char>());
		std::stringstream response;

		response << "HTTP/1.1 " << status_line << "\r\n";
		response << "Date: " << response_object->get_http_date() << "\r\n";
		response << "Content-Type:" << response_object->get_req().get_resource_info().find("mime_type")->second << "\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		response << "Connection: keep-alive\r\n";
		response << "Cache-Control: no-store\r\n";
		response << "\r\n";
		response << body;
		response_object->set_response(response.str());
	}
	else
	{
		std::cerr << "Failed to open file for reading." << std::endl;
		response_object->set_response("HTTP/1.1 500 Internal Server Error\r\n\r\n");
	}
	path.close();
}


static void process_get_request(Response *response_object, Request &req)
{
	std::string path, status_line, error_msg, error_page;
	
	path = join_paths(get_current_dir_name(), req.get_uri());
	std::fstream file(path.c_str(), std::ios::in | std::ios::binary);

	if (file.fail())
	{
		if (errno == 2) // No such file or directory (404)
		{
			error_msg = "File Not Found: " + path + "\n";
			error_page = "error_404.html";
			status_line = "404 Not Found";
		}
		else if (errno == 13) // Permission denied (403)
		{
			error_msg = "Permission Denied: " + path + "\n";
			error_page = "error_403.html";
			status_line = "403 Forbidden";
		}
		else if (errno == 21) // Is a directory (ou tenter 20, si c'est pas 21)
		{
			// Handle directory access / listing (peut-etre pas ici, ici on est dans le cas FAIL)
		}
		else
		{
			error_msg = "Unknown error: " + path + "\n";
		}
		
		std::cerr << "errno : " << errno << std::endl;
		std::cerr << error_msg;
		response_object->set_status_line(status_line);
		
		std::string err_path;
		err_path = join_paths(get_current_dir_name(), "/pages/");
		err_path = join_paths(err_path, error_page);
		std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
		build_response(response_object, file_err, status_line);
	}
	else
	{
		build_response(response_object, file, "200 OK");
	}
}


void Response::handle_multipart_post(std::string &boundary)
{
	//DEBUG print
	std::cout << "Multipart POST request handled." << std::endl;//DEBUG
	size_t actual_body_length = this->get_req().get_actual_body_length();


	//DEBUG
	std::cout << "Actual body length: " << actual_body_length << std::endl;//DEBUG
	(void)boundary;//DEBUG
}

void Response::handle_single_part_post()
{
	//DEBUG print
	std::cout << "Single part POST request handled." << std::endl;//DEBUG
	std::string body = _req.get_body();
	if (body.empty())
		return ;
	else
		for (size_t i = 0; i < body.length(); ++i)
			std::cout << body[i];
}

// zero size
// single part vs multipart
static void process_post_request(Response *response_object, Request &req)
{
	if (req.get_boundary().empty())
	{
		response_object->handle_single_part_post();
		return ;
	}
	std::string boundary, status_line, response;

	//DEBUG
	std::cout << "Boundary: " << std::endl << boundary << std::endl;//DEBUG

	if (boundary.length() > 70)
	{
		std::cerr << "Boundary too long: " << boundary.length() << " characters" << std::endl;
		status_line = "400 Bad Request";
		response = "HTTP/1.1 " + status_line + "\r\n\r\n";
		response_object->set_response(response);
		return ;
	}
	else if (boundary.length() > 0)
	{
		//DEBUG
		std::cout << "Boundary found: " << boundary << std::endl;//DEBUG
		
		response_object->handle_multipart_post(boundary);
	}

	//DEBUG
	return ;//DEBUG


	int req_headers_length = req.get_headers_string().length();

	std::string response_body = req.get_raw_request().substr(req.get_raw_request().find("\r\n\r\n") + 4);

	std::string start_boundary = response_body.substr(0, response_body.find("--" + boundary) + boundary.length() + 4); // +4 for the "--" before, and the "\r\n" after
	std::string body_part_headers = response_body.substr(response_body.find("Content"));
	body_part_headers = body_part_headers.substr(0, body_part_headers.find("\r\n\r\n") + 4);

	if (boundary.empty())
	{
		std::cerr << "No boundary provided in POST request" << std::endl;
		status_line = "400 Bad Request";
		response = "HTTP/1.1 " + status_line + "\r\n\r\n";
		return ;
	}
	std::string filename = req.get_raw_request().substr(req.get_raw_request().find("filename=\"") + 10);
	filename = filename.substr(0, filename.find_first_of('"'));
	if (filename.empty())
	{
		std::cerr << "No filename provided in POST request" << std::endl;
		status_line = "400 Bad Request";
		response = "HTTP/1.1 " + status_line + "\r\n\r\n";
		return ;
	}

	size_t total_content_length;

	std::istringstream iss(req.get_headers().find("Content-Length")->second);
	iss >> total_content_length;
	
	int actual_body_length = total_content_length - ((start_boundary.length() * 2) + 2) - body_part_headers.length() - 2; // last 2 is the \r\n before the closing boundary

	int limit;
	if (actual_body_length < (BUFFER_SIZE - req_headers_length - (int)body_part_headers.length() - (((int)start_boundary.length() * 2) + 2) - 2))
		limit = actual_body_length;
	else
		limit = BUFFER_SIZE;
	limit = limit - req_headers_length - (int)body_part_headers.length() - (((int)start_boundary.length() * 2) + 2) - 2;
	
	std::string actual_body = req.get_body().substr(req.get_body().find("\r\n\r\n") + 4);
	std::string path;
	path = join_paths(get_current_dir_name(), req.get_config().get_root());
	path = join_paths(path, UPLOAD_PATH);
	path = join_paths(path, filename);
	std::fstream file(path.c_str(), std::ios::out | std::ios::binary);
	if (file.is_open())
	{
		for (int i = 0; i < limit; ++i)
		{
			file << actual_body[i];
			if (i == actual_body_length - 1)
				break;
		}
		file.close();
		status_line = "201 Created";
	}
	else
	{
		std::cerr << "Failed to open file for writing: " << path << std::endl;
		status_line = "500 Internal Server Error";
	}
	response_object->set_response("HTTP/1.1 " + status_line + "\r\n\r\n");
	return ;
}


static void process_delete_request(Response *response_object, Request &req)
{
	// Basic implementation for now
    int status = remove(req.get_uri().c_str());

    if (status != 0) {
        perror("Error deleting file");
    }
    else {
        std::cout << "File successfully deleted : " << req.get_uri() << std::endl;
    }
	
	response_object->set_response("...");
}


Response::Response(const int fd, Request &req): _fd(fd), _req(req)
{
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
		process_get_request(this, _req);
	else if (_req.get_method() == "POST")
		process_post_request(this, _req);
	else if (_req.get_method() == "DELETE")
		process_delete_request(this, _req);
}

Response::~Response()
{}

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
