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

std::vector<std::string> Location::getIndex(void) const {
	return (_index);
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
	std::vector<std::string> tks;
	tks = ConfParser::tokenizer(directive); // Tokenize
	if (tks.size() < 2)
		throw std::runtime_error("Directive " RED + tks[0] + NC " is invalid");

	std::map<std::string, DirHandler>::const_iterator it;
	it = _directiveMap.find(tks[0]);
	if (it != _directiveMap.end())
		(this->*(it->second))(tks);
	else
		throw std::runtime_error("Directive " RED + tks[0] + NC " is invalid");

}

/// @brief Set the Root value
/// @param tks The tokens of the root directive
void Location::setRoot(std::vector<std::string> &tks) {
	if (tks.size() > 2)
		throw std::runtime_error("Invalid number of args in root directive");
	if (!_root.empty())
		throw std::runtime_error("Root already set bruh!");
	_root = tks[1];
}

/// @brief Set the Index value
/// @param tks The tokens of the index directive
void Location::setIndex(std::vector<std::string> &tks) {
tks.erase(tks.begin()); // Remove 'index'
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin(); it != tks.end(); it++)
		_index.push_back(*it);
}
