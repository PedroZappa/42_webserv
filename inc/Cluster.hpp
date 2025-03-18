/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:12:24 by passunca          #+#    #+#             */
/*   Updated: 2025/03/09 19:36:35 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "AResponse.hpp"
#include "HttpParser.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include <sys/socket.h>

class Server;

/**
 * @class Cluster
 * @brief Manages a cluster of servers, handling socket setup and request processing.
 */
class Cluster {
  public:
	/**
	 * @struct VirtualServer
	 * @brief Represents a virtual server with its associated details.
	 */
	struct VirtualServer {
		const Server *server; /**< Pointer to the server instance. */
		std::string name;     /**< Name of the virtual server. */
		std::string ip;       /**< IP address of the virtual server. */
		std::string port;     /**< Port number of the virtual server. */

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
	void stop(void);

	// Getters
	std::vector<VirtualServer> getVirtualServers(void) const;
	const std::vector<const Server *> &getServers(void) const;
	const std::vector<int> &getListeningSockets(void) const;
	int getEpollFd(void) const;

  private:
	std::vector<const Server *> _servers;       /**< List of server pointers. */
	std::vector<VirtualServer> _virtualServers; /**< List of virtual servers. */
	std::vector<int> _listenSockets; /**< List of listening socket file descriptors. */
	int _epollFd;                    /**< Epoll file descriptor. */
	std::map<int, std::string> _requestBuff; /**< Buffer for incoming requests. */

	// Private Methods
	// setupCluster()
	void setEpollFd(void);
	std::set<Socket> getVirtualServerSockets(void);
	int setSocket(const std::string &ip, const std::string &port);
	void startListen(int socket);
	void setEpollSocket(int socket);

	// run()
	bool isSocketListening(int socket) const;
	void setupConnection(int socket);
	void setSocketToNonBlocking(int socket);
	void handleRequest(int socket);
	bool isRequestValid(const std::string &requestBuf) const;
	void processRequest(int socket, const std::string &requestBuf);
	const std::string getResponse(HttpRequest &,
								  unsigned short &errorStatus,
								  int socket);

	const Server *getContext(const HttpRequest &, int socket);
	const Socket getSocketAddress(int socket);
	const std::string getHostnameFromRequest(const HttpRequest &);

	void killConnection(int socket, int epollFd);

	// Unnused Constructors & Operators
	Cluster(void);
	Cluster(const Cluster &src);
	const Cluster &operator=(const Cluster &src);
};

std::ostream &operator<<(std::ostream &os, const Cluster &cluster);
std::ostream &operator<<(std::ostream &os, const Cluster::VirtualServer &server);

#endif
