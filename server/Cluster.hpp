/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 16:48:38 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/13 17:21:25 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// #include "../webserv.hpp"
#include "Server.hpp"
#include "../config/Config.hpp"

class Cluster
{
	private:
		std::vector<Server*> _cluster;

	protected:

	public:
		// Constructor and Destructor
		Cluster();
		~Cluster();

		// Methods
		void init_cluster(std::vector<Config> config);
		void add_server(const Config config);
		void remove_server(const int i);
		void run_servers();
};