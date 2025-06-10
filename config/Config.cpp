/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:07:02 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/10 19:58:27 by tafocked         ###   ########.fr       */
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
	std::ifstream is(file.c_str());

	if (!is.good())
		throw std::runtime_error("");
	getline(is, str, '\0');
	while (str.find("server") != std::string::npos)
	{
		tmp = extract_server_block(str);
		std::cout << tmp << std::endl;
	}
	is.close();
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
