/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:35:06 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/09 11:12:15 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "Config.hpp"

class Location
{
private:
	std::string _root;
	std::string _index;
	std::vector<std::string> _allow_methods;

public:
	// Constructor and Destructor
	Location(const std::string& raw);
	~Location();

	// Getters
	const std::string& get_root() const { return _root; }
	const std::string& get_index() const { return _index; }
	const std::vector<std::string>& get_allow_methods() const { return _allow_methods; }

};
