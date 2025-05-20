/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:59:00 by tafocked          #+#    #+#             */
/*   Updated: 2025/05/20 16:15:50 by tafocked         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Error: No configuration file provided." << std::endl;
		return (1);
	}
	if (argc > 2)
	{
		std::cerr << "Error: Too many arguments provided." << std::endl;
		return (1);
	}
	return (0);
}