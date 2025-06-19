/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:35:09 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/19 19:35:56 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(const std::string& raw)
{
	std::string str = raw;
	_root = Config::extract_token(str, "root");
	_index = Config::extract_token(str, "index");
	std::string tmp = Config::extract_token(str, "method");
	if (tmp.find("GET") != std::string::npos)
		_allow_methods.push_back("GET");
	if (tmp.find("POST") != std::string::npos)
		_allow_methods.push_back("POST");
	if (tmp.find("DELETE") != std::string::npos)
		_allow_methods.push_back("DELETE");
}

Location::~Location()
{}
