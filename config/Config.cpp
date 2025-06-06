/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:07:02 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/05 20:02:58 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config Config::parse()
{
	static int i = 0;

	Config config;

	if (i == 0)
	{
		static uint16_t port[4] = {8000, 8001, 8002, 0}; // Example port
		config.set_server_name("MyServer");
		config.set_port(port);
		config.set_addr(0);
		std::cout << "Configuration parsed successfully." << std::endl;
		i++; // Increment to move to the next configuration
		return config; // Return the first configuration
	}
	if (i == 1)
	{
		static uint16_t port[3] = {8081, 8082, 0}; // Example port for the second config
		config.set_server_name("AnotherServer");
		config.set_port(port);
		config.set_addr(0);
		std::cout << "Configuration parsed successfully." << std::endl;
		i++; // Increment to move to the next configuration
		return config; // Return the second configuration
	}
	if (i == 2)
	{
		static uint16_t port[2] = {7070, 0};
		config.set_server_name("ThirdServer");
		config.set_port(port);
		config.set_addr(0);
		std::cout << "Configuration parsed successfully." << std::endl;
		i++; // Increment to signal no more configurations
		return config; // Return the third configuration
	}
	return Config(); // Return an empty config to signal no more configurations
}