/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 15:20:31 by passunca          #+#    #+#             */
/*   Updated: 2025/01/10 17:33:52 by passunca         ###   ########.fr       */
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

std::string method2string(Method method) {
	switch (method) {
	case GET:
		return ("GET");
	case POST:
		return ("POST");
	case DELETE:
		return ("DELETE");
	default:
		return ("UNKNOWN");
	}
}

Method string2method(const std::string &str) {
	if (str == "GET")
		return (GET);
	else if (str == "POST")
		return (POST);
	else if (str == "DELETE")
		return (DELETE);
	else
		return (UNKNOWN);
}
