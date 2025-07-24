/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:35:06 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/23 23:53:24 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "Config.hpp"

class Location
{
	private:
		std::map<std::string, std::string> _tokens;

	public:
		// Constructor and Destructor
		Location(std::string str);
		~Location();

		// Getters
		const std::map<std::string, std::string>& get_tokens() const { return _tokens; }

		// Methods
	private:
		void map_methods(std::string& str);
		void map_tokens(std::string& str);
};
