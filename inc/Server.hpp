/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:45:52 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 11:11:09 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"

/// @brief Network Listening Endpoint
struct Listen {
	// Constructors
	Listen(void);
	Listen(const std::string &ip, const std::string &port)
		: ip(ip), port(port) {};
	~Listen(void);

	// Operators
	bool operator<(const Listen &rhs) const {
		if (this->ip != rhs.ip)
			return (this->ip < rhs.ip);
		return (this->port < rhs.port);
	}
	bool operator==(const Listen &rhs) const {
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
	void setupServer(void);

	// Directive Handlers
	
	// Getters
	
  private:
	// Server Context
	std::vector<Listen> _netAddr;
	std::vector<std::string> _serverName;
	std::vector<std::string> _serverIdx;
	std::string _root; // Root Directive
	// TODO: Add other Context Data

	// Connection Data
	std::vector<int> _listeningSockets;
	std::vector<struct sockaddr_in> _sockAddrVec;
};

// Insertion Operator
std::ostream &operator<<(std::ostream &os, const Server &ctx);

#endif
