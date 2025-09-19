/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:37:58 by jrichir           #+#    #+#             */
/*   Updated: 2025/09/19 14:43:07 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

std::string join_paths(const std::string &path_left, const std::string &path_right);
std::string server_path();
bool	is_directory(std::string path);
bool	is_regular_file(std::string path);
bool	is_symlink(std::string path);
std::string get_file_type(const std::string path);