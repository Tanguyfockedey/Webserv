/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:07:02 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/11 21:18:25 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config Config::parse()
{
	static int i = 0;

	Config config;

	if (i == 0)
	{
		static uint16_t port[4] = {8000, 8001, 8002, 0};
		config._server_name = "MyServer";
		config._port = port;
		config._addr = 0;
		std::cout << "Configuration parsed successfully." << std::endl;
		i++;
		return config;
	}
	// if (i == 1)
	// {
	// 	static uint16_t port[3] = {8081, 8082, 0};
	// 	config.set_server_name("AnotherServer");
	// 	config.set_port(port);
	// 	config.set_addr(0);
	// 	std::cout << "Configuration parsed successfully." << std::endl;
	// 	i++;
	// 	return config;
	// }
	// if (i == 2)
	// {
	// 	static uint16_t port[2] = {7070, 0};
	// 	config.set_server_name("ThirdServer");
	// 	config.set_port(port);
	// 	config.set_addr(0);
	// 	std::cout << "Configuration parsed successfully." << std::endl;
	// 	i++;
	// 	return config;
	// }
	return Config(); // Return an empty config to signal no more configurations
}

Config* Config::parse_file(std::string file)
{
	std::string tmp, str;
	std::ifstream ifs(file.c_str());

	if (!ifs.good())
		throw std::runtime_error("");
	getline(ifs, str, '\0');
	while (str.find("server") != std::string::npos)
	{
		Config config;
		tmp = extract_server_block(str);
		config._server_name = extract_token(tmp, "server_name");
		config.extract_address(tmp);
		std::cout << config._server_name << std::endl;
	}
	ifs.close();
	return NULL;
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
	tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace));
	return tmp;
}

uint32_t Config::extract_address(std::string& str)
{
	std::string tmp;
	// in_addr_t addr;

	tmp = extract_token(str, "listen");
	_addr = inet_addr(tmp.substr(0, tmp.find(':')).c_str());
	int i = atoi(tmp.substr(tmp.find(':') + 1).c_str());
	std::cout << _addr << " " << i << std::endl;
	return _addr;
}