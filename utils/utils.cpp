/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:38:05 by jrichir           #+#    #+#             */
/*   Updated: 2025/08/21 12:55:55 by jrichir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::string join_paths(const std::string &path_left, const std::string &path_right)
{
	if (path_left.empty())
		return path_right;
	if (path_right.empty())
		return path_left;

	char left_last = path_left[path_left.length() - 1];
	char right_first = path_right[0];

	if (left_last == '/' && right_first == '/')
		return path_left + path_right.substr(1);
	else if (left_last != '/' && right_first != '/')
		return path_left + "/" + path_right;
	else
		return path_left + path_right;
}

std::string root_directory()
{
	char* malloc_str;
	std::string str;
	
	if (!(malloc_str = get_current_dir_name()))
	{
		std::cerr << "Error getting current working directory: " << strerror(errno) << std::endl;
		return "";
	}
	str = malloc_str;
	free(malloc_str);
	// str = str.substr(0, str.find("Webserv") + 7); //uncomment for debugging, remove for production
	return str;
}
