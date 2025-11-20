/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/11/20 06:43:44 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../cgi/CgiHandler.hpp"

Response::Response(const int fd, Request &req): _fd(fd), _req(req)
{
	int	request_error_code = _req.get_error_code();
	_config = req.get_config();

	std::string error_name;

	if (request_error_code != 0)
	{
		switch(request_error_code)
		{
			case 400:
				error_name = "Bad Request";
				break;
			case 405:
			{
				handle_405();
				return ;
			}
			case 413:
				error_name = "Payload Too Large";
				break;
			case 414:
				error_name = "URI Too Long";
				break;
			case 501:
				error_name = "Not Implemented";
				break;
			case 505:
				error_name = "HTTP Version Not Supported";
				break;
			default:
				error_name = "Error";
				break;
		}
		std::string error_msg;
		std::stringstream ss(error_msg);
		ss << "HTTP/1.1 " << _req.get_error_code() << " " << error_name << "\r\n";
		ss << "Host: " << _config.get_token("", "server_name") << "\r\n";
		ss << "Date: " << get_http_date() << "\r\n";
		_headers_string = ss.str();
		ss << "\r\n";
		_response = ss.str();
		return ;
	}

	_file_error = false;
	if (_req.get_method() == "GET")
	{
		process_get_request();
	}
	else if (_req.get_method() == "POST")
		process_post_request();
	else if (_req.get_method() == "DELETE")
		process_delete_request();
}

Response::~Response()
{}

