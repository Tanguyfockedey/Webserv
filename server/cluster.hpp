/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 16:48:38 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/28 17:30:49 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "server.hpp"

class cluster : public server
{
	private:
		std::vector<server> _servers;

	protected:

	public:
		// Constructor and Destructor
		cluster();
		~cluster();

		// Methods
		void add_server(int port);
		void remove_server(int port);
};