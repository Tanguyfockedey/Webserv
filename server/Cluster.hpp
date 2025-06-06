/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 16:48:38 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/06 14:04:31 by tafocked         ###   ########.fr       */
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
		void init_cluster();
		void add_server(Config config);
		void remove_server(int i);
		void run_servers();
};