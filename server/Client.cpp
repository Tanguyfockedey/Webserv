/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:22:31 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/29 15:52:36 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd): _fd(fd), _last_activity(time(NULL)), _waiting_request(false)
{
	std::cout << "Client created with fd: " << _fd << std::endl;
}

Client::~Client()
{}
