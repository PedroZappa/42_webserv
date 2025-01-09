/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:45:52 by passunca          #+#    #+#             */
/*   Updated: 2024/12/26 11:36:10 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include "Webserv.hpp"

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
	bool isIpValid(const std::string &ip) const;
	bool isPortValid(const std::string &port) const;

	// Getters
	std::vector<Socket> getNetAddr(void) const;
	std::vector<std::string> getServerName(void) const;
	long getClientMaxBodySize(void) const;
	long getClientMaxBodySize(const std::string &route) const;
	std::map<short, std::string> getErrorPage(void) const;
	std::map<short, std::string> getErrorPage(const std::string &route) const;
	std::string getRoot(void) const;
	std::string getRoot(const std::string &route) const;
	std::map<std::string, Location> getLocations(void) const;
	std::vector<std::string> getServerIdx(void) const;
	State getAutoIdx(void) const;
	std::string getUploadStore(std::string &route) const;
	std::string getUploadStore(void) const;
	std::pair<short, std::string> getReturn(const std::string &route) const;
	std::pair<short, std::string> getReturn(void) const;
	std::string getCgiExt(const std::string &route) const;
	std::string getCgiExt(void) const;

	// Setters
	void setDirective(std::string &directive);
	void setListen(std::vector<std::string> &tks);
	void setServerName(std::vector<std::string> &tks);
	void setClientMaxBodySize(std::vector<std::string> &tks);
	void setErrorPage(std::vector<std::string> &tks);
	void setRoot(std::vector<std::string> &root);
	void setLocation(std::string block, size_t start, size_t end);
	void setIndex(std::vector<std::string> &tks);
	void setAutoIndex(std::vector<std::string> &tks);
	void setUploadStore(std::vector<std::string> &tks);
	void setReturn(std::vector<std::string> &tks);
	void setCgiExt(std::vector<std::string> &tks);
	void setIPaddr(const std::string &ip, struct sockaddr_in &sockaadr) const;

  private:
	// Server Context
	std::vector<Socket> _netAddr;
	std::vector<std::string> _serverName;
	long _clientMaxBodySize;
	std::map<short, std::string> _errorPage;
	std::string _root;
	std::map<std::string, Location> _locations;
	std::vector<std::string> _serverIdx;
	State _autoIndex;
	std::string _uploadStore;
	std::set<Method> _validMethods;
	std::pair<short, std::string> _return;
	std::string _cgiExt;

	// Directive Map w/ Function Pointer
	typedef void (Server::*DirHandler)(std::vector<std::string> &d);
	std::map<std::string, DirHandler> _directiveMap;
};

// Insertion Operator
std::ostream &operator<<(std::ostream &os, const Server &ctx);

#endif
