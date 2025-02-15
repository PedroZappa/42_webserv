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
#include "../inc/Logger.hpp"
#include "../inc/Webserv.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

Location::Location(void) : _autoIndex(UNSET), _clientMaxBodySize(-1)
{
	initDirectiveMap();
	_return = std::make_pair(-1, "");
}

Location::Location(const Location &copy)
	: _root(copy.getRoot()), _index(copy.getIndex()),
	  _autoIndex(copy.getAutoIndex()),
	  _clientMaxBodySize(copy.getClientMaxBodySize()),
	  _validMethods(copy.getLimitExcept()), _errorPage(copy.getErrorPage()),
	  _uploadStore(copy.getUploadStore()), _return(copy.getReturn()),
	  _cgiExt(copy.getCgiExt())
{
}

Location::~Location(void) {}

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
Location &Location::operator=(const Location &src)
{
	_root = src.getRoot();
	_index = src.getIndex();
	_autoIndex = src.getAutoIndex();
	_clientMaxBodySize = src.getClientMaxBodySize();
	_validMethods = src.getLimitExcept();
	_errorPage = src.getErrorPage();
	_uploadStore = src.getUploadStore();
	_return = src.getReturn();
	_cgiExt = src.getCgiExt();
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Location &ctx)
{
	os << BRED "Location Configuration:" NC << std::endl;

	os << BYEL "Root:\n" NC << ctx.getRoot() << std::endl;

	os << BYEL "Location Index Files:\n" NC;
	std::vector<std::string> index = ctx.getIndex();
	std::vector<std::string>::const_iterator it;
	for (it = index.begin(); it != index.end(); ++it)
		os << *it << std::endl;

	os << BYEL "AutoIndex:\n" NC
	   << ((ctx.getAutoIndex() == TRUE		? "TRUE"
			: (ctx.getAutoIndex() == FALSE) ? "FALSE"
											: "UNSET"))
	   << std::endl;

	os << BYEL "Client Max Body Size:\n" NC << ctx.getClientMaxBodySize()
	   << std::endl;

	os << BYEL "Error Pages:\n" NC;
	std::map<short, std::string> errPages = ctx.getErrorPage();
	std::map<short, std::string>::const_iterator errit;
	for (errit = errPages.begin(); errit != errPages.end(); ++errit)
		os << errit->first << ": " << errit->second << std::endl;

	os << BYEL "Allowed Methods:\n" NC;
	std::set<Method> methods = ctx.getLimitExcept();
	std::set<Method>::const_iterator mit;
	for (mit = methods.begin(); mit != methods.end(); ++mit)
	{
		bool found = false;
		for (int i = 0; Location::methodMap[i].str != NULL; ++i)
		{
			if (Location::methodMap[i].method == *mit)
			{
				os << Location::methodMap[i].str << " ";
				found = true;
				break;
			}
		}
		if (!found)
			os << "UNKNOWN ";
	}
	os << "\n";

	os << BYEL "Upload Store:\n" NC << ctx.getUploadStore() << std::endl;

	os << BYEL "Return:\n" NC;
	std::pair<short, std::string> ret = ctx.getReturn();
	if (ret.first != -1)
		os << ret.first << ": " << ret.second << std::endl;

	os << BYEL "Cgi Extension:\n" NC << ctx.getCgiExt() << std::endl;

	return (os);
}

/* ************************************************************************** */
/*                                Server Setup                                */
/* ************************************************************************** */

