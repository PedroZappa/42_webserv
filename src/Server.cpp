/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 11:33:13 by passunca          #+#    #+#             */
/*   Updated: 2024/12/26 12:09:03 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/ConfParser.hpp"
#include "../inc/Logger.hpp"
#include "../inc/Location.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

Server::Server(void) : _clientMaxBodySize(-1), _autoIndex(FALSE)
{
	// Push back index.html/index.htm to _serverIdx vector (NginX Defaults)
	_serverIdx.push_back("index.html");
	_serverIdx.push_back("index.htm");
	this->initDirectiveMap();
	std::set<Method> methods;
	methods.insert(GET);
	methods.insert(POST);
	methods.insert(DELETE);
	this->_validMethods = methods;
	this->_return = std::make_pair(-1, "");
}

Server::Server(const Server &copy)
	: _netAddr(copy.getNetAddr()), _serverName(copy.getServerName()),
	  _clientMaxBodySize(copy.getClientMaxBodySize()),
	  _errorPage(copy.getErrorPage()), _root(copy.getRoot()),
	  _locations(copy.getLocations()), _autoIndex(copy.getAutoIdx()),
	  _return(copy.getReturn()), _cgiExt(copy.getCgiExt()) {
}

Server::~Server(void) { }

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

Server &Server::operator=(const Server &copy)
{
	this->_netAddr = copy.getNetAddr();
	this->_serverName = copy.getServerName();
	this->_clientMaxBodySize = copy.getClientMaxBodySize();
	this->_errorPage = copy.getErrorPage();
	this->_root = copy.getRoot();
	this->_locations = copy.getLocations();
	this->_serverIdx = copy.getServerIdx();
	this->_autoIndex = copy.getAutoIdx();
	this->_return = copy.getReturn();
	this->_cgiExt = copy.getCgiExt();
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Server &ctx)
{
	os << BRED "Server Configuration:" NC << std::endl;
	os << BYEL "Network Addresses:" NC << std::endl;
	std::vector<Socket> netAddrs = ctx.getNetAddr();
	std::vector<Socket>::const_iterator sockit;
	for (sockit = netAddrs.begin(); sockit != netAddrs.end(); sockit++)
		os << sockit->ip << ":" << sockit->port << std::endl;

	os << BYEL "Server Name:" NC << std::endl;
	std::vector<std::string> names = ctx.getServerName();
	std::vector<std::string>::const_iterator strit;
	for (strit = names.begin(); strit != names.end(); strit++)
		os << *strit << std::endl;

	os << BYEL "Client Max Body Size:\n" NC << ctx.getClientMaxBodySize()
	   << std::endl;

	os << BYEL "Error Pages:\n" NC;
	std::map<short, std::string> errPages = ctx.getErrorPage();
	std::map<short, std::string>::const_iterator errit;
	for (errit = errPages.begin(); errit != errPages.end(); errit++)
		os << errit->first << ": " << errit->second << std::endl;

	os << BYEL "Root:\n" NC << ctx.getRoot() << std::endl;

	os << BYEL "Server Locations:\n" NC;
	std::map<std::string, Location> loci = ctx.getLocations();
	std::map<std::string, Location>::const_iterator locit;
	for (locit = loci.begin(); locit != loci.end(); locit++)
		os << locit->first << std::endl;

	os << BYEL "Return:\n" NC;
	std::pair<short, std::string> ret = ctx.getReturn();
	if (ret.first != -1)
		os << ret.first << ": " << ret.second << std::endl;

	return (os);
}

/* ************************************************************************** */
/*                                Server Setup                                */
/* ************************************************************************** */

/// @brief Initializes the directive setters map.
void Server::initDirectiveMap(void)
{
	_directiveMap["listen"] = &Server::setListen;
	_directiveMap["server_name"] = &Server::setServerName;
	_directiveMap["client_max_body_size"] = &Server::setClientMaxBodySize;
	_directiveMap["error_page"] = &Server::setErrorPage;
	_directiveMap["root"] = &Server::setRoot;
	_directiveMap["index"] = &Server::setIndex;
	// _directiveMap["autoindex"] = &Server::setAutoIdx;
	_directiveMap["return"] = &Server::setReturn;
	_directiveMap["cgi_ext"] = &Server::setCgiExt;
}

