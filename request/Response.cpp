/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 18:08:26 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/18 18:12:23 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(const int fd): _fd(fd)
{
	_raw_response = "HTTP/1.1 200 ok\r\n\r\n";
}

Response::~Response()
{}