void Location::initDirectiveMap(void)
{
	_directiveMap["root"] = &Location::setRoot;
	_directiveMap["index"] = &Location::setIndex;
	_directiveMap["limit_except"] = &Location::setLimitExcept;
	_directiveMap["autoindex"] = &Location::setAutoIndex;
	_directiveMap["client_max_body_size"] = &Location::setClientMaxBodySize;
	_directiveMap["limit_except"] = &Location::setLimitExcept;
	_directiveMap["error_page"] = &Location::setErrorPage;
	_directiveMap["upload_store"] = &Location::setUploadStore;
	_directiveMap["return"] = &Location::setReturn;
	_directiveMap["cgi_ext"] = &Location::setCgiExt;
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

/// @brief Get the Root value
std::string Location::getRoot(void) const
{
	return (_root);
}

/// @brief Get the Index value
std::vector<std::string> Location::getIndex(void) const
{
	return (_index);
}

/// @brief Get the LimitExcept value
std::set<Method> Location::getLimitExcept(void) const
{
	return (_validMethods);
}

/// @brief Get the AutoIndex value
State Location::getAutoIndex(void) const
{
	return (_autoIndex);
}

/// @brief Get the CliMaxBodySize value
long Location::getClientMaxBodySize(void) const
{
	return (_clientMaxBodySize);
}

/// @brief Get the ErrorPage value
std::map<short, std::string> Location::getErrorPage(void) const
{
	return (_errorPage);
}

/// @brief Get the UploadStore value
std::string Location::getUploadStore(void) const
{
	return (_uploadStore);
}

/// @brief Get the Return value
std::pair<short, std::string> Location::getReturn(void) const
{
	return (_return);
}

std::string Location::getCgiExt(void) const
{
	return (_cgiExt);
}

/* ************************************************************************** */
/*                                  Setters                                   */
/* ************************************************************************** */

/// @brief Set a directive
/// @param directive The directive to set
/// @throw std::runtime_error if the directive is invalid
void Location::setDirective(std::string &directive)
{
	Logger::debug("Location", __func__, "Processing directive: " YEL + directive + NC);
	std::vector<std::string> tks;
	tks = ConfParser::tokenizer(directive); // Tokenize
	if (tks.empty())
		throw std::runtime_error("No directive to set");
	if (tks.size() < 2)
		throw std::runtime_error("Directive " RED + tks[0] + NC " is invalid");

	showContainer(__func__, "Directive Tokens", tks);

	std::map<std::string, DirHandler>::const_iterator it;
	it = _directiveMap.find(tks[0]);
	if (it == _directiveMap.end())
		throw std::runtime_error("Directive " RED + tks[0] + NC " is invalid");
		
	(this->*(it->second))(tks);
	Logger::debug("Location", __func__, "Set directive: " YEL + directive + " " NC);
}

/// @brief Set the Root value
/// @param root The root to set
void Location::setRoot(std::string &root)
{
	Logger::debug("Location", __func__, "Processing root directive: " YEL + root + NC);
	if (!_root.empty())
		throw std::runtime_error("Root already set");
	_root = root;
	Logger::debug("Location", __func__, "Processed root directive: " YEL + _root + NC);
}

/// @brief Set the Root value
/// @param tks The tokens of the root directive
void Location::setRoot(std::vector<std::string> &tks)
{
	if (tks.size() > 2)
		throw std::runtime_error("Invalid number of args in root directive");
	if (!_root.empty())
		throw std::runtime_error("Root already set bruh!");
	_root = tks[1];
	Logger::debug("Location", __func__, "Processed root directive: " YEL + _root + NC);
}

/// @brief Set the Index value
/// @param tks The tokens of the index directive
void Location::setIndex(std::vector<std::string> &tks)
{
	tks.erase(tks.begin()); // Remove 'index'
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin(); it != tks.end(); it++)
		this->_index.push_back(*it);
	Logger::debug("Location", __func__, "Processed index directive: " YEL + _index[0] + NC);
}