/// @brief Checks if the IP address is valid.
/// @param ip The ip to check.
/// @return True if the IP address is valid, false otherwise.
bool Server::isIpValid(const std::string &ip) const
{
	if (ip.empty() || ip == "0.0.0.0" || ip == "localhost")
		return (true);
	std::istringstream iss(ip);
	std::string seg;
	int nPeriods = 0;

	while (std::getline(iss, seg, '.')) {
		if (seg.empty() || (seg.size() > 3))
			return (false);
		std::string::size_type i;
		for (i = 0; i < seg.size(); i++)
			if (!std::isdigit(seg[i])) // Check if the string contains only digits
				return (false);
		int n = std::atoi(seg.c_str());
		if ((n < 0) || (n > 255)) // Check if the number is in the range 0-255
			return (false);
		++nPeriods;
	}
	return (nPeriods == 4);
}

/// @brief Checks if the port number is valid.
/// @param port The port number to check.
/// @return True if the port number is valid, false otherwise.
bool Server::isPortValid(const std::string &port) const
{
	std::string::size_type i;
	for (i = 0; i < port.size(); ++i)
		if (!std::isdigit(port[i])) // Check if the string contains only digits
			return (false);
	int portN = std::atoi(port.c_str());
	return ((portN >= 0) && (portN <= MAX_PORTS));
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

/// @brief Returns the network addresses of the server.
/// @return A vector of Socket objects representing the network addresses of the server.
std::vector<Socket> Server::getNetAddr(void) const
{
	return (this->_netAddr);
};

/// @brief Returns the server names.
/// @return A vector of strings representing the server names.
std::vector<std::string> Server::getServerName(void) const
{
	return (this->_serverName);
}

/// @brief Returns the maximum body size.
/// @return The maximum body size.
long Server::getClientMaxBodySize(void) const
{
	return (this->_clientMaxBodySize);
}

/// @brief Returns the maximum body size.
/// @param route The route to append to the maximum body size
/// @return The maximum body size.
long Server::getClientMaxBodySize(const std::string &route) const
{
	if (route.empty())
		return (this->_clientMaxBodySize);
	std::map<std::string, Location>::const_iterator it;
	it = _locations.find(route);
	if ((it == _locations.end()) || (it->second.getClientMaxBodySize() == -1))
		return (this->_clientMaxBodySize);
	else
		return (it->second.getClientMaxBodySize());
}

/// @brief Returns the error page.
/// @return The error page.
std::map<short, std::string> Server::getErrorPage(void) const
{
	return (this->_errorPage);
}

/// @brief Returns the error page.
/// @param route The route to append to the error page
/// @return The error page.
std::map<short, std::string> Server::getErrorPage(const std::string &route) const
{
	if (route.empty())
		return (this->_errorPage);
	std::map<std::string, Location>::const_iterator it;
	it = _locations.find(route);
	if ((it == _locations.end()) || (it->second.getErrorPage().empty()))
		return (this->_errorPage);
	else
		return (it->second.getErrorPage());
}

/// @brief Returns the root of the server.
/// @return The root of the server.
std::string Server::getRoot(void) const
{
	return (this->_root);
}

/// @brief Returns the root of the server.
/// @param route The route to append to the root
/// @return The root of the server.
std::string Server::getRoot(const std::string &route) const
{
	if (route.empty())
		return (this->_root);
	std::map<std::string, Location>::const_iterator it;
	it = _locations.find(route);
	if ((it == _locations.end()) || (it->second.getRoot().empty()))
		return (this->_root); // Return the root of the server
	else
		return (it->second.getRoot()); // Return the root of location
}

/// @brief Returns the locations.
/// @return The locations.
std::map<std::string, Location> Server::getLocations(void) const
{
	return (this->_locations);
}

/// @brief Returns the server indexes.
/// @return A vector of strings representing the server indexes.
std::vector<std::string> Server::getServerIdx(void) const
{
	return (this->_serverIdx);
}

/// @brief Returns the auto index.
/// @return The auto index.
State Server::getAutoIdx(void) const
{
	return (this->_autoIndex);
}

/// @brief Returns the upload store.
/// @param route The route to append to the upload store
/// @return The upload store.
std::string Server::getUploadStore(std::string &route) const
{
	_DEBUG(FSTART, "route: " + route);

	if (route.empty())
		return (this->_uploadStore);
	std::map<std::string, Location>::const_iterator it;
	it = _locations.find(route);
	if ((it == _locations.end()) || (it->second.getUploadStore().empty()))
		return (this->_uploadStore);
	else
		return (it->second.getUploadStore());

	_DEBUG(FEND, "upload store: " + this->_uploadStore);
}

/// @brief Returns the upload store.
/// @return The upload store.
std::string Server::getUploadStore(void) const
{
	return (this->_uploadStore);
}

/// @brief Returns the return.
/// @param route The route to get
/// @return The return.
std::pair<short, std::string> Server::getReturn(const std::string &route) const
{
	if (route.empty())
		return (_return);
	std::map<std::string, Location>::const_iterator it;
	it = _locations.find(route);
	if ((it == _locations.end()) || (it->second.getReturn().first == -1))
		return (_return);
	else
		return (it->second.getReturn());
}

/// @brief Returns the return.
/// @return The return.
std::pair<short, std::string> Server::getReturn(void) const
{
	return (_return);
}

/// @brief Returns the cgi extension.
/// @param route The route to get
/// @return The cgi extension.
std::string Server::getCgiExt(const std::string &route) const
{
	if (route.empty())
		return (_cgiExt);
	std::map<std::string, Location>::const_iterator it;
	it = _locations.find(route);
	if ((it == _locations.end()) || (it->second.getCgiExt().empty()))
		return (_cgiExt);
	else
		return (it->second.getCgiExt());
}

/// @brief Returns the cgi extension.
/// @return The cgi extension.
std::string Server::getCgiExt(void) const
{
	return (_cgiExt);
}

/* ************************************************************************** */
/*                                  Setters                                   */
/* ************************************************************************** */

/// @brief Sets a directive.
/// @param directive The directive to set.
/// @throw std::runtime_error if the directive is invalid.
void Server::setDirective(std::string &directive)
{
	_DEBUG(FSTART, "directive: " GRN + directive);
	std::vector<std::string> tks;
	tks = ConfParser::tokenizer(directive); // Tokenize
	if (tks.size() < 2)
		throw std::runtime_error("Directive " + directive + " is invalid");
	showContainer(__func__, "Directive Tokens", tks);

	std::map<std::string, DirHandler>::const_iterator it;
	// iterate through tokens to find a valid directive
	for (it = _directiveMap.begin(); it != _directiveMap.end(); it++) {
		for (size_t tk = 0; tk < tks.size(); tk++) {
			if (tks[tk] == it->first) {
				(this->*(it->second))(tks);
				_DEBUG(FEND, "set directive: " BWHT + directive + NC);
				return;
			}
		}
	}
}

/// @brief Sets the listen directive
/// @param tks The tokens of the listen directive
/// @throw std::runtime_error if the listen directive is invalid
void Server::setListen(std::vector<std::string> &tks)
{
	_DEBUG(FSTART, "processing listen directive: " YEL + tks[0] + NC);
	if (tks.size() > 2)
		throw std::runtime_error("Invalid listen directive: directive must "
								 "include at least one IP/port");

	// Process tokens from the second one onwards
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin() + 1; it != tks.end(); ++it)
	{
		std::string val = *it;
		Socket socket;

		size_t sep = val.find(':');     // Check for 'IP:Port' format
		if (sep != std::string::npos) { // If ':' is present
			socket.ip = val.substr(0, sep);
			socket.port = val.substr(sep + 1);

			if (socket.ip.empty() || socket.port.empty())
				throw std::runtime_error("Invalid listen directive '" + val + "'");
		} else { // No ':' present; it could be just an IP or port
			if (val.find_first_not_of("0123456789") == std::string::npos)
				socket.port = val; // All numeric: assume it's a port
			else
				socket.ip = val; // Contains non-numeric: assume it's an IP
		}

		// Apply defaults
		if (socket.ip.empty() || socket.ip == "listen")
			socket.ip = "0.0.0.0";
		if (socket.port.empty())
			socket.port = "80";
		// Validate IP and Port
		if (!isIpValid(socket.ip))
			throw std::runtime_error("Invalid listen directive: invalid IP '" +
									 socket.ip + "'");
		if (!isPortValid(socket.port))
			throw std::runtime_error("Invalid listen directive: invalid port "
									 "'" +
									 socket.port + "'");
		// Add the socket to the list
		_netAddr.push_back(socket);
	}

	_DEBUG(FEND, "processed listen directive: " YEL + tks[0] + NC);
}

