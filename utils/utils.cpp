/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrichir <jrichir@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 16:38:05 by jrichir           #+#    #+#             */
/*   Updated: 2025/07/23 16:40:13 by jrichir          ###   ########.fr       */
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
