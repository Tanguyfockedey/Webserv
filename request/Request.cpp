/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 14:48:10 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/18 16:15:45 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const int fd, const std::string raw_request)
	: fd(fd),_timestamp(time(NULL)), _raw_request(raw_request)
{}

Request::~Request()
{}

