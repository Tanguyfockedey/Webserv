/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:22:31 by tafocked          #+#    #+#             */
/*   Updated: 2025/08/11 16:07:29 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd): _fd(fd), _last_activity(time(NULL))//, _waiting_request(false)
{
	std::cout << YELLOW << "Client [" << _fd << "] connected." << RESET << std::endl;
}

Client::~Client()
{}
