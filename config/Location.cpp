/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:35:09 by tafocked          #+#    #+#             */
/*   Updated: 2025/11/19 13:27:07 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(std::string data, std::string path) : _path(path)
{
	map_methods(data);
	map_tokens(data);
}

Location::~Location()
{}

void Location::map_methods(std::string& str)
{
	std::string tmp = Config::extract_token(str, "method");
	if (!(tmp.empty()))
	{
		(tmp.find("GET") != std::string::npos) ?
			_tokens["GET"] = "true": _tokens["GET"] = "false";
		(tmp.find("POST") != std::string::npos) ?
			_tokens["POST"] = "true": _tokens["POST"] = "false";
		(tmp.find("DELETE") != std::string::npos) ?
			_tokens["DELETE"] = "true": _tokens["DELETE"] = "false";
	}
}

void Location::map_tokens(std::string& str)
{
	std::string tmp;
	while (str.find(";") != std::string::npos)
	{
		tmp = str.substr(0, str.find(";"));
		str.erase(0, str.find(";") + 1);
		tmp.erase(0, tmp.find_first_not_of(" \t\n\r"));
		std::string key = tmp.substr(0, tmp.find_first_of(" \t\n\r"));
		tmp.erase(0, tmp.find_first_of(" \t\n\r"));
		tmp.erase(0, tmp.find_first_not_of(" \t\n\r"));
		std::string value = tmp.substr(0, tmp.find(";"));
		value = value.substr(0, value.find_last_not_of(" \t\n\r") + 1);
		if (!key.empty() && !value.empty())
			_tokens[key] = value;
	}
}
