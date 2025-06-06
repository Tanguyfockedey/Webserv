/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:06:16 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/05 20:22:56 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

class Config
{
private:
	// Configuration parameters
	std::string server_name;
	uint16_t* port;
	uint32_t addr;

protected:

public:
	// Constructor and Destructor
	Config() {}
	~Config() {}

	// Getters
	const std::string &get_server_name() const { return server_name; }
	uint16_t* get_port() const { return port; }
	uint32_t get_addr() const { return addr; }

	// Setters
	void set_server_name(const std::string &name) { server_name = name; }
	void set_port(uint16_t* p) { port = p; }
	void set_addr(uint32_t address) { addr = address; }

	// Method to parse configuration (placeholder)
	static Config parse();
};