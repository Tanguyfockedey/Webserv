/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:06:16 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/19 19:17:58 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "Location.hpp"

class Location;

class Config
{
private:
	// Configuration parameters
	std::string _server_name;
	std::vector<std::string> _addr;
	std::vector<uint16_t> _port;
	std::string _index;
	std::string _root;
	int _client_body_size;
	std::map<std::string, Location> _locations;

public:
	// Constructor and Destructor
	Config() {}
	~Config() {}

	// Getters
	const std::string& get_server_name() const {return _server_name;}
	const std::vector<std::string>& get_addr() const {return _addr;}
	const std::vector<uint16_t>& get_port() const {return _port;}
	const int& get_client_body_size() const {return _client_body_size;}
	const std::string& get_index() const {return _index;}
	const std::string& get_root() const {return _root;}
	const std::map<std::string, Location>& get_locations() const {return _locations;}

	// Method to parse configuration
	static std::vector<Config> parse_file(std::string file);
	static std::string extract_token(std::string& str, const char* token);

private:
	static std::string extract_server_block(std::string& str);
	void extract_name(std::string& str);
	void extract_address(std::string& str);
	void extract_client_body_size(std::string& str);
	void extract_index(std::string& str);
	void extract_root(std::string& str);
	void extract_location(std::string& str);
};
