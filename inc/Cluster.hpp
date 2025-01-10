/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:12:24 by passunca          #+#    #+#             */
/*   Updated: 2025/01/10 17:25:43 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Server.hpp"

class Server;

class Cluster {
  public:
	// Virtual Server Context
	struct VirtualServer {
		const Server *server;
		std::string name;
		std::string ip;
		std::string port;

		bool operator<(const VirtualServer &rhs) const { // For std:set
			if (this->ip != rhs.ip)
				return (this->ip < rhs.ip);
			if (this->port != rhs.port)
				return (this->port < rhs.port);
			return (this->port < rhs.port);
		}
	};

	// Constructor & Destructor
	Cluster(const std::vector<Server> &servers); // Builds the cluster context of servers
	~Cluster();                                  // Closes epoll fd

	// Operators
	const Server &operator[](size_t idx) const;

	// Setup
	bool hasDuplicates(void) const;
	void setup(void); // Sets up the cluster listening sockets
	void run(void);   // Runs the cluster listening loop

	// Getters
	std::vector<VirtualServer> getVirtualServers(void) const;
	const std::vector<const Server *> &getServers() const;
	const std::vector<int> &getListeningSockets() const;
	int getEpollFd(void) const;

  private:
	std::vector<const Server *> _servers;
	std::vector<VirtualServer> _virtualServers;
	std::vector<int> _listenSockets;
	int _epollFd;
	std::map<int, std::string> _requestBuff;

	// Private Methods
	// setupCluster()
	void setEpollFd(void);
	std::set<Socket> getVirtualServerSockets(void);
	int setSocket(const std::string &ip, const std::string &port);
	void startListen(int socket);
	void setEpollSocket(int socket);

	// run()

	// Unnused Constructors & Operators
	Cluster(void);
	Cluster(const Cluster &src);
	const Cluster &operator=(const Cluster &src);
};

#endif
