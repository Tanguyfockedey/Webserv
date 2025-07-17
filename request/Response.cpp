/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/17 16:20:38 by jrichir          ###   ########.fr       */
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
	std::string method;
	std::string uri;
	std::string host;
	std::string dir_path;
	std::string status;
	std::string body;
	std::string rq;
	std::string boundary;
	
	_raw_response.clear();// = "";
	uri.clear();
	
	// DEBUG PRINT RAW REQUEST
	rq = _req.get_raw_request();
	//std::cout << "Raw request: " << std::endl << rq << std::endl;


	// TRY-CATCH pour requetes erronees
	if (rq.empty())
	{
		std::cerr << "Empty request received" << std::endl;
		status = "400 Bad Request";
		_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
		return ;
	}
	if (rq.find("\r\n\r\n") == std::string::npos)
	{
		std::cerr << "Malformed request: missing headers" << std::endl;
		status = "400 Bad Request";
		_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
		return ;
	}
	
	// Parse request line
	method = _req.get_raw_request().substr(0, _req.get_raw_request().find_first_of(' '));
	method.erase(method.find_last_not_of(" \t\r\n") + 1); // Remove trailing whitespace
	method.erase(0, method.find_first_not_of(" \t\r\n")); // Remove leading whitespace

	uri = _req.get_raw_request().substr(0, _req.get_raw_request().find_first_of('\n'));
	uri = uri.substr(uri.find_first_of(' '));
	uri = uri.substr(1, uri.find_last_of(' '));
	uri.erase(uri.find_last_not_of(" \t\r\n") + 1); // Remove trailing whitespace
	uri.erase(0, uri.find_first_not_of(" \t\r\n")); // Remove leading whitespace

	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		std::cerr << "Unsupported HTTP method: " << method << std::endl;
		status = "501 Not Implemented";
		_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
		return ;
	}
	
	if (method == "POST")
	{
		boundary = _req.get_raw_request().substr(_req.get_raw_request().find("boundary=") + 9);
		std::string full_boundary = boundary.substr(0, boundary.find_first_of('\r'));
		full_boundary = "--" + full_boundary;
		// full_boundary.erase(full_boundary.find_last_not_of(" \t\r\n") + 1); // Remove trailing whitespace
		// full_boundary.erase(0, full_boundary.find_first_not_of(" \t\r\n")); // Remove leading whitespace

		// DEBUG PRINT
		std::cout << "Boundary: " << full_boundary << std::endl;

		std::string bodypart = _req.get_raw_request().substr(_req.get_raw_request().find("\r\n\r\n") + 4);
		std::cout << "Body part len: " << bodypart.length() << std::endl;
		std::string bound1 = bodypart.substr(0, bodypart.find(full_boundary) + full_boundary.length() + 2);
		std::string bodyHeaders = bodypart.substr(bodypart.find("Content"));
		bodyHeaders = bodyHeaders.substr(0, bodyHeaders.find("\r\n\r\n") + 4);

		std::cout << "BodyHeaders --->" << bodyHeaders << "<--- ENDOFBodyHeaders" << std::endl;
		std::cout << "Bounds len: " << ((bound1.length() * 2) + 2) << std::endl;
		std::cout << "BodyHeaders len: " << bodyHeaders.length() << std::endl;
		
		




		
		if (boundary.empty())
		{
			std::cerr << "No boundary provided in POST request" << std::endl;
			status = "400 Bad Request";
			_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
			return ;
		}
		std::string filename = _req.get_raw_request().substr(_req.get_raw_request().find("filename=\"") + 10);
		filename = filename.substr(0, filename.find_first_of('"'));
		if (filename.empty())
		{
			std::cerr << "No filename provided in POST request" << std::endl;
			status = "400 Bad Request";
			_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
			return ;
		}
		std::string header_length = _req.get_raw_request().substr(_req.get_raw_request().find("Content-Length"));
		std::string key;
		size_t length;
		std::istringstream iss(header_length);
		iss >> key >> length;
		
		int actualBodyLength = length - ((bound1.length() * 2) + 2) - bodyHeaders.length() - 2; // last 2 is the \r\n before the closing boundary
		std::cout << "Actual body len: " << actualBodyLength << std::endl;


		// DEBUG PRINT
		//std::cout << "length of request: " << length << std::endl;
		
		//DELETE LINE
		//method = _req.get_raw_request().substr(0, _req.get_raw_request().find_first_of(' '));
		
		//body = _req.get_raw_request().substr(_req.get_raw_request().find("\r\n\r\n") + 4);
		body = rq;
		do {
			size_t pos = body.find("\r\n\r\n");
			if (pos == std::string::npos)
			{
				std::cerr << "Malformed request: missing headers" << std::endl;
				status = "400 Bad Request";
				_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
				return ;
			}

			body = body.substr(pos + 4);
		} while (body.find("\r\n\r\n") != std::string::npos);

		// DEBUG PRINT
		std::cout << "FirstPrint Body : " << std::endl << body << std::endl << "---FIN de BODY---" << std::endl;
		//std::string closing_boundary = full_boundary + "--";
		//body = body.substr(body.find(full_boundary) + full_boundary.length());
		

		//body = body.substr(0, body.find("\r\n")); // JUST COMMENTED OUT

		
		// body = body.erase(...) ?
		// body.erase(body.find_last_not_of(" \r\n") + 1); // Remove trailing whitespace
		// body.erase(0, body.find_first_not_of(" \r\n")); // Remove leading whitespace
		// body.erase(body.find(full_boundary), full_boundary.length());
		// body.erase(body.find_last_not_of(" \r\n") + 1); // Remove trailing whitespace
		// body.erase(0, body.find_first_not_of(" \r\n")); // Remove leading whitespace
		//body = rq.substr(rq.find_last_of("\r\n\r\n") + 4);
		if (body.empty())
		{
			std::cerr << "No body in POST request" << std::endl;
			status = "400 Bad Request";
			_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
			return ;
		}

		// DEBUG PRINT
		std::cout << "Body : " << std::endl << body << std::endl << "---FIN de BODY---" << std::endl;

		dir_path = std::string(get_current_dir_name()) + "/www/" + filename;
		std::fstream file(dir_path.c_str(), std::ios::out | std::ios::binary);
		if (file.is_open())
		{
			//file.clear(); // Could cause problems
			for (int i = 0; i < actualBodyLength; ++i)
			{
				file << body[i];
			}
			//file << body;
			file.close();
			status = "201 Created";
			_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
			return ;
		}
		else
		{
			std::cerr << "Failed to open file for writing: " << dir_path << std::endl;
			status = "500 Internal Server Error";
			_raw_response = "HTTP/1.1 " + status + "\r\n\r\n";
			return ;
		}
		return ;
	}

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
	else if (extension == "css")
		mime_type = "text/css";
	else if (extension == "js")
		mime_type = "application/javascript";
	else if (extension == "json")
		mime_type = "application/json";
	else if (extension == "xml")
		mime_type = "application/xml";
	else if (extension == "pdf")
		mime_type = "application/pdf";
	else if (extension == "doc" || extension == "docx")
		mime_type = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	else if (extension == "xls" || extension == "xlsx")
		mime_type = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	else if (extension == "ppt" || extension == "pptx")
		mime_type = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	else if (extension == "csv")
		mime_type = "text/csv";
	else if (extension == "rtf")
		mime_type = "application/rtf";
	else if (extension == "epub")
		mime_type = "application/epub+zip";
	else if (extension == "odt")
		mime_type = "application/vnd.oasis.opendocument.text";
	else if (extension == "odp")
		mime_type = "application/vnd.oasis.opendocument.presentation";
	else if (extension == "ods")
		mime_type = "application/vnd.oasis.opendocument.spreadsheet";
	else if (extension == "jsonp")
		mime_type = "application/javascript"; // JSONP is often served as JavaScript
	else if (extension == "jsonld")
		mime_type = "application/ld+json";
	else if (extension == "wasm")
		mime_type = "application/wasm";
	else if (extension == "woff" || extension == "woff2")
		mime_type = "font/woff";
	else if (extension == "ttf")
		mime_type = "font/ttf";
	else if (extension == "otf")
		mime_type = "font/otf";
	else if (extension == "svgz")
		mime_type = "image/svg+xml"; // Compressed SVG
	else if (extension == "webmanifest")
		mime_type = "application/manifest+json";
	else if (extension == "mpd")
		mime_type = "application/dash+xml";
	else if (extension == "flv")
		mime_type = "video/x-flv";
	else if (extension == "mkv")
		mime_type = "video/x-matroska";
	else if (extension == "mov" || extension == "qt")
		mime_type = "video/quicktime";
	else if (extension == "wmv")
		mime_type = "video/x-ms-wmv";
	else if (extension == "mpegts" || extension == "ts")
		mime_type = "video/mp2t";
	else if (extension == "3gp" || extension == "3gpp")
		mime_type = "video/3gpp";
	else if (extension == "3g2" || extension == "3gp2")
		mime_type = "video/3gpp2";
	else if (extension == "zip")
		mime_type = "application/zip";
	else if (extension == "tar")
		mime_type = "application/x-tar";
	else if (extension == "gz" || mime_type == "gzip")
		mime_type = "application/gzip";
	else if (extension == "bz2")
		mime_type = "application/x-bzip2";
	else if (extension == "7z")
		mime_type = "application/x-7z-compressed";
	else if (extension == "rar")
		mime_type = "application/vnd.rar";
	else if (extension == "exe")
		mime_type = "application/x-msdownload";
	else if (extension == "apk")
		mime_type = "application/vnd.android.package-archive";
	else if (extension == "sh")
		mime_type = "application/x-sh";
	else if (extension == "ps" || extension == "eps")
		mime_type = "application/postscript";
	else if (extension == "txt" || extension == "text" || extension == "conf" || extension == "log" || extension == "md")
		mime_type = "text/plain";
	else if (extension == "mp3")
		mime_type = "audio/mpeg";
	else if (extension == "wav")
		mime_type = "audio/wav";
	else if (extension == "ogg")
		mime_type = "audio/ogg";
	else if (extension == "flac")
		mime_type = "audio/flac";
	else if (extension == "midi" || extension == "mid")
		mime_type = "audio/midi";
	else if (extension == "aac")
		mime_type = "audio/aac";
	else if (extension == "m4a")
		mime_type = "audio/x-m4a";
	else if (extension == "wma")
		mime_type = "audio/x-ms-wma";
	else if (extension == "mp4a")
		mime_type = "audio/mp4";
	else if (extension == "aiff")
		mime_type = "audio/aiff";
	else if (extension == "opus")
		mime_type = "audio/opus";
	else if (extension == "ico")
		mime_type = "image/x-icon";
	else if (extension == "png")
		mime_type = "image/png";
	else if (extension == "jpg" || extension == "jpeg")
		mime_type = "image/jpeg";
	else if (extension == "gif")
		mime_type = "image/gif";
	else if (extension == "webp") // WebP Image
		mime_type = "image/webp";
	else if (extension == "bmp") // Bitmap Image
		mime_type = "image/bmp";
	else if (extension == "tiff" || extension == "tif") // TIFF Image
		mime_type = "image/tiff";
	else if (extension == "avif") // AV1 Image File Format
		mime_type = "image/avif";
	else if (extension == "mpg" || extension == "mpeg") // MPEG Video
		mime_type = "video/mpeg";
	else if (extension == "avi") // AVI: Audio Video Interleave
		mime_type = "video/x-msvideo";
	else if (extension == "mp4") // MPEG-4 Video
		mime_type = "video/mp4";
	else if (extension == "webm") // WebM Video
		mime_type = "video/webm";
	else if (extension == "azw") // Amazon Kindle eBook format
		mime_type = "application/vnd.amazon.ebook";
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
		std::string errorpage;
		if (errno == 2) // No such file or directory (404)
		{
			std::cerr << "errno : " << errno << std::endl;
			std::cerr << "File Not Found: " << dir_path << std::endl;
			errorpage = "error_404.html";
			status = "404 Not Found";
		}
		else if (errno == 13) // Permission denied (403)
		{
			std::cerr << "errno : " << errno << std::endl;
			std::cerr << "Permission Denied: " << dir_path << std::endl;
			errorpage = "error_403.html";
			status = "403 Forbidden";
		}
		else if (errno == 21) // Is a directory (ou tenter 20, si c'est pas 21)
		{
			// If the path is a directory, we can try to open the index.html file in that directory
			dir_path += "/index.html";
			file.open(dir_path.c_str(), std::ios::in | std::ios::binary);
			if (file.fail())
			{
				std::cerr << "errno : " << errno << std::endl;
				std::cerr << "Failed to open index file in directory: " << dir_path << std::endl;
			}
			else
			{
				std::cout << "Opened index file in directory: " << dir_path << std::endl;
			}
		}
		else
		{
			std::cerr << "errno : " << errno << std::endl;
			std::cout << "Unknown error: " << dir_path << std::endl;
		}
		// std::cerr << "errno : " << errno << std::endl;
		// std::cerr << "Failed to open file: " << dir_path << std::endl;
		// std::cerr << "Error: " << strerror(errno) << std::endl;
		std::string err_path = std::string(get_current_dir_name()) + "/pages/" + errorpage;
		// Try to open the error page
		std::fstream file_err(err_path.c_str(), std::ios::in | std::ios::binary);
		if(file_err.is_open())
		{
			std::string body = std::string((std::istreambuf_iterator<char>(file_err)), std::istreambuf_iterator<char>());
			std::string date = get_http_date();
			std::stringstream response;
			response << "HTTP/1.1 " << status << "\r\n";
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
			return ;
		}
		// std::cerr << "Failed to open error page: " << err_path << std::endl;
		// std::cerr << "Error: " << strerror(errno) << std::endl;
		// std::cerr << "File Not Found" << std::endl;
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
		return ;
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
		return ;
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