/// @brief Sets the server name.
/// @param name The name of the server.
void Server::setServerName(std::vector<std::string> &tks)
{
	_DEBUG(FSTART, "processing directive: " YEL + tks[0] + NC);
	tks.erase(tks.begin()); // Remove 'server_name'
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin(); it != tks.end(); it++)
		_serverName.push_back(*it);

	_DEBUG(FEND, "processed directive: " YEL + _serverName[0] + NC);
}

/// @brief Sets the max_body_size directive
/// @param tks Vector of tokens for the max_body_size directive
/// @throw std::runtime_error if the max_body_size directive is invalid
void Server::setClientMaxBodySize(std::vector<std::string> &tks)
{
	_DEBUG(FSTART, "processing directive: " YEL + tks[0] + NC);
	if (tks.size() != 2) // Check number of tokens
		throw std::runtime_error("Invalid max_body_size directive: " + tks[0]);
	if (_clientMaxBodySize != -1) // Check if already set
		throw std::runtime_error("Max body size already set");

	std::string maxSize = tks[1];
	char unit = maxSize[maxSize.size() - 1]; // Extract last character
	if (!std::isdigit(unit))
		maxSize.resize(maxSize.size() - 1); // Remove last character

	// convert string to long
	char *endPtr = NULL;
	long size = std::strtol(maxSize.c_str(), &endPtr, 10);
	if ((*endPtr != '\0') | (size < 0))
		throw std::runtime_error("Invalid max_body_size directive: " + tks[1]);

	// Applying unit checking for overflow
	_clientMaxBodySize = size;
	if (!std::isdigit(unit)) {
		const long maxLim = LONG_MAX;
		switch (unit) {
		case 'k':
		case 'K':
			if (size > (maxLim / KB))
				throw std::runtime_error("client_max_body_size overflows");
			_clientMaxBodySize = (size * KB);
			break;
		case 'm':
		case 'M':
			if (size > (maxLim / MB))
				throw std::runtime_error("client_max_body_size overflows");
			_clientMaxBodySize = (size * MB);
			break;
		case 'g':
		case 'G':
			if (size > (maxLim / GB))
				throw std::runtime_error("client_max_body_size overflows");
			_clientMaxBodySize = (size * GB);
			break;
		default:
			throw std::runtime_error("Invalid unit for max_body_size: " + tks[1]);
		}
	}

	// std::stringstream ss;
	// ss << _clientMaxBodySize;
	// std::string cliMaxBodyStr = ss.str();
	_DEBUG(FEND, "processed directive: " YEL + tks[0] + NC);
}

