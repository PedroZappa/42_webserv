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
