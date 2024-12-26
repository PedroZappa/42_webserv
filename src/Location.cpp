/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:23:58 by passunca          #+#    #+#             */
/*   Updated: 2024/12/26 11:45:47 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Location.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

Location::Location(void) {
}

Location::Location(const Location &copy) {
	*this = copy;
}

Location::~Location(void) {
}

/* ************************************************************************** */
/*                                  Operators                                 */
/* ************************************************************************** */

/// @brief Copy Operator
/// @param src The object to copy
Location &Location::operator=(const Location &src) {
	(void)src;
	return (*this);
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

/// @brief Get the CliMaxBodySize value
long Location::getCliMaxBodySize(void) const {
	return (_cliMaxBodySize);
}

/// @brief Get the ErrorPage value
std::map<short, std::string> Location::getErrorPage(void) const {
	return (_errorPage);
}


/// @brief Get the Root value
std::string Location::getRoot(void) const {
	return (_root);
}

/* ************************************************************************** */
/*                                  Setters                                   */
/* ************************************************************************** */

void Location::setDirective(std::string &directive) {
	(void)directive;
}
