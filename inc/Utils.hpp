/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 15:18:14 by passunca          #+#    #+#             */
/*   Updated: 2025/01/10 17:33:49 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "Server.hpp"
#include "Webserv.hpp"
#include "Logger.hpp"

/* ************************************************************************** */
/*                                Convertions                                 */
/* ************************************************************************** */

std::string toLower(const std::string &str);
Method string2method(const std::string &str);
std::string method2string(Method method);

/// @brief Converts a number to a string
/// @param num The number to be converted
/// @return The string
/// @throws std::invalid_argument if the number cannot be converted
template <typename T> std::string nToStr(T num) {
	std::stringstream ss;
	ss << num;
	if (ss.fail() || !ss.eof())
		throw std::invalid_argument(
			"Failed to convert " + std::string(typeid(T).name()) + " to string");
	return (ss.str());
}

/// @brief Converts a string to a number (int, float, etc)
/// @param str The string to convert
/// @return The number
/// @throws std::invalid_argument if the string cannot be converted
template <typename T> T strToN(const std::string &str) {
	T num;
	std::stringstream ss(str);
	ss >> num;
	if (ss.fail() || !ss.eof())
		throw std::invalid_argument("Failed to convert string to " +
									std::string(typeid(T).name()));
	return (num);
}

#endif
