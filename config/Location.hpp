/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcygan <mcygan@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:35:06 by tafocked          #+#    #+#             */
/*   Updated: 2025/09/11 05:21:40 by mcygan           ###   ########.fr       */
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
		bool is_allowed(std::string method);
	private:
		void map_methods(std::string& str);
		void map_tokens(std::string& str);
};
