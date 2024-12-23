/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 11:33:13 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 15:57:28 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

Server::Server(void) {
	// NginX Default server
	// TODO: Push back index.html/index.htm to _index vewctor
	// TODO: Initialize Directives Map
	// TODO: Init Set of HTTP request methods (Declare it in class?)
	// TODO: Insert methods into _validMethods vector
	// TODO: set _return to Pair(-1, "") (empty)
}

Server::Server(const Server &copy)
	: methods(copy.methods)
// TODO: Use initialization list to get values to copy
{
}

Server::~Server(void) {
}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

Server &Server::operator=(const Server &copy) {
	// TODO: Init same vars as Copy Constructor
	(void)copy;
	return (*this);
}

/* ************************************************************************** */
/*                                Server Setup                                */
/* ************************************************************************** */

/// @brief Sets up the server by setting up the listening sockets.
/// @throws std::runtime_error if the socket setup fails.
/// @returns void
void Server::setupSockets(void) {
	const std::vector<Socket> netAddr = this->getNetAddr();

	// Setup sockets
	for (std::vector<Socket>::const_iterator sock = netAddr.begin();
		 sock != netAddr.end();
		 sock++) {
		// Create IPv4/TCP socket
		int sockFd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockFd < 0)
			throw std::runtime_error("socket: Failed to create socket");

		// Set socket options (for debugging)
		int optval = 1;
		if (setsockopt(
				sockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
			throw std::runtime_error("setsockopt: Failed to set SO_REUSEADDR");

		// Setup listening for conns on port ()
		struct sockaddr_in sockAddr;
		std::memset(&sockAddr, 0, sizeof(sockAddr)); // Clear sockAddr
		sockAddr.sin_family = AF_INET; // Set family to AF_INET (IPv4)
		this->setIPaddr(sock->ip, sockAddr);
		// convert port number to network byte order
		sockAddr.sin_port = htons(std::atoi(sock->port.c_str()));

		// Bind socket to port
		if (bind(sockFd, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0) {
			close(sockFd);
			throw std::runtime_error("bind: Failed to bind socket");
		}
		// Listen for conns
		if (listen(sockFd, SOMAXCONN) < 0) {
			close(sockFd);
			throw std::runtime_error("listen: Failed to listen for conns");
		}
		// Store listening socket for debugging & data management
		this->_listeningSockets.push_back(sockFd);
		this->_sockAddrVec.push_back(sockAddr);
	}
}

/* ************************************************************************** */
/*                             Directive Handlers                             */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

/// @brief Returns the network addresses of the server.
/// @return A vector of Socket objects representing the network addresses of the server.
std::vector<Socket> Server::getNetAddr(void) const {
	return (this->_netAddr);
};

/* ************************************************************************** */
/*                                  Setters                                   */
/* ************************************************************************** */

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
