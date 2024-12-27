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
#include "../inc/ConfParser.hpp"
#include "../inc/Debug.hpp"
#include "../inc/Webserv.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

Location::Location(void) : _autoIndex(UNSET), _cliMaxBodySize(-1) {
	initDirectiveMap();
}

Location::Location(const Location &copy)
	: _root(copy.getRoot()), _index(copy.getIndex()),
	  _autoIndex(copy.getAutoIndex()), _cliMaxBodySize(copy.getCliMaxBodySize()),
	  _errorPage(copy.getErrorPage()) {
}

Location::~Location(void) {
}

/* ************************************************************************** */
/*                                 Local Data                                 */
/* ************************************************************************** */

const MethodMapping Location::methodMap[] = {
	{"GET", GET},
	{"HEAD", HEAD},
	{"POST", POST},
	{"PUT", PUT},
	{"DELETE", DELETE},
	{"CONNECT", CONNECT},
	{"OPTIONS", OPTIONS},
	{"TRACE", TRACE},
	{"PATCH", PATCH},
	{NULL, Method(0)} // Sentinel value
};

/* ************************************************************************** */
/*                                  Operators                                 */
/* ************************************************************************** */

/// @brief Copy Operator
/// @param src The object to copy
Location &Location::operator=(const Location &src) {
	_root = src.getRoot();
	_index = src.getIndex();
	_autoIndex = src.getAutoIndex();
	_cliMaxBodySize = src.getCliMaxBodySize();
	_errorPage = src.getErrorPage();
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Location &ctx) {
	os << BRED "Location Configuration:" NC << std::endl;

	os << BYEL "Root:\n" NC << ctx.getRoot() << std::endl;

	os << BYEL "Location Index Files:\n" NC;
	std::vector<std::string> index = ctx.getIndex();
	std::vector<std::string>::const_iterator it;
	for (it = index.begin(); it != index.end(); it++)
		os << *it << std::endl;

	os << BYEL "Client Max Body Size:\n" NC << ctx.getCliMaxBodySize()
	   << std::endl;
	return (os);
}

/* ************************************************************************** */
/*                                Server Setup                                */
/* ************************************************************************** */

void Location::initDirectiveMap(void) {
	_directiveMap["root"] = &Location::setRoot;
	_directiveMap["index"] = &Location::setIndex;
	_directiveMap["limit_except"] = &Location::setLimitExcept;
	_directiveMap["autoindex"] = &Location::setAutoIndex;
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

/// @brief Get the Index value
std::vector<std::string> Location::getIndex(void) const {
	return (_index);
}

/// @brief Get the LimitExcept value
std::set<Method> Location::getLimitExcept(void) const {
	return (_validMethods);
}

/// @brief Get the AutoIndex value
State Location::getAutoIndex(void) const {
	return (_autoIndex);
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

/// @brief Set a directive
/// @param directive The directive to set
/// @throw std::runtime_error if the directive is invalid
void Location::setDirective(std::string &directive) {
#ifdef DEBUG
	DEBUG_LOCUS(FSTART, "processing directive: " YEL + directive + NC);
#endif
	std::vector<std::string> tks;
	tks = ConfParser::tokenizer(directive); // Tokenize
	if (tks.size() < 2)
		throw std::runtime_error("Directive " RED + tks[0] + NC " is invalid");
#ifdef DEBUG
	showContainer(__func__, "Directive Tokens", tks);
#endif

	std::map<std::string, DirHandler>::const_iterator it;
	it = _directiveMap.find(tks[0]);
	if (it != _directiveMap.end())
		(this->*(it->second))(tks);
	else
		throw std::runtime_error("Directive " RED + tks[0] + NC " is invalid");
#ifdef DEBUG
	DEBUG_LOCUS(FEND, "set directive: " YEL + directive + " " NC);
#endif
}

/// @brief Set the Root value
/// @param tks The tokens of the root directive
void Location::setRoot(std::vector<std::string> &tks) {
	if (tks.size() > 2)
		throw std::runtime_error("Invalid number of args in root directive");
	if (!_root.empty())
		throw std::runtime_error("Root already set bruh!");
	this->_root = tks[1];
#ifdef DEBUG
	DEBUG_LOCUS(FEND, "processed root directive: " YEL + _root + NC);
#endif
}

/// @brief Set the Index value
/// @param tks The tokens of the index directive
void Location::setIndex(std::vector<std::string> &tks) {
	tks.erase(tks.begin()); // Remove 'index'
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin(); it != tks.end(); it++)
		this->_index.push_back(*it);
#ifdef DEBUG
	DEBUG_LOCUS(FEND, "processed index directive: " YEL + _index[0] + NC);
#endif
}

/// @brief Set the LimitExcept value
/// @param tks The tokens of the limit_except directive
/// @throw std::runtime_error if the method is invalid
void Location::setLimitExcept(std::vector<std::string> &tks) {
	if (!_validMethods.empty()) {
		throw std::runtime_error("Limit_except already set bruh!");
	}

	std::set<Method> methods;
	std::vector<std::string>::const_iterator it;
	// Start from tks.begin() + 1 to skip the directive name
	for (it = tks.begin() + 1; it != tks.end(); ++it) {
		// Search through methodMap for matching method
		bool methodFound = false;
		for (const MethodMapping *map = methodMap; map->str != NULL; ++map) {
			if (*it == map->str) {
				methods.insert(map->method);
				methodFound = true;
				break;
			}
		}
		if (!methodFound)
			throw std::runtime_error("Invalid method in limit_except "
									 "directive: " +
									 *it);
	}
	this->_validMethods = methods;

#ifdef DEBUG
	showContainer(__func__, "Parsed Methods", _validMethods);
	DEBUG_LOCUS(FEND, "processed limit_except directive: " YEL);
#endif
}

/// @brief Set the AutoIndex value
/// @param tks The tokens of the autoindex directive
void Location::setAutoIndex(std::vector<std::string> &tks) {
	#ifdef DEBUG
	DEBUG_LOCUS(FSTART, "processing directive: " YEL + tks[0] + NC);
#endif
	if (_autoIndex == TRUE || _autoIndex == FALSE)
		throw std::runtime_error("Autoindex already set");
	if (tks[1] == "on")
		_autoIndex = TRUE;
	else if (tks[1] == "off")
		_autoIndex = FALSE;
	else
		throw std::runtime_error("Invalid autoindex directive");

#ifdef DEBUG
	DEBUG_LOCUS(FEND, "processed directive: " YEL + tks[1]);
#endif
}
