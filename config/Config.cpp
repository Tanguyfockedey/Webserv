/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:07:02 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/23 23:47:05 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

std::vector<Config> Config::parse_file(std::string file)
{
	std::vector<Config> cluster;
	try
	{
		std::string tmp, str;
		std::ifstream ifs(file.c_str());
	
		if (!ifs.good())
			throw std::runtime_error("");
		getline(ifs, str, '\0');
		while (str.find("server") != std::string::npos)
		{
			try
			{
				Config config;
				tmp = extract_server_block(str);
				config.extract_address(tmp);
				config.extract_location(tmp);
				config.map_methods(tmp);
				config.map_tokens(tmp);
				cluster.push_back(config);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				std::cerr << "Error parsing server block: " << strerror(errno) << std::endl;
				std::cerr << "Skipping invalid server block." << std::endl;
			}
		}
		ifs.close();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cerr << "Error reading configuration file: " << strerror(errno) << std::endl;
		std::cerr << "Please check the file path and format." << std::endl;
	}
	return cluster;
}

std::string Config::extract_server_block(std::string& str)
{
	str.erase(0, str.find("server"));
	str.erase(0, str.find('{') + 1);
	std::string::iterator it = str.begin();
	int parenthesis = 0;

	while (*(it++))
	{
		if ((*it) == '{')
			parenthesis++;
		if ((*it) == '}')
		{
			if (parenthesis)
				parenthesis--;
			else
				break;
		}
	}
	std::string tmp = str.substr(0, it - str.begin());
	str.erase(0, it - str.begin());
	return (tmp);
}

std::string Config::extract_token(std::string& str, const char* token)
{
	std::string tok;
	std::string tmp;
	tok.append(token);
	if (str.find(tok) == std::string::npos)
		return tmp;
	tmp = str.substr(str.find(tok), str.find(";", str.find(tok)) - str.find(tok));
	str.erase(str.find(tok), str.find(";", str.find(tok)) - str.find(tok) + 1);
	tmp.erase(0, tok.length());
	tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
	return tmp;
}

void Config::extract_address(std::string& str)
{
	std::string tmp;

	while (str.find("listen") != std::string::npos)
	{
		tmp = extract_token(str, "listen");
		if (tmp.find(':') == std::string::npos)
			_addr.push_back("0.0.0.0");
		else
			_addr.push_back(tmp.substr(0, tmp.find(':')));
		_port.push_back(atoi(tmp.substr(tmp.find(':') + 1).c_str()));
	}
}

void Config::extract_location(std::string& str)
{
	std::string path, data;
	while (str.find("location") != std::string::npos)
	{
		try
		{
			path = str.substr(str.find("location") + 9, str.find('{') - str.find("location") - 9);
			data = str.substr(str.find('{', str.find("location")) + 1, str.find('}',
				str.find("location")) - str.find('{', str.find("location")) - 1);
			path.erase(remove_if(path.begin(), path.end(), isspace), path.end());
			str.erase(str.find("location"), str.find('}', str.find("location")) - str.find("location") + 1);
			_locations.insert((std::make_pair(path, Location(data))));
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			std::cerr << "Error parsing location block: " << strerror(errno) << std::endl;
			std::cerr << "Skipping invalid location block." << std::endl;
		}
	}
}

void Config::map_tokens(std::string& str)
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

void Config::map_methods(std::string& str)
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
	else
	{
		_tokens["GET"] = "true";
		_tokens["POST"] = "true";
		_tokens["DELETE"] = "true";
	}
}

const std::string Config::get_token(const std::string &path, const char* token)
{
	std::string tok;
	tok.append(token);
	std::map<std::string, Location>::reverse_iterator it = _locations.rbegin();
	std::map<std::string, std::string>::iterator it2;
	while (it != _locations.rend())
	{
		if (path.find(it->first) != std::string::npos)
		{
			std::map<std::string, std::string> tokens = it->second.get_tokens();
			if ((it2 = tokens.find(tok)) != tokens.end())
				return it2->second;
			else
				break;
		}
		++it;
	}
	if ((it2 = _tokens.find(tok)) != _tokens.end())
		return it2->second;
	else
		return "";
}
