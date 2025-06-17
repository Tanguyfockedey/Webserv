/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:59:00 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/14 19:50:12 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "server/Cluster.hpp"


int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Too many arguments provided." << std::endl;
		return (1);
	}
	Cluster cluster;
	std::string config_file = "/home/tafocked/Documents/Webserv/config/config1.conf";
	if (argc == 2)
		config_file = argv[1];
	try
	{
		cluster.init_cluster(Config::parse_file(config_file));
		if (cluster.get_cluster().empty())
			return (1);
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
