/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 19:39:13 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/09 18:26:27 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// C Standard Library
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

// C++ Standard Library
#include <iostream>

// Network
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

// Containers
#include <algorithm>
#include <vector>
#include <map>
#include <utility>

// Constants
#define CONNECTION_TIMEOUT 60
