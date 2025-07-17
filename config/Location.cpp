/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:35:09 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/17 15:03:35 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(std::string str)
{
	_root = Config::extract_token(str, "root");
	_index = Config::extract_token(str, "index");
	_client_body_size = Config::extract_client_body_size(str);
	_allow_methods = Config::extract_token(str, "method");
}

Location::~Location()
{}
