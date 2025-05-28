/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:59:00 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/28 19:07:53 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "server/server.hpp"
#include <errno.h>
#include  <string.h>


int main(int argc, char **argv)
{
	// if (argc < 2)
	// {
	// 	std::cerr << "Error: No configuration file provided." << std::endl;
	// 	return (1);
	// }
	// if (argc > 2)
	// {
	// 	std::cerr << "Error: Too many arguments provided." << std::endl;
	// 	return (1);
	// }
	(void)argc;
	(void)argv;
	
	uint16_t port[] = {8000, 8001, 0};

	try
	{
		server* myserver = new server(port, INADDR_ANY);
		myserver->polling();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		std::cerr << strerror(errno) << std::endl;
		return (1);
	}
	return (0);
}
