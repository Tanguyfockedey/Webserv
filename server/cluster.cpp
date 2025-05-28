/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 16:48:31 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/28 17:05:51 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cluster.hpp"

cluster::cluster() : server(0) // Initialize with a dummy port
{
	std::cout << "Cluster initialized." << std::endl;
}

cluster::~cluster()
{
	std::cout << "Cluster destructor called." << std::endl;
	for (size_t i = 0; i < _servers.size(); ++i)
		_servers[i].~server();
	_servers.clear();
}

void cluster::add_server(int port)
{
	server new_server(port);
	_servers.push_back(new_server);
	std::cout << "Server added on port " << port << std::endl;
}

void cluster::remove_server(int port)
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		if (_servers[i].get_port() == port)
		{
			_servers[i].~server();
			_servers.erase(_servers.begin() + i);
			std::cout << "Server removed from port " << port << std::endl;
			return;
		}
	}
	std::cerr << "No server found on port " << port << std::endl;
}