/// @brief Sets the error page directive
/// @param tks Vector of tokens for the error_page directive
/// @throw std::runtime_error if the error_page directive is invalid
void Server::setErrorPage(std::vector<std::string> &tks) {
	_DEBUG(FSTART, "processed directive: " YEL + tks[0] + NC);

	if (tks.size() < 3)
		throw std::runtime_error("Invalid error_page directive: " + tks[0]);

	std::string page = tks.back();
	for (size_t i = 1; (i < (tks.size() - 1)); i++) {
		char *end;
		long code = std::strtol(tks[i].c_str(), &end, 10);
		if ((*end != '\0') || (code < 300) || (code > 599) ||
			code != static_cast<short>(code))
			throw std::runtime_error("Invalid error_page directive: " + tks[i]);
		_errorPage[static_cast<short>(code)] = page;
	}

#ifdef DEBUG
	std::stringstream ss;
	std::map<short, std::string>::const_iterator it;
	for (it = _errorPage.begin(); it != _errorPage.end(); ++it) {
		if (it != _errorPage.begin())
			ss << ", ";
		ss << it->first << ": " << it->second;
	}
	std::string errorPageStr = ss.str();
	_DEBUG(FEND, "processed directive: " YEL + errorPageStr + NC);
#endif
}

/// @brief Sets the root of the server.
/// @param root The root to set.
/// @throw std::runtime_error if the root is invalid.
void Server::setRoot(std::vector<std::string> &root) {
	_DEBUG(FSTART, "processing root directive: " YEL + root[0] + NC);
	if (!_root.empty())
		throw std::runtime_error("Root already set");
	if (root.size() > 2)
		throw std::runtime_error("Invalid root directive: directive must "
								 "include only one root");
	_root = root[1];
	_DEBUG(FEND, "processed root directive: " YEL + _root + NC);
}

