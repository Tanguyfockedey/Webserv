/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:59:00 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/06 17:54:32 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "server/Cluster.hpp"


int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

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
