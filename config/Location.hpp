/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:35:06 by tafocked          #+#    #+#             */
/*   Updated: 2025/11/19 13:27:22 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "Config.hpp"

class Location
{
	private:
		std::string _path;
		std::map<std::string, std::string> _tokens;

	public:
		// Constructor and Destructor
		Location(std::string data, std::string path);
		~Location();

		// Getters
		const std::map<std::string, std::string>& get_tokens() const { return _tokens; }
		const std::string& get_path() const { return _path; }
		
		// Methods
	private:
		void map_methods(std::string& str);
		void map_tokens(std::string& str);
};