/// @brief Sets the location block.
/// @param block The location block to set.
/// @param start The start of the location block.
/// @param end The end of the location block.
/// @throw std::runtime_error if the location block is invalid.
void Server::setLocation(std::string block, size_t start, size_t end) {
#ifdef DEBUG
	_DEBUG(FSTART,
		   "processing location block: " YEL +
			   block.substr(start, (end - start)) + NC);
#endif

	std::istringstream location(block.substr(start, (end - start)));
	std::vector<std::string> tokens;
	std::string route;
	std::string line;
	std::string discard;
	Location locInfo;

	location >> route; // Skip 'location'
	location >> route; // Get route
	locInfo.setRoot(route);
	if (route == "{")
		throw std::runtime_error("Invalid location route");
	location >> line; // Skip opening "{"
	if (line != "{")
		throw std::runtime_error("Only one route per location block "
								 "supported.");
	while (std::getline(location, line, ';')) {
		ConfParser::removeSpaces(line);
		if (!location.eof() && line.empty())
			throw std::runtime_error("Invalid location block: unparsable");
		if (line.empty())
			continue;

#ifdef DEBUG
		Logger::debug("about to set directive: \n" + line);
#endif

		locInfo.setDirective(line);
		// if (discard != "root" && discard != "autoindex" && discard != "index")
		// 	throw std::runtime_error("Invalid location block: unknown directive");
		if (location.tellg() >= static_cast<std::streampos>(end))
			break;
	}
	locInfo.getClientMaxBodySize();
	_locations[route] = locInfo;

#ifdef DEBUG
	Logger::debug("processed location block:\n" + _locations[route]);
#endif
}

/// @brief Sets the index of the server.
/// @param tks The index to set.
/// @throw std::runtime_error if the index is invalid.
void Server::setIndex(std::vector<std::string> &tks) {
#ifdef DEBUG
	_DEBUG(FSTART, "processing directive: " YEL + tks[0] + NC);
#endif
	tks.erase(tks.begin()); // Remove 'index'
	std::vector<std::string>::const_iterator it;

	for (it = tks.begin(); it != tks.end(); it++)
		_serverIdx.push_back(*it);
#ifdef DEBUG
	_DEBUG(FEND, "processed directive: " YEL + *it + NC);
#endif
}

/// @brief Sets the autoindex of the server.
/// @param tks The autoindex to set.
/// @throw std::runtime_error if the autoindex is invalid.
void Server::setAutoIndex(std::vector<std::string> &tks) {
#ifdef DEBUG
	_DEBUG(FSTART, "processing directive: " YEL + tks[0] + NC);
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
	_DEBUG(FEND, "processed directive: " YEL + _autoIndex);
#endif
}

/// @brief Sets the upload_store of the server.
/// @param tks The upload_store to set.
/// @throw std::runtime_error if the upload_store is invalid.
void Server::setUploadStore(std::vector<std::string> &tks) {
#ifdef DEBUG
	_DEBUG(FSTART, "processing directive: " YEL + tks[0] + NC);
#endif

	if (tks.size() != 2)
		throw std::runtime_error("Invalid upload_store directive");
	if (!_uploadStore.empty())
		throw std::runtime_error("Upload_store already set");
	_uploadStore = tks[1];

#ifdef DEBUG
	_DEBUG(FEND, "processed directive: " YEL + _uploadStore);
#endif
}

/// @brief Sets the return of the server.
/// @param tks The return to set.
/// @throw std::runtime_error if the return is invalid.
void Server::setReturn(std::vector<std::string> &tks) {
#ifdef DEBUG
	_DEBUG(FSTART, "processing directive: " YEL + tks[0] + NC);
#endif

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

#ifdef DEBUG
	_DEBUG(FEND, "processed directive: " YEL + _return.second);
#endif
}

/// @brief Sets the cgi_ext of the server.
/// @param tks The cgi_ext to set.
/// @throw std::runtime_error if the cgi_ext is invalid.
void Server::setCgiExt(std::vector<std::string> &tks) {
	if (tks.size() != 2)
		throw std::runtime_error("Invalid cgi_ext directive");
	if (!_cgiExt.empty())
		throw std::runtime_error("Cgi_ext already set");
	this->_cgiExt = tks[1];
}

/// @brief Sets the IP address for the server.
/// @param ip The IP address to set.
/// @param sockaadr The sockaddr_in object to set the IP address for.
/// @throws std::runtime_error if the IP address is invalid.
void Server::setIPaddr(const std::string &ip, struct sockaddr_in &sockaadr) const {
	if (ip.empty())
		sockaadr.sin_addr.s_addr = INADDR_ANY; //
	else if (ip == "localhost") {
		if (inet_aton("127.0.0.1", &sockaadr.sin_addr) == 0)
			throw std::runtime_error("inet_addr: Invalid IP address");
	} else if (inet_aton(ip.c_str(), &sockaadr.sin_addr) == 0)
		throw std::runtime_error("inet_addr: Invalid IP address");
}
