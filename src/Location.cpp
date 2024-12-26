/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:23:58 by passunca          #+#    #+#             */
/*   Updated: 2024/12/26 11:58:25 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Location.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

Location::Location(void) : _cliMaxBodySize(-1) {
	initDirectiveMap();
}

Location::Location(const Location &copy)
	: _root(copy._root), _cliMaxBodySize(copy._cliMaxBodySize),
	  _errorPage(copy._errorPage) {
}

Location::~Location(void) {
}

/* ************************************************************************** */
/*                                  Operators                                 */
/* ************************************************************************** */

/// @brief Copy Operator
/// @param src The object to copy
Location &Location::operator=(const Location &src) {
	_root = src.getRoot();
	_cliMaxBodySize = src.getCliMaxBodySize();
	_errorPage = src.getErrorPage();
	return (*this);
}

/* ************************************************************************** */
/*                                Server Setup                                */
/* ************************************************************************** */

void Location::initDirectiveMap(void) {
	// _directiveMap["client_max_body_size"] = &Location::setCliMaxBodySize;
	// _directiveMap["error_page"] = &Location::setErrorPage;
	// _directiveMap["root"] = &Location::setRoot;
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

/// @brief Get the Root value
std::string Location::getRoot(void) const {
	return (_root);
}

/// @brief Get the CliMaxBodySize value
long Location::getCliMaxBodySize(void) const {
	return (_cliMaxBodySize);
}

/// @brief Get the ErrorPage value
std::map<short, std::string> Location::getErrorPage(void) const {
	return (_errorPage);
}

/* ************************************************************************** */
/*                                  Setters                                   */
/* ************************************************************************** */

void Location::setDirective(std::string &directive) {
	(void)directive;
}
