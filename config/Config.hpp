/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:06:16 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/14 20:00:47 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

class Config
{
private:
	// Configuration parameters
	std::string _server_name;
	std::vector<std::string> _addr;
	std::vector<uint16_t> _port;
	std::map<std::string, std::string> _location;
	int _client_body_size;

protected:

public:
	// Constructor and Destructor
	Config() {}
	~Config() {}

	// Getters
	const std::string& get_server_name() const {return _server_name;}
	const std::vector<std::string>& get_addr() const {return _addr;}
	const std::vector<uint16_t>& get_port() const {return _port;}
	const int& get_client_body_size() const {return _client_body_size;}

	// Method to parse configuration
	static std::vector<Config> parse_file(std::string file);

private:
	static std::string extract_server_block(std::string& str);
	static std::string extract_token(std::string& str, const char* token);
	void extract_name(std::string& str);
	void extract_address(std::string& str);
	void extract_client_body_size(std::string& str);
	void extract_location(std::string& str);
};
