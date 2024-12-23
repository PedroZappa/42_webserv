/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:45:52 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 18:16:35 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Location.hpp"

/// @brief Network Listening Endpoint
struct Socket {
	// Constructors
	Socket(void) {};
	Socket(const std::string &ip, const std::string &port)
		: ip(ip), port(port) {};

	// Operators
	bool operator<(const Socket &rhs) const {
		if (this->ip != rhs.ip)
			return (this->ip < rhs.ip);
		return (this->port < rhs.port);
	}
	bool operator==(const Socket &rhs) const {
		return ((this->ip == rhs.ip) && (this->port == rhs.port));
	}

	// Attributes
	std::string ip;
	std::string port;
};

class Server {
  public:
	// Constructors
	Server(void);
	Server(const Server &copy);
	~Server(void);

	// Operators
	Server &operator=(const Server &src);

	// Server Setup
	void setupSockets(void);

	// Directive Handlers

	// Getters
	std::vector<Socket> getNetAddr(void) const;

	// Setters
	void setLocation(std::string block, size_t start, size_t end);
	void setDirective(std::string &directive);
	void setIPaddr(const std::string &ip, struct sockaddr_in &sockaadr) const;

	// Public Data
	std::set<Method> methods;

  private:
	// Server Context
	std::vector<Socket> _netAddr;
	std::vector<std::string> _serverName;
	std::vector<std::string> _serverIdx;
	std::string _root; // Root Directive
	std::set<Method> _validMethods;
	std::map<std::string, Location> _locations;
	// TODO: Add other Context Data

	// Connection Data (for debugging goodness)
	std::vector<int> _listeningSockets;
	std::vector<struct sockaddr_in> _sockAddrVec;
};

// Insertion Operator
std::ostream &operator<<(std::ostream &os, const Server &ctx);

#endif
