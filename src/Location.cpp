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
	: _root(copy.getRoot()), _cliMaxBodySize(copy.getCliMaxBodySize()),
	  _errorPage(copy.getErrorPage()) {
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

std::ostream &operator<<(std::ostream &os, const Location &ctx) {
	os << BRED "Location Configuration:" NC << std::endl;
	os << BYEL "Root:\n" NC << ctx.getRoot() << std::endl;
	os << BYEL "Client Max Body Size:\n" NC << ctx.getCliMaxBodySize()
	   << std::endl;
	return (os);
}

/* ************************************************************************** */
/*                                Server Setup                                */
/* ************************************************************************** */

void Location::initDirectiveMap(void) {
	_directiveMap["root"] = &Location::setRoot;
	// _directiveMap["client_max_body_size"] = &Location::setCliMaxBodySize;
	// _directiveMap["error_page"] = &Location::setErrorPage;
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

/// @brief Set the Root value
/// @param tks The tokens of the root directive
void Location::setRoot(std::vector<std::string> &tks) {
	if (tks.size() > 2)
		throw std::runtime_error("Invalid number of aargs in root directive");
	if (!_root.empty())
		throw std::runtime_error("Root already set");
	_root = tks[1];
}

void Location::setDirective(std::string &directive) {
	(void)directive;
}
