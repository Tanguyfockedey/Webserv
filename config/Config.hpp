/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:06:16 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/10 19:33:17 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

class Config
{
private:
	// Configuration parameters
	std::string _server_name;
	uint32_t _addr;
	uint16_t* _port;
	int _client_body_size;

protected:

public:
	// Constructor and Destructor
	Config() {}
	~Config() {}

	// Getters
	std::string get_server_name() const {return _server_name;}
	uint32_t get_addr() const {return _addr;}
	uint16_t* get_port() const {return _port;}

	// Method to parse configuration (placeholder)
	static Config parse();
	static Config* parse_file(std::string file);

private:
	static std::string extract_server_block(std::string & str);
};
