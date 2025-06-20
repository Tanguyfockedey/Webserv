/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 19:39:13 by tafocked          #+#    #+#             */
/*   Updated: 2025/06/21 00:13:12 by jrichir          ###   ########.fr       */
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
#include <fstream>
#include <sstream>
#include <exception>
#include <ctime>

// Network
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

// Containers
#include <algorithm>
#include <vector>
#include <map>
#include <utility>

// Constants
#define CONNECTION_TIMEOUT 30
#define REQUEST_TIMEOUT 5

// Colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RESET "\033[0m"
