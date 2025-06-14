/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:07:02 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/14 20:03:13 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

std::vector<Config> Config::parse_file(std::string file)
{
	std::vector<Config> cluster;
	std::string tmp, str;
	std::ifstream ifs(file.c_str());

	if (!ifs.good())
		throw std::runtime_error("");
	getline(ifs, str, '\0');
	while (str.find("server") != std::string::npos)
	{
		Config config;
		tmp = extract_server_block(str);
		config.extract_name(tmp);
		config.extract_address(tmp);
		config.extract_client_body_size(tmp);
		config.extract_location(tmp);

		cluster.push_back(config);
	}
	ifs.close();
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

void Config::extract_name(std::string& str)
{
	_server_name = extract_token(str, "server_name");
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

void Config::extract_client_body_size(std::string& str)
{
	std::string tmp = extract_token(str, "client_body_size");
	if (!tmp.empty())
		_client_body_size = atoi(tmp.c_str());
	else
		_client_body_size = 1000000;
}

void Config::extract_location(std::string& str)
{
	std::string loc, data;
	while (str.find("location") != std::string::npos)
	{
		loc = str.substr(str.find("location") + 9, str.find('{') - str.find("location") - 9);
		data = str.substr(str.find('{', str.find("location")) + 1, str.find('}',
			str.find("location")) - str.find('{', str.find("location")) - 1);
		loc.erase(remove_if(loc.begin(), loc.end(), isspace), loc.end());
		str.erase(str.find("location"), str.find('}', str.find("location")) - str.find("location") + 1);
		_location[loc] = data;
	}
}
