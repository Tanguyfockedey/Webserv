/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:38:05 by jrichir           #+#    #+#             */
/*   Updated: 2025/11/04 14:34:09 by jrichir          ###   ########.fr       */
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

std::string server_path()
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

bool	is_directory(std::string path)
{
	struct stat s;

	//if (path.length() > 0 && path[path.length() - 1] == '/')
	//	return true;
	if (lstat(path.c_str(), &s) == 0 && S_ISDIR(s.st_mode))
		return true;
	return false;
}

bool	is_regular_file(std::string path)
{
	struct stat s;

	if (lstat(path.c_str(), &s) == 0 && S_ISREG(s.st_mode))
		return true;
	return false;
}

bool	is_symlink(std::string path)
{
	struct stat s;

	if (lstat(path.c_str(), &s) == 0 && S_ISLNK(s.st_mode))
		return true;
	return false;
}

std::string get_file_type(const std::string path)
{
	if (is_symlink(path))
		return "symlink";
	else if (is_directory(path))
		return "directory";
	else if (is_regular_file(path))
		return "regular_file";
	else
		return "nonexistent";
}
