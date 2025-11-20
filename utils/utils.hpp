/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:37:58 by jrichir           #+#    #+#             */
/*   Updated: 2025/11/19 23:21:59 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

std::string join_paths(const std::string &path_left, const std::string &path_right);
std::string common_path(const std::string &path1, const std::string &path2);
std::string server_path();
bool	is_directory(std::string path);
bool	is_regular_file(std::string path);
bool	is_symlink(std::string path);
std::string get_file_type(const std::string path);