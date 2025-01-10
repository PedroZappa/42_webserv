/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:12:24 by passunca          #+#    #+#             */
/*   Updated: 2025/01/10 12:26:27 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Server.hpp"

class Cluster {
  public:
	// Constructor & Destructor
	Cluster(const std::vector<Server> &servers); // Builds the cluster context of servers
	~Cluster();                                  // Closes epoll fd

	// Operators
	const Server &operator[](size_t idx) const;

	// Setup
	void setupCluster(void); // Sets up the cluster listening sockets
	void run(void);          // Runs the cluster listening loop

	// Getters
	const std::vector<const Server *> &getServers() const;
	const std::vector<int> &getListeningSockets() const;
	int getEpollFd(void) const;

  private:
	std::vector<Server *> _servers;
	// std::vector<VirtualServer> _virtualServers;
	std::vector<Socket> _listenAddr;
	int _epollFd;
	std::map<int, std::string> _requestBuff;

	// Private Methods

	// Unnused Constructors
	Cluster(void);
	Cluster(const Cluster &src);
	const Cluster &operator=(const Cluster &src);
};

#endif