/// @brief Set the LimitExcept value
/// @param tks The tokens of the limit_except directive
/// @throw std::runtime_error if the method is invalid
void Location::setLimitExcept(std::vector<std::string> &tks)
{
	if (!_validMethods.empty())
		throw std::runtime_error("Limit_except already set bruh!");

	std::set<Method> methods;
	std::vector<std::string>::const_iterator it;
	// Start from tks.begin() + 1 to skip the directive name
	for (it = tks.begin() + 1; it != tks.end(); ++it)
	{
		// Search through methodMap for matching method
		bool methodFound = false;
		for (const MethodMapping *map = methodMap; map->str != NULL; ++map)
		{
			if (*it == map->str)
			{
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
	_validMethods = methods;

	showContainer(__func__, "Parsed Methods", _validMethods);
	Logger::debug("Location", __func__, "Processed limit_except directive: " YEL);
}

/// @brief Set the AutoIndex value
/// @param tks The tokens of the autoindex directive
void Location::setAutoIndex(std::vector<std::string> &tks)
{
	Logger::debug("Location", __func__, "processing directive: " YEL + tks[0] + NC);

	if (_autoIndex == TRUE || _autoIndex == FALSE)
		throw std::runtime_error("Autoindex already set");
	if (tks[1] == "on")
		_autoIndex = TRUE;
	else if (tks[1] == "off")
		_autoIndex = FALSE;
	else
		throw std::runtime_error("Invalid autoindex directive");

	Logger::debug("Location", __func__, "Processed directive: " YEL + tks[1]);
}

/// @brief Set the MaxBodySize value
/// @param tks The tokens of the max_body_size directive
/// @throw std::runtime_error if the max_body_size directive is invalid
void Location::setClientMaxBodySize(std::vector<std::string> &tks)
{
	Logger::debug("Location", __func__, "Processing directive: " YEL + tks[0] + NC);

	if (tks.size() != 2) // Check number of tokens
		throw std::runtime_error("Invalid max_body_size directive: " + tks[0]);
	if (_clientMaxBodySize != -1) // Check if already set
		throw std::runtime_error("Max body size already set");

	std::string maxSize = tks[1];
	char unit = maxSize[maxSize.size() - 1]; // Extract last character
	if (!std::isdigit(unit))
		maxSize.resize(maxSize.size() - 1); // Remove last character

	// Overflow checking
	char *endPtr = NULL;
	long size = std::strtol(maxSize.c_str(), &endPtr, 10);
	if ((*endPtr != '\0') | (size < 0))
		throw std::runtime_error("Invalid max_body_size directive: " + tks[1]);

	// Applying unit checking for overflow
	_clientMaxBodySize = size;
	if (!std::isdigit(unit))
	{
		switch (unit)
		{
		case 'k':
		case 'K':
			if (size > (LONG_MAX / KB))
				throw std::runtime_error("Invalid max_body_size value: " +
										 tks[1] + RED " : overflow" NC);
			_clientMaxBodySize = (size * KB);
			break;
		case 'm':
		case 'M':
			if (size > (LONG_MAX / MB))
				throw std::runtime_error("Invalid max_body_size value: " +
										 tks[1] + RED " : overflow" NC);
			_clientMaxBodySize = (size * MB);
			break;
		case 'g':
		case 'G':
			if (size > (LONG_MAX / GB))
				throw std::runtime_error("Invalid max_body_size value: " +
										 tks[1] + RED " : overflow" NC);
			_clientMaxBodySize = (size * GB);
			break;
		default:
			throw std::runtime_error("Invalid max_body_size directive: " + tks[1]);
		}
	}
	Logger::debug("Location", __func__, "Processed directive: " YEL + tks[0] + " " + tks[1]);
}

/// @brief Set the ErrorPage value
/// @param tks The tokens of the error_page directive
/// @throw std::runtime_error if the error_page directive is invalid
void Location::setErrorPage(std::vector<std::string> &tks)
{
	if (tks.size() < 3)
		throw std::runtime_error("Invalid error_page directive");

	std::string page = tks.back();
	for (size_t i = 1; (i < (tks.size() - 1)); ++i)
	{
		char *end;
		long code = std::strtol(tks[i].c_str(), &end, 10);
		if ((*end != '\0') || (code < 300) || (code > 599) ||
			code != static_cast<short>(code))
			throw std::runtime_error("Invalid status code in error_page "
									 "directive");
		_errorPage[static_cast<short>(code)] = page;
	}
}

/// @brief Set the UploadStore value
/// @param tks The tokens of the upload_store directive
void Location::setUploadStore(std::vector<std::string> &tks)
{
	Logger::debug("Location", __func__, "Processing directive: " YEL + tks[0] + NC);

	if (tks.size() != 2)
		throw std::runtime_error("Invalid upload_store directive");
	if (!_uploadStore.empty())
		throw std::runtime_error("Upload_store already set");
	_uploadStore = tks[1];

	Logger::debug("Location", __func__, "Processed directive: " YEL + _uploadStore);
}

/// @brief Set the Return value
/// @param tks The tokens of the return directive
void Location::setReturn(std::vector<std::string> &tks)
{
	Logger::debug("Location", __func__, "Processing directive: " YEL + tks[0] + NC);

	if (tks.size() != 3)
		throw std::runtime_error("Invalid return directive");
	if (!_return.second.empty())
		throw std::runtime_error("Return already set");
	// Protect against overflow & conforming to Nginx values
	char *end = NULL;
	long errCode = std::strtol(tks[1].c_str(), &end, 10);
	if ((*end != '\0') || (errCode < 0) || (errCode > 999) ||
		errCode != static_cast<short>(errCode))
		throw std::runtime_error("Invalid return directive");

	_return.first = static_cast<short>(errCode);
	_return.second = tks[2];

	Logger::debug("Location", __func__, "Processed directive: " YEL + tks[1]);
}

/// @brief Set the CgiExt value
/// @param tks The tokens of the cgi_ext directive
void Location::setCgiExt(std::vector<std::string> &tks)
{
	if (tks.size() != 2)
		throw std::runtime_error("Invalid cgi_ext directive");
	if (!_cgiExt.empty())
		throw std::runtime_error("Cgi_ext already set");
	_cgiExt = tks[1];
}
