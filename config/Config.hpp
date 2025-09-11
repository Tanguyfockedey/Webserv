/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcygan <mcygan@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:06:16 by tafocked          #+#    #+#             */
/*   Updated: 2025/09/11 05:07:21 by mcygan           ###   ########.fr       */
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
		std::map<std::string, Location> _locations;
		std::map<std::string, std::string> _tokens;
		std::vector<std::string> _addr;
		std::vector<uint16_t> _port;

	public:
		// Constructor and Destructor
		Config() {}
		~Config() {}

		// Getters
		const std::map<std::string, Location>& get_locations() const {return _locations;}
		const std::vector<std::string>& get_addr() const {return _addr;}
		const std::vector<uint16_t>& get_port() const {return _port;}
		const std::string get_token(const std::string &path, const char* token);

		// Method to parse configuration
		static std::vector<Config> parse_file(std::string file);
		static std::string extract_token(std::string& str, const char* token);

		// Other methods
		bool is_allowed(std::string path, std::string method);

	private:
		static std::string extract_server_block(std::string& str);
		void extract_address(std::string& str);
		void extract_location(std::string& str);
		void map_methods(std::string& str);
		void map_tokens(std::string& str);
};
