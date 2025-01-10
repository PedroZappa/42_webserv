/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:12:12 by passunca          #+#    #+#             */
/*   Updated: 2025/01/10 18:49:08 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Cluster.hpp"
#include "../inc/Utils.hpp"
#include <sys/socket.h>

/* ************************************************************************** */
/*                          Constructor & Destructor                          */
/* ************************************************************************** */

/// @brief Default constructor
/// @param servers Vector of servers to add to the cluster
/// @details Creates a parameterized cluster
Cluster::Cluster(const std::vector<Server> &servers)
	: _servers(), _epollFd(-1) {
	std::vector<Server>::const_iterator it;
	for (it = servers.begin(); it != servers.end(); ++it) {
		_servers.push_back(&(*it)); // Add server to cluster
		std::vector<Socket> netAddrs = (*it).getNetAddr();
		std::vector<std::string> names = (*it).getServerName();

		std::vector<Socket>::const_iterator sockIt; // Add virtual servers
		for (sockIt = netAddrs.begin(); sockIt != netAddrs.end(); ++sockIt) {
			if (names.empty()) { // Add nameless virtual server
				struct VirtualServer vs;
				vs.ip = sockIt->ip;
				vs.port = sockIt->port;
				vs.name = "";
				vs.server = &(*it);
				_virtualServers.push_back(vs);
			} else { // Add named virtual servers
				std::vector<std::string>::const_iterator nameIt;
				for (nameIt = names.begin(); nameIt != names.end(); ++nameIt) {
					struct VirtualServer vs;
					vs.ip = sockIt->ip;
					vs.port = sockIt->port;
					vs.name = *nameIt;
					vs.server = &(*it);
					_virtualServers.push_back(vs);
				}
			}
		}
	}
}

///@brief Default destructor
Cluster::~Cluster() {
	// Close epoll instance
	if (_epollFd != -1)
		close(_epollFd);
	// Close listening sockets
	std::vector<int>::iterator it;
	for (it = _listenSockets.begin(); it != _listenSockets.end(); ++it)
		close(*it);
}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

/// @brief Index operator
/// @param idx The index to get
/// @throw std::out_of_range if the index is out of range
/// @details Returns the server at the specified index
const Server &Cluster::operator[](size_t idx) const {
	if (idx >= _servers.size())
		throw std::out_of_range("Index out of range");
	return (*_servers[idx]);
}

/* ************************************************************************** */
/*                                   Setup                                    */
/* ************************************************************************** */

/// @brief Checks if the cluster has duplicate virtual servers
/// @return True if the cluster has duplicate virtual servers, false otherwise
bool Cluster::hasDuplicates(void) const {
	std::set<const Server *> servers(_servers.begin(), _servers.end());
	return (servers.size() != _servers.size());
}

/// @brief Sets up the cluster's listening sockets
void Cluster::setup(void) {
	setEpollFd(); // Create epoll instance
	std::set<Socket> sockets = getVirtualServerSockets();
	std::set<Socket>::const_iterator it;

	for ((it = sockets.begin()); (it != sockets.end()); ++it) {
		Socket addr(it->ip, it->port);
		int fd = setSocket(it->ip, it->port);
		startListen(fd);
		setEpollSocket(fd);
	}
}

/// @brief Creates an epoll instance
void Cluster::setEpollFd(void) {
	if ((_epollFd = epoll_create(1) == -1))
		throw std::runtime_error("Failed to create epoll instance");
}

/// @brief Gets the virtual server sockets
/// @details Returns a set of interest sockets
std::set<Socket> Cluster::getVirtualServerSockets(void) {
	std::vector<VirtualServer> virtualServers = getVirtualServers();
	std::set<std::string> portsToDelete;

	std::vector<VirtualServer>::const_iterator vsit;
	for (vsit = virtualServers.begin(); vsit != virtualServers.end(); ++vsit)
		if (vsit->ip.empty())
			portsToDelete.insert(vsit->port);

	std::vector<VirtualServer>::iterator vsit2;
	for (vsit2 = virtualServers.begin(); vsit2 != virtualServers.end(); ++vsit2)
		if ((portsToDelete.count(vsit2->port) > 0) && !(vsit2->ip.empty()))
			vsit2 = virtualServers.erase(vsit2);
		else
			++vsit2;

	std::set<Socket> serversInterestList;
	std::vector<VirtualServer>::const_iterator vsit3;
	for (vsit3 = virtualServers.begin(); vsit3 != virtualServers.end(); ++vsit3) {
		Socket addr(vsit3->ip, vsit3->port);
		serversInterestList.insert(addr);
	}
	return (serversInterestList);
}

/// @brief Sets a socket
/// @param ip The IP address of the socket
/// @param port The port of the socket
/// @throw std::runtime_error if the socket could not be created
/// @return The socket file descriptor
int Cluster::setSocket(const std::string &ip, const std::string &port) {
	// Setup Socket
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		throw std::runtime_error("Failed to create socket");
	_listenSockets.push_back(fd);

	// Set socket options
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("Failed to set socket options");

	// Setup socket configuration
	struct sockaddr_in addr;
	std::memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET; // Set IPv4
	if (ip == "0.0.0.0")       // Bind to all avaliable interfaces
		addr.sin_addr.s_addr = INADDR_ANY;
	else if (ip == "localhost") {
		if (inet_aton("127.0.0.1", &addr.sin_addr) == 0)
			throw std::runtime_error("inet_addr: Invalid IP address");
	} else {
		if (inet_aton(ip.c_str(), &addr.sin_addr) == 0)
			throw std::runtime_error("inet_addr: Invalid IP address");
	}
	addr.sin_port = htons(strToN<int>(port));

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		close(fd);
		throw std::runtime_error("Failed to bind socket");
	}
	return (fd);
}

/// @brief Listens on a socket
/// @param socket The socket to listen on
/// @throw std::runtime_error if the socket could not be listened on
void Cluster::startListen(int socket) {
	if (listen(socket, SOMAXCONN) == -1) {
		close(socket);
		throw std::runtime_error("Failed to listen on socket");
	}
}
/// @brief Sets a socket in the epoll instance
/// @param socket The socket to set
/// @throw std::runtime_error if the socket could not be added to the epoll instance
void Cluster::setEpollSocket(int socket) {
	epoll_event ee;
	std::memset(&ee, '\0', sizeof(ee));
	ee.events = EPOLLIN;
	ee.data.fd = socket;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, socket, &ee) == -1)
		throw std::runtime_error("Failed to add socket to epoll instance");
}

/* ************************************************************************** */
/*                                    Run                                     */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                  Getters */
/* ************************************************************************** */

/// @brief Gets the virtual servers
/// @return A vector of virtual servers
std::vector<Cluster::VirtualServer> Cluster::getVirtualServers(void) const {
	return (_virtualServers);
}
