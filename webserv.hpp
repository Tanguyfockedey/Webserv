/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 19:39:13 by tafocked          #+#    #+#             */
/*   Updated: 2025/07/30 15:35:57 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// C Standard Library
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// C++ Standard Library
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <ctime>
#include <cstdio>

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

// Project helper functions
#include "utils/utils.hpp"

// Constants
#define CONNECTION_TIMEOUT 3600
#define REQUEST_TIMEOUT 3600
#define BUFFER_SIZE 65536 // 64 KB
#define MAX_URI_LENGTH 2048 // 2 KB
#define MAX_HEADER_LENGTH 8192 // 8 KB
#define MAX_BODY_LENGTH 4294967296 // 4GB 
#define UPLOAD_PATH "/uploads/"
#define REQUIRE_HEADERS 0

// Colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RESET "\033[0m"
