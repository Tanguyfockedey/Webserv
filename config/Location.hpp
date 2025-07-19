/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:35:06 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/09 10:29:45 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"
#include "Config.hpp"

class Location
{
private:
	std::string _index;
	std::string _root;
	int _client_body_size;
	std::string _allow_methods;
	std::string _autoindex;

public:
	// Constructor and Destructor
	Location(std::string str);
	~Location();

	// Getters
	const std::string& get_root() const { return _root; }
	const std::string& get_index() const { return _index; }
	const int& get_client_body_size() const { return _client_body_size; }
	const std::string& get_allow_methods() const { return _allow_methods; }
};
