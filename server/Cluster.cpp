/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 16:48:31 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/13 17:22:33 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

Cluster::Cluster()
{
	std::cout << "Cluster constructor called." << std::endl;
}

Cluster::~Cluster()
{
	std::cout << "Cluster destructor called." << std::endl;
	for (size_t i = 0; i < _cluster.size(); ++i)
		delete _cluster[i];
	_cluster.clear();
}

void Cluster::init_cluster(std::vector<Config> config)
{
	std::cout << "Cluster initialized." << std::endl;
	for (std::vector<Config>::iterator i = config.begin(); i < config.end(); i++)
		add_server(*i);
	config.clear();
}

void Cluster::add_server(const Config config)
{
	Server *new_server= new Server(config.get_port(), config.get_addr(), config.get_server_name());
	_cluster.push_back(new_server);
	std::cout << "Server '" << new_server->get_server_name() << "' added to cluster." << std::endl;
}

void Cluster::remove_server(const int i)
{
	if ((_cluster.begin() + i) < _cluster.end() && i >= 0)
	{
		delete _cluster[i];
		_cluster.erase(_cluster.begin() + i);
	}
}

void Cluster::run_servers() 
{
	while (true)
	{
		for (size_t i = 0; i < _cluster.size(); ++i) 
			_cluster[i]->polling();
	}
}
