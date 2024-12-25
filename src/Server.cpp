/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 11:33:13 by passunca          #+#    #+#             */
/*   Updated: 2024/12/25 20:05:38 by passunca         ###   ########.fr       */
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
	  _serverIdx(copy.getServerIdx()) {
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
	os << "Network Addresses:" << std::endl;
	std::vector<Socket> netAddrs = ctx.getNetAddr();
	std::vector<Socket>::const_iterator it;
	for (it = netAddrs.begin(); it != netAddrs.end(); it++)
		os << "=> IP: " << it->ip << " Port: " << it->port << std::endl;

	os << "Server Name:" << std::endl;
	std::vector<std::string> names = ctx.getServerName();
	std::vector<std::string>::const_iterator it2;
	for (it2 = names.begin(); it2 != names.end(); it2++)
		os << "=> " << *it2 << std::endl;
	return (os);
}

/* ************************************************************************** */
/*                                Server Setup                                */
/* ************************************************************************** */

/// @brief Initializes the directive map.
void Server::initDirectiveMap(void) {
	_directiveMap["listen"] = &Server::setListen;
	_directiveMap["server_name"] = &Server::setServerName;
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

/// @brief Returns the server names.
/// @return A vector of strings representing the server names.
std::vector<std::string> Server::getServerName(void) const {
	return (this->_serverName);
}

/// @brief Returns the server indexes.
/// @return A vector of strings representing the server indexes.
std::vector<std::string> Server::getServerIdx(void) const {
	return (this->_serverIdx);
}

/// @brief Returns the network addresses of the server.
/// @return A vector of Socket objects representing the network addresses of the server.
std::vector<Socket> Server::getNetAddr(void) const {
	return (this->_netAddr);
};

/* ************************************************************************** */
/*                                  Setters                                   */
/* ************************************************************************** */

/// @brief Sets the listen directive
/// @param tks The tokens of the listen directive
/// @throw std::runtime_error if the listen directive is invalid
void Server::setListen(std::vector<std::string> &tks) {
#ifdef DEBUG
	debugLocus(__func__, FSTART, "tokenizing line: " YEL + tks[0] + NC);
#endif
	if (tks.size() > 2)
		throw std::runtime_error("Invalid listen directive");
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin(); it != tks.end(); it++) {
		Socket socket;
		std::string val = (*it);
		size_t sep = val.find(';');
		if (sep != std::string::npos) {
			socket.ip = val.substr(0, sep);
			socket.port = val.substr(sep + 1);
			if (socket.ip.empty() || socket.port.empty())
				throw std::runtime_error("Invalid listen directive");
		} else {
			if ((*it).find_first_not_of("0123456789") == std::string::npos)
				socket.port = val;
			else
				socket.ip = val;
		}
		if (socket.ip.empty())
			socket.ip = "0.0.0.0";
		if (socket.port.empty())
			socket.ip = "80";
		if (!isIpValid(socket.ip) || !isPortValid(socket.port))
			throw std::runtime_error("Invalid listen directive");
		_netAddr.push_back(socket);
	}
#ifdef DEBUG
	debugLocus(__func__, FEND, "after tokenizing line: " YEL + tks[0] + NC);
#endif
}

/// @brief Sets the server name.
/// @param name The name of the server.
void Server::setServerName(std::vector<std::string> &tks) {
	_serverName.clear();
	std::vector<std::string>::const_iterator it;
	for (it = tks.begin(); it != tks.end(); it++)
		_serverName.push_back(*it);
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
