/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 11:33:13 by passunca          #+#    #+#             */
/*   Updated: 2024/12/24 19:03:17 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/ConfParser.hpp"
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
	  _serverIdx(copy.getServerIdx())
{
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

///
void Server::initDirectiveMap(void) {
	_directiveMap["server_name"] = &Server::setServerName;
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

void Server::setDirective(std::string &directive) {
	std::vector<std::string> tks;
	tks = ConfParser::tokenizer(directive);
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
