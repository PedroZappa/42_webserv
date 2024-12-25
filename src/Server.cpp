/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 11:33:13 by passunca          #+#    #+#             */
/*   Updated: 2024/12/25 21:23:58 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/ConfParser.hpp"
#include "../inc/Debug.hpp"
#include "../inc/Error.h"
#include "../inc/Location.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

Server::Server(void) {
	// Push back index.html/index.htm to _serverIdx vector (NginX Defaults)
	_serverIdx.push_back("index.html");
	_serverIdx.push_back("index.htm");
	this->initDirectiveMap();
	// TODO: Init Set of HTTP request methods (Declare it in class?)
	std::set<Method> methods;
	methods.insert(GET);
	methods.insert(POST);
	methods.insert(DELETE);
	this->_validMethods = methods;
	// TODO: set _return to Pair(-1, "") (empty)
}

Server::Server(const Server &copy)
	: _netAddr(copy.getNetAddr()), _serverName(copy.getServerName()),
	  _serverIdx(copy.getServerIdx()), _root(copy.getRoot()) {
}

Server::~Server(void) {
}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

Server &Server::operator=(const Server &copy) {
	this->_netAddr = copy.getNetAddr();
	this->_serverName = copy.getServerName();
	this->_serverIdx = copy.getServerIdx();
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Server &ctx) {
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

	os << BYEL "Root:\n" NC << ctx.getRoot() << std::endl;

	return (os);
}

/* ************************************************************************** */
/*                                Server Setup                                */
/* ************************************************************************** */

/// @brief Initializes the directive setters map.
void Server::initDirectiveMap(void) {
	_directiveMap["listen"] = &Server::setListen;
	_directiveMap["server_name"] = &Server::setServerName;
	_directiveMap["root"] = &Server::setRoot;
}

/// @brief Checks if the IP address is valid.
/// @param ip The ip to check.
/// @return True if the IP address is valid, false otherwise.
bool Server::isIpValid(const std::string &ip) const {
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
bool Server::isPortValid(const std::string &port) const {
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
std::vector<Socket> Server::getNetAddr(void) const {
	return (this->_netAddr);
};

/// @brief Returns the server names.
/// @return A vector of strings representing the server names.
std::vector<std::string> Server::getServerName(void) const {
	return (this->_serverName);
}

/// @brief Returns the root of the server.
/// @return The root of the server.
std::string Server::getRoot(void) const {
	return (this->_root);
}

/// @brief Returns the root of the server.
/// @param route The route to append to the root
/// @return The root of the server.
std::string Server::getRoot(const std::string &route) const {
	if (route.empty())
		return (this->_root);
	std::map<std::string, Location>::const_iterator it;
	it = _locations.find(route);
	if ((it == _locations.end()) || (it->second.getRoot().empty()))
		return (this->_root); // Return the root of the server
	else
		return (it->second.getRoot()); // Return the root of location
}

/// @brief Returns the server indexes.
/// @return A vector of strings representing the server indexes.
std::vector<std::string> Server::getServerIdx(void) const {
	return (this->_serverIdx);
}

/* ************************************************************************** */
/*                                  Setters                                   */
/* ************************************************************************** */

/// @brief Sets the listen directive
/// @param tks The tokens of the listen directive
/// @throw std::runtime_error if the listen directive is invalid
void Server::setListen(std::vector<std::string> &tks) {
#ifdef DEBUG
	debugLocus(
		__func__, FSTART, "processing listen directive: " YEL + tks[0] + NC);
#endif
	if (tks.size() < 2)
		throw std::runtime_error("Invalid listen directive: directive must "
								 "include at least one IP/port");

	// Process tokens from the second one onwards
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin() + 1; it != tks.end(); ++it) {
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
		if (socket.ip.empty())
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

#ifdef DEBUG
	debugLocus(__func__, FEND, "processed listen directive: " YEL + tks[0] + NC);
#endif
}

/// @brief Sets the server name.
/// @param name The name of the server.
void Server::setServerName(std::vector<std::string> &tks) {
#ifdef DEBUG
	debugLocus(__func__, FSTART, "processing directive: " YEL + tks[0] + NC);
#endif
	tks.erase(tks.begin()); // Remove 'server_name'
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin(); it != tks.end(); it++)
		_serverName.push_back(*it);

#ifdef DEBUG
	debugLocus(__func__, FEND, "processed directive: " YEL + _serverName[0] + NC);
#endif
}

void Server::setRoot(std::vector<std::string> &root) {
#ifdef DEBUG
	debugLocus(
		__func__, FSTART, "processing root directive: " YEL + root[0] + NC);
#endif
	if (!_root.empty())
		throw std::runtime_error("Root already set");
	if (root.size() > 2)
		throw std::runtime_error("Invalid root directive: directive must "
								 "include only one root");
	_root = root[1];
#ifdef DEBUG
	debugLocus(__func__, FEND, "processed root directive: " YEL + _root + NC);
#endif
}

/// @brief Sets the location block.
/// @param block The location block to set.
/// @param start The start of the location block.
/// @param end The end of the location block.
/// @throw std::runtime_error if the location block is invalid.
void Server::setLocation(std::string block, size_t start, size_t end) {
	std::istringstream location(block.substr(start, (end - start)));
	std::vector<std::string> tokens;
	std::string route;
	std::string line;
	std::string key;
	Location locInfo;

	location >> route; // Skip 'location'
	location >> route; // Get route
	if (route == "{")
		throw std::runtime_error("Invalid location route");
	location >> line; // Skip opening "{"
	if (line != "{")
		throw std::runtime_error("Invalid location block: no '{' at start");
	while (std::getline(location, line, ';')) {
		ConfParser::removeSpaces(line);
		if (line.empty() && !location.eof())
			throw std::runtime_error("Invalid location block: unparsable");
		if (line.empty())
			continue;
		std::istringstream lineRead(line);
		lineRead >> key;
		locInfo.setDirective(line);
		// if (key != "root" && key != "autoindex" && key != "index")
		// 	throw std::runtime_error("Invalid location block: unknown directive");
		if (location.tellg() >= static_cast<std::streampos>(end))
			break;
	}
	_locations[route] = locInfo;
}

/// @brief Sets a directive.
/// @param directive The directive to set.
/// @throw std::runtime_error if the directive is invalid.
void Server::setDirective(std::string &directive) {
#ifdef DEBUG
	debugLocus(__func__, FSTART, "directive: " GRN + directive);
#endif
	std::vector<std::string> tks;
	tks = ConfParser::tokenizer(directive); // Tokenize
	if (tks.size() < 2)
		throw std::runtime_error("Directive " + directive + " is invalid");

#ifdef DEBUG
	showContainer(__func__, "Directive Tokens", tks);
#endif

	std::map<std::string, DirHandler>::const_iterator it;
	// iterate through tokens to find a valid directive
	for (it = _directiveMap.begin(); it != _directiveMap.end(); it++) {
		for (size_t tk = 0; tk < tks.size(); tk++) {
			if (tks[tk] == it->first) {
				(this->*(it->second))(tks);
				return;
			}
		}
	}
#ifdef DEBUG
	debugLocus(__func__, FEND, "directive: " GRN + directive);
#endif
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