void Response::print_dir_listing(std::string &dir_path)
{
	std::vector<std::string> files;
	
	std::string body = "<html><head>"\
	"<link rel=\"stylesheet\" type=\"text/css\" href=\"/styles.css\">" \
	"<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\">" \
	"<meta charset=\"UTF-8\"><title>Index of " + _req.get_raw_uri() + "</title></head><body>";
	if (get_dir_content(dir_path, files) != -1)
	{
		_status_line = "200 OK";
		body += "<h1>Index of " + _req.get_raw_uri() + "</h1>\n";
		for (size_t i = 0; i < files.size(); i++)
		{
			if (files[i] == ".")
				continue ;
			else if (files[i] == "..")
			{
				body += "<p><a href=\"..\">⬑ Parent Directory</a></p>\n";
				continue ;
			}
			body += "<p><a href=\"" + files[i] + "\">" + files[i] + "</a></p>\n";
		}
	}
	else
	{
		std::cerr << "Failed to open directory: " << dir_path << std::endl;
		set_error_page("500", "Internal Server Error", "");
		return;
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
	
	dir_path = _req.get_computed_path();

	if (dir_path[dir_path.length() - 1] != '/')
	{
		redirect(_req.get_raw_uri() + "/");
		return;
	}
	
	std::string file_type = get_file_type(dir_path);
	
	if (file_type == "nonexistent")
	{
		error_msg = "Directory does not exist: " + dir_path + "\n";
		std::cerr << error_msg;
		set_error_page("404", "Not Found", "");
		return ;
	}

	std::string server_path_part = server_path();
	std::string config_root_part = _config.get_token(_req.get_uri(), "root");
	std::string raw_uri_part = _req.get_raw_uri();

	// check if index or location-specific index
	index = _config.get_token(_req.get_uri(), "index");
	if (!index.empty())
	{
		index_path = dir_path;
		//index_path = join_paths(dir_path, index);
		if (get_file_type(index_path) == "nonexistent")
		{
			
			// check for autoindex option
			std::string option = _config.get_token(_req.get_uri(), "autoindex");
			
			if (_config.get_token(_req.get_uri(), "autoindex") == "on")
			{
				print_dir_listing(dir_path);
				return ;
			}
			else
			{
				error_msg = "Index file does not exist: " + index_path + "\n";
				std::cerr << error_msg;
				set_error_page("403", "Forbidden", "");
				return ;
			}
		}
		else
		{
			std::fstream file(index_path.c_str(), std::ios::in | std::ios::binary);
			if (file.fail())
			{
				error_msg = "Failed to open index file: " + index_path + "\n";
				std::cerr << error_msg;
				set_error_page("403", "Forbidden", "");
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
			print_dir_listing(dir_path);
			_status_line = "200 OK";
		}
		else
		{
			// autoindex off, error 403
			error_msg = "Autoindex is off for this directory: " + _req.get_uri() + "\n";
			std::cerr << error_msg;
			set_error_page("403", "Forbidden", "");
		}
	}
}

void Response::get_file(int error = 0)
{
	std::string index, path, error_msg, error_page;
	
	path = _req.get_computed_path();
	
	std::fstream file(path.c_str(), std::ios::in | std::ios::binary);
	_file_error = false;
	if (file.fail() || error)
	{
		_file_error = true;
		if (error)
			errno = error;
		if (errno == ENOENT) // No such file or directory (404)
		{
			set_error_page("404", "Not Found", "");
			return;
		}
		else if (errno == EACCES) // Permission denied (403)
		{
			set_error_page("403", "Forbidden", "");
			return;
		}
		else if (errno == EISDIR) // Is a directory (ou tenter ENOTDIR, si c'est pas EISDIR)
		{
			error_msg =  "Failed opening a directory";
			set_error_page("500", "Internal Server Error", "");
		}
		else
		{
			error_msg = "Unknown error: " + path + "\n";
		}
		
		std::cerr << "errno : " << errno << std::endl;
		std::cerr << error_msg;
		
		std::string err_path;
		err_path = join_paths(server_path(), "/data/error_pages/");
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

void Response::process_get_request()
{
	std::string allowed_methods = _config.get_token(_req.get_raw_uri(), "method");
	
	if (!is_allowed_method("GET"))
	{
		handle_405();
		return ;
	}
	
	std::string computed_path = _req.get_computed_path();
	
	bool is_dir = _req.get_uri_is_directory();
	bool is_file = _req.get_uri_is_regular_file();
	if (is_dir)
	{
		get_dir();
	}
	else if (is_file)
	{
		std::string	uri = _req.get_uri();
		if (!uri.rfind("/cgi-bin/", 0) && uri.substr(uri.length() - 3) == ".py")
			this->handle_cgi();
		else
			get_file();
	}
	else
	{
		set_error_page("404", "Not Found", "");
		return;
	}
}

void Response::process_post_request()
{
	std::string path, error_msg, error_page;
	
	if (!is_allowed_method("POST"))
	{
		handle_405();
		return ;
	}
	std::string	uri = _req.get_uri();
	if (!uri.rfind("./cgi-bin/", 0) && uri.substr(uri.length() - 3) == ".py")
		return this->handle_cgi();
	if (_req.get_boundary().empty())
	{
		handle_single_part_post();
		return ;
	}

	std::string boundary;
	
	boundary = _req.get_boundary();
	std::cerr << "boundary: " << boundary << std::endl;

	if (boundary.length() > 70)
	{
		std::cerr << "Boundary too long: " << boundary.length() << " characters" << std::endl;
		_status_line = "400 Bad Request";
		_response = "HTTP/1.1 " + _status_line + "\r\n\r\n";
		return ;
	}
	else if (boundary.length() > 0)
	{
		handle_multipart_post();
	}
}

std::string	Response::delete_success_response(std::string status_line, std::string body = "")
{
	std::stringstream	response;

	response << "HTTP/1.1 " << status_line << "\r\n";
	response << "Host: " << _config.get_token("", "server_name") << "\r\n";
	response << "Date: " << get_http_date() << "\r\n";
	response << "Content-Type: " << "text/html" << "\r\n";
	response << "Content-Length: " << body.length() << "\r\n\r\n";
	if (body != "")
		response << "<html><body><center>" << body << " has been deleted</center></body></html>\r\n";

	return response.str();
}

void Response::process_delete_request()
{	
	if (!is_allowed_method("DELETE"))
		return handle_405();
		
	std::string path = join_paths(server_path(), _req.get_uri());
	if (access(path.c_str(), W_OK))
	{
		if (errno == EACCES)
			return set_error_page("403", "Forbidden", "");
		else if (errno == ENOENT)
			return set_error_page("404", "Not found", "");
	}
	if (_req.get_uri_is_directory())
	{
		if (rmdir(path.c_str()))
			return set_error_page("500", "Internal server error", "");
		else 
			_response = delete_success_response("200 OK", path);
	}
	else
	{
		if (remove(path.c_str()))
			return set_error_page("500", "Internal server error", "");
		else
			_response = delete_success_response("200 OK", path);
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
	{
		content_type = _req.get_resource_info().find("mime_type")->second;
	}
	if (path.is_open())
	{
		_body = std::string((std::istreambuf_iterator<char>(path)), std::istreambuf_iterator<char>());
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
		path.close();
	}
	else
	{
		std::cerr << "Failed to open file for reading." << std::endl;
		set_error_page("500", "Internal Server Error", "");
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
	std::string file = _req.get_raw_request().substr(_req.get_raw_request().find("POST ") + 5);
	file = file.substr(0, file.find_first_of(' '));
	std::string filename;
	
	// Extract last part of path (file) as filename
	if (file.find_last_of('/') != std::string::npos)
		filename = file.substr(file.find_last_of('/') + 1);
	else
		filename = file;
	
	std::string length_string = _req.get_raw_request().substr(_req.get_raw_request().find("ength: ") + 7);
	length_string = length_string.substr(0, length_string.find_first_of("\r\n"));
	std::stringstream length_stream;
	length_stream << length_string;
	size_t content_length;
	length_stream >> content_length;
	std::string new_file_path = join_paths(server_path(), _config.get_token("/", "root"));
	new_file_path = join_paths(new_file_path, UPLOAD_PATH);
	new_file_path = join_paths(new_file_path, filename);

	if (filename.empty()) // si l'URL est un directory
	{
		handle_405();
		return ;
	}

	if (content_length < 1)
	{
		content_length = _req.get_body().length();
		if (content_length < 1)
		{
			std::cerr << "No body in POST request" << std::endl;
			_status_line = "400 Bad Request";
			_response = "HTTP/1.1 " + _status_line + "\r\n\r\n";
			return ;
		}
	}
	
	std::fstream filestream(new_file_path.c_str(), std::ios::out | std::ios::binary);
	if (filestream.is_open())
	{
		for (size_t i = 0; i < content_length; ++i)
		{
			filestream << _req.get_body()[i];
		}
		filestream.close();
		_status_line = "201 Created";
		std::stringstream response;
		response << "HTTP/1.1 " << _status_line << "\r\n";
		response << "Host: " << _config.get_token("", "server_name") << "\r\n";
		response << "Date: " << get_http_date() << "\r\n";
		response << "Content-Length: 0\r\n";
		_headers_string = response.str();
		response << "\r\n";
		_response = response.str();
	}
	else
	{
		std::cerr << "Failed to open file for writing: " << new_file_path << std::endl;
		_status_line = "500 Internal Server Error";
		_response = "HTTP/1.1 " + _status_line + "\r\n\r\n";
		return ;
	}

	if (file.empty() && _body.empty())
	{
		std::cerr << "File seems to be empty and the request has no body data" << std::endl;
		_status_line = "400 Bad Request";
		_response = "HTTP/1.1 " + _status_line + "\r\n\r\n";
		return ;
	}
}

int Response::get_dir_content(std::string dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
		return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

void Response::handle_multipart_post()
{
	std::string post_data, filename, path;

	post_data = _req.get_multipart_data();

	filename = _req.get_raw_request().substr(_req.get_raw_request().find("filename=\"") + 10);
	filename = filename.substr(0, filename.find_first_of('"'));
	if (filename.empty())
	{
		std::cerr << "No filename provided in POST request" << std::endl;
		set_error_page("400", "Bad Request", "");
		return ;
	}
	path = join_paths(server_path(), _config.get_token(_req.get_uri(), "root"));
	path = join_paths(path, UPLOAD_PATH);
	
	mkdir(path.c_str(), 0755);
    DIR *dp;
    if((dp  = opendir(path.c_str())) == NULL) {
		set_error_page("403", "Forbidden", "");
		return ;
	}
	else
	{
		closedir(dp);
	}

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
		set_error_page("500", "Internal Server Error", "");
		return ;
	}
	std::string html_file_link = UPLOAD_PATH + filename;

	_body = "<!DOCTYPE html>" \
		"<html lang=\"en\">" \
			"<head>" \
				"<link rel=\"stylesheet\" type=\"text/css\" href=\"/styles.css\">" \
				"<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\">" \
				"<title>"+ filename + " uploaded - WS Homepage</title>" \
			"</head>" \
			"<body>" \
				"<h1 style=\"text-align: center;\">" \
					"FILE UPLOADED" \
				"</h1>" \
				"<p style=\"text-align: center; margin: 42px;\">" \
					"<a style=\"color: #7F7; font-weight: bold; font-size: 3em; margin: 42px;\" href=\"" + html_file_link + "\">Link to the file you've just uploaded</a>" \
				"</p>" \
				"<p style=\"text-align: center;\">" \
					"<a href=\"/page2.html\">Upload another file</a><br><br>" \
					"<a href=\"/\">Go back to index page</a>" \
				"</p>" \
			"</body>" \
		"</html>" \
		;

	std::stringstream response;
	response << "HTTP/1.1 " << _status_line << "\r\n";
	response << "Host: " << _config.get_token("", "server_name") << "\r\n";
	response << "Date: " << get_http_date() << "\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << _body.length() << "\r\n";
	_headers_string = response.str();
	response << "\r\n";
	response << _body;
	_response = response.str();
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

void Response::redirect(std::string path)
{
	std::stringstream response;
	response << "HTTP/1.1 301 Moved Permanently\r\n";
	response << "Host: " << _config.get_token("", "server_name") << "\r\n";
	response << "Date: " << get_http_date() << "\r\n";
	response << "Location: " << path << "\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: 0\r\n";
	_headers_string = response.str();
	response << "\r\n";
	response << _body;
	_response = response.str();
}

void Response::set_error_page(std::string nb, std::string name, std::string header)
{
	_status_line = nb + " " + name;
	std::string err_page = "err_page_";
	err_page += nb;
	std::string err_path = _config.get_token(_req.get_uri(), err_page.c_str());
	std::string path = _req.get_computed_path().substr(0, _req.get_computed_path().find_last_of('/') + 1);
	path = join_paths(path, err_path);

	std::fstream file_err(path.c_str(), std::ios::in | std::ios::binary);
	if (!err_path.empty() && !file_err.fail() && file_err.is_open())
	{
		_body = std::string((std::istreambuf_iterator<char>(file_err)), std::istreambuf_iterator<char>());
		std::stringstream response;
		response << "HTTP/1.1 " << _status_line << "\r\n";
		response << "Host: " << _config.get_token("", "server_name") << "\r\n";
		response << "Date: " << get_http_date() << "\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << _body.length() << "\r\n";
		_headers_string = response.str();
		response << "\r\n";
		response << _body;
		_response = response.str();
		file_err.close();
	}
	else
	{
		_body = "<!DOCTYPE html>" \
			"<html lang=\"en\">" \
				"<head>" \
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"/styles.css\">" \
					"<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\">" \
					"<title>" + nb + " - WS Homepage</title>" \
				"</head>" \
				"<body style=\"background-color: grey;\">" \
					"<h1 style=\"text-align: center;\">" \
						"<span style=\"text-decoration: underline;\">" \
							"<strong>GENERIC ERROR " + nb +"</strong>" \
						"</span>" \
					"</h1>" \
					"<p style=\"text-align: center;\">" \
						+ name + \
					"</p>" \
					"<p style=\"text-align: center;\">" \
						"<a href=\"/\">Go back to index page</a>" \
					"</p>" \
				"</body>" \
			"</html>" \
			;

		std::stringstream response;
		response << "HTTP/1.1 " << _status_line << "\r\n";
		response << "Host: " << _config.get_token("", "server_name") << "\r\n";
		response << "Date: " << get_http_date() << "\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << _body.length() << "\r\n";
		if (!header.empty())
			response << header << "\r\n";
		_headers_string = response.str();
		response << "\r\n";
		response << _body;
		_response = response.str();
	}
}

void Response::handle_405()
{
	std::string allowed;
	bool	sep = false;

	if (_config.get_token(_req.get_uri(), "GET") == "true")
	{
		allowed.append("GET");
		sep = true;
	}
	if (_config.get_token(_req.get_uri(), "POST") == "true")
	{
		if (sep)
			allowed.append(", ");
		allowed.append("POST");
		sep = true;
	}
	if (_config.get_token(_req.get_uri(), "DELETE") == "true")
	{
		if (sep)
			allowed.append(", ");
		allowed.append("DELETE");
	}

	set_error_page("405", "Method Not Allowed", "Allow: " + allowed);
}

bool Response::is_allowed_method(const std::string& method) {
	std::string allowed_method = _config.get_token(_req.get_uri(), method.c_str());

	if (allowed_method == "false")
		return false;
	return true;
}

void Response::handle_cgi()
{
	CgiHandler			cgi(_req);
	std::string			path;
	std::string			body;
	std::stringstream	response;
	
	path = _req.get_computed_path();
	if (access(path.c_str(), R_OK) || access(path.c_str(), X_OK))
		return set_error_page("403", "Forbidden", "");
	body = cgi.executeCgi(path);
	if (body == "500 Internal server error\r\n\r\n")
		_response = body;
	else
	{
		response << "HTTP/1.1 200 OK\r\n";
		response << "Host: " << _config.get_token("", "server_name") << "\r\n";
		response << "Date: " << get_http_date() << "\r\n";
		response << "Content-Type: " << "text/html" << "\r\n";
		response << "Content-Length: " << body.length() << "\r\n";
		_headers_string = response.str();
		response << "\r\n";
		response << _body;
		_response = response.str();
	}
}
