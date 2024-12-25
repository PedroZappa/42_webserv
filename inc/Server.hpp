/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:45:52 by passunca          #+#    #+#             */
/*   Updated: 2024/12/24 19:14:21 by passunca         ###   ########.fr       */
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
	void initDirectiveMap(void);

	// Getters
	std::vector<std::string> getServerName(void) const;
	std::vector<std::string> getServerIdx(void) const;
	std::vector<Socket> getNetAddr(void) const;

	// Setters
	void setServerName(std::vector<std::string> &tks);
	void setLocation(std::string block, size_t start, size_t end);
	void setDirective(std::string &directive);
	void setIPaddr(const std::string &ip, struct sockaddr_in &sockaadr) const;

  private:
	// Server Context
	std::vector<Socket> _netAddr;
	std::vector<std::string> _serverName;
	std::vector<std::string> _serverIdx;
	std::string _root; // Root Directive
	std::map<std::string, Location> _locations;
	std::set<Method> _validMethods;
	// TODO: Add other Context Data

	// Directive Map w/ Function Pointer
	typedef void (Server::*DirHandler)(std::vector<std::string> &d);
	std::map<std::string, DirHandler> _directiveMap;

	// Connection Data (for debugging goodness)
	std::vector<int> _listeningSockets;
	std::vector<struct sockaddr_in> _sockAddrVec;
};

// Insertion Operator
std::ostream &operator<<(std::ostream &os, const Server &ctx);

#endif
