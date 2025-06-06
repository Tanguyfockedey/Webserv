/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:59:00 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/05 21:16:20 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "server/Cluster.hpp"


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
	
	// uint16_t ports1[] = {8000, 8001, 8002, 0};
	// uint16_t ports2[] = {8080, 8081, 8082, 0};
	// uint16_t ports3[] = {9000, 9001, 9002, 0};

	try
	{
		Cluster cluster;
		cluster.init_cluster();
		cluster.run_servers();

	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		std::cerr << strerror(errno) << std::endl;
		return (1);
	}
	return (0);
}
