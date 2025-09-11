/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:37:58 by jrichir           #+#    #+#             */
/*   Updated: 2025/08/27 12:21:10 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../webserv.hpp"

std::string join_paths(const std::string &path_left, const std::string &path_right);
std::string root_directory();
bool	is_directory(std::string path);
bool	is_regular_file(std::string path);
bool	is_symlink(std::string path);
std::string get_file_type(const std::string path);