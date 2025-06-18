/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:31:44 by jrichir           #+#    #+#             */
/*   Updated: 2025/06/18 16:32:58 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(const int fd, const std::string raw_response)
	: fd(fd),_timestamp(time(NULL)), _raw_response(raw_response)
{}

Response::~Response()
{}

