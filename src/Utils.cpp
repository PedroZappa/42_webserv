/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 15:20:31 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 16:01:44 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Utils.hpp"
#include "../inc/Webserv.hpp"

/* ************************************************************************** */
/*                                Convertions                                 */
/* ************************************************************************** */

/// @brief Converts a string to lowercase
/// @param str The string to convert
/// @return The lowercase string
std::string toLower(const std::string &str) {
	std::string lower = str;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return (lower);
}

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
