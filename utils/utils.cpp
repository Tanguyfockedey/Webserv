/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:38:05 by jrichir           #+#    #+#             */
/*   Updated: 2025/11/21 13:58:27 by jrichir          ###   ########.fr       */
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

std::string common_path(const std::string &path1, const std::string &path2)
{
	size_t min_length = std::min(path1.length(), path2.length());
	size_t last_slash_pos = 0;

	for (size_t i = 0; i < min_length; ++i)
	{
		if (path1[i] != path2[i])
			break;
		if (path1[i] == '/')
			last_slash_pos = i;
	}
	return path1.substr(0, last_slash_pos + 1);
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
	return str;
}

bool	is_directory(std::string path)
{
	struct stat s;

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

size_t uint_from_string(const std::string &str)
{
	size_t result;
	std::istringstream iss(str);

	iss >> result;
	return result;
}
