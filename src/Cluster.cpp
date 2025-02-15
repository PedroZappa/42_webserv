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

bool isRunning = true; // Is server running?

/* ************************************************************************** */
/*                          Constructor & Destructor                          */
/* ************************************************************************** */

/// @brief Default constructor
/// @param servers Vector of servers to add to the cluster
/// @details Creates a parameterized cluster
Cluster::Cluster(const std::vector<Server> &servers)
	: _servers(), _epollFd(-1)
{
	std::vector<Server>::const_iterator it;
	for (it = servers.begin(); it != servers.end(); ++it)
	{
		_servers.push_back(&(*it)); // Add server to cluster
		std::vector<Socket> netAddrs = (*it).getNetAddr();
		std::vector<std::string> names = (*it).getServerName();

		std::vector<Socket>::const_iterator sockIt; // Add virtual servers
		for (sockIt = netAddrs.begin(); sockIt != netAddrs.end(); ++sockIt)
		{
			if (names.empty())
			{ // Add nameless virtual server
				struct VirtualServer vs;
				vs.ip = sockIt->ip;
				vs.port = sockIt->port;
				vs.name = "";
				vs.server = &(*it);
				_virtualServers.push_back(vs);
			}
			else
			{ // Add named virtual servers
				std::vector<std::string>::const_iterator nameIt;
				for (nameIt = names.begin(); nameIt != names.end(); ++nameIt)
				{
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
Cluster::~Cluster()
{
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
const Server &Cluster::operator[](size_t idx) const
{
	if (idx >= _servers.size())
		throw std::out_of_range("Index out of range");
	return (*_servers[idx]);
}

std::ostream &operator<<(std::ostream &os, const Cluster &cluster)
{
	// Print basic cluster information
	os << "Cluster Info:\n";

	// Print all Virtual Servers
	os << "Virtual Servers:\n";
	const std::vector<Cluster::VirtualServer> &virtualServers =
		cluster.getVirtualServers();
	for (size_t i = 0; i < virtualServers.size(); ++i)
	{
		os << "  [" << i << "] " << virtualServers[i] << "\n";
	}

	// Print Listening Sockets
	os << "Listening Sockets:\n";
	const std::vector<int> &listeningSockets = cluster.getListeningSockets();
	for (size_t i = 0; i < listeningSockets.size(); ++i)
	{
		os << "  [" << i << "] " << listeningSockets[i] << "\n";
	}

	// Print Epoll FD
	os << "Epoll FD: " << cluster.getEpollFd() << "\n";

	return (os);
}

/// @brief Overload the << operator for the Cluster::VirtualServer class
/// @param os The output stream
/// @param server The VirtualServer object
/// @return The output stream
std::ostream &operator<<(std::ostream &os, const Cluster::VirtualServer &server)
{
	os << "VirtualServer(Name: " << server.name << ", IP: " << server.ip
	   << ", Port: " << server.port << ")";
	return (os);
}

/* ************************************************************************** */
/*                                  Checkers                                  */
/* ************************************************************************** */

/// @brief Checks if the cluster has duplicate virtual servers
/// @return True if the cluster has duplicate virtual servers, false otherwise
bool Cluster::hasDuplicates(void) const
{
	std::set<const Server *> servers(_servers.begin(), _servers.end());
	return (servers.size() != _servers.size());
}

/* ************************************************************************** */
/*                                   Setup                                    */
/* ************************************************************************** */

/// @brief Sets up the cluster's listening sockets
void Cluster::setup(void)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Starting Cluster Setup");
#endif

	setEpollFd(); // Create epoll instance
	std::set<Socket> sockets = getVirtualServerSockets();
	std::set<Socket>::const_iterator it; // To iterate through sockets

	for ((it = sockets.begin()); (it != sockets.end()); ++it)
	{
		Socket addr(it->ip, it->port);
		int fd = setSocket(it->ip, it->port);
		startListen(fd);
		setEpollSocket(fd);
	}

#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Cluster Setup Done");
#endif
}

/// @brief Creates an epoll instance
void Cluster::setEpollFd(void)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "creating epoll instance");
#endif

	_epollFd = epoll_create(1);
	if (_epollFd == -1)
		throw std::runtime_error("Failed to create epoll instance");

#ifdef DEBUG
	std::cout << "epoll instance created with fd: " BLU << _epollFd << NC
			  << std::endl;
	Logger::debug("Cluster", __func__, "epoll instance created");
#endif
}

/// @brief Gets the virtual server sockets
/// @details Returns a set of interest sockets
std::set<Socket> Cluster::getVirtualServerSockets(void)
{
	std::vector<VirtualServer> virtualServers = getVirtualServers();
	std::set<std::string> portsToDelete;

	std::vector<VirtualServer>::const_iterator vsit;
	for (vsit = virtualServers.begin(); vsit != virtualServers.end(); ++vsit)
		if (vsit->ip.empty())
			portsToDelete.insert(vsit->port);

	std::vector<VirtualServer>::iterator vsit2;
	for (vsit2 = virtualServers.begin(); vsit2 != virtualServers.end();)
		if ((portsToDelete.count(vsit2->port) > 0) && !(vsit2->ip.empty()))
			vsit2 = virtualServers.erase(vsit2);
		else
			++vsit2;

	std::set<Socket> serversInterestList;
	std::vector<VirtualServer>::const_iterator vsit3;
	for (vsit3 = virtualServers.begin(); vsit3 != virtualServers.end(); ++vsit3)
	{
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
int Cluster::setSocket(const std::string &ip, const std::string &port)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "setting up socket: " YEL + ip + ":" + port + NC);
#endif

	// Setup Socket
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		throw std::runtime_error("Failed to create socket");
	_listenSockets.push_back(fd);

	// Set socket options
	int optval = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw std::runtime_error("Failed to set socket options");

	// Setup socket configuration
	struct sockaddr_in addr;
	std::memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET; // Set IPv4
	if (ip == "0.0.0.0")	   // Bind to all avaliable interfaces
		addr.sin_addr.s_addr = INADDR_ANY;
	else if (ip == "localhost")
	{
		if (inet_aton("127.0.0.1", &addr.sin_addr) == 0)
			throw std::runtime_error("inet_addr: Invalid IP address");
	}
	else
	{
		if (inet_aton(ip.c_str(), &addr.sin_addr) == 0)
			throw std::runtime_error("inet_addr: Invalid IP address");
	}
	addr.sin_port = htons(strToN<int>(port));

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		close(fd);
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to bind socket to address: " + reason);
	}

#ifdef DEBUG
	std::cout << "socket created with fd: " BLU << fd << NC << std::endl;
	Logger::debug("Cluster", __func__, "setting up socket: " YEL + ip + ":" + port + NC);
#endif

	return (fd);
}

/// @brief Listens on a socket
/// @param socket The socket to listen on
/// @throw std::runtime_error if the socket could not be listened on
void Cluster::startListen(int socket)
{
	if (listen(socket, SOMAXCONN) == -1)
	{
		close(socket);
		throw std::runtime_error("Failed to listen on socket");
	}
}
/// @brief Sets a socket in the epoll instance
/// @param socket The socket to set
/// @throw std::runtime_error if the socket could not be added to the epoll instance
void Cluster::setEpollSocket(int socket)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "adding socket (epoll_event) to epoll instance");
#endif

	epoll_event ee;
	std::memset(&ee, '\0', sizeof(ee));
	ee.events = EPOLLIN;
	ee.data.fd = socket;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, socket, &ee) == -1)
	{
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to add socket to epoll instance: " +
								 reason);
	}

#ifdef DEBUG
	std::cout << "epoll_event added with fd: " BLU << socket << NC << std::endl;
	Logger::debug("Cluster", __func__, "adding socket (epoll_event) to epoll instance");
#endif
}

/* ************************************************************************** */
/*                                    Run                                     */
/* ************************************************************************** */

/// @brief Start the cluster
void Cluster::run(void)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Started running cluster");
#endif

	std::vector<struct epoll_event> events(MAX_CLIENTS);
	while (isRunning)
	{
		try
		{
			int nEvents = epoll_wait(_epollFd, &events[0], MAX_CLIENTS, -1);
			if ((nEvents == -1) && (errno == EINTR)) // Loop exit condition
				continue;
			else if (nEvents == -1)
			{
				std::string reason = std::strerror(errno);
				throw std::runtime_error("epoll_wait failed: " + reason);
			}

			for (long i = 0; i < nEvents; ++i)
			{
				int socket = events[i].data.fd;
				if (events[i].events & EPOLLERR)
				{
					killConnection(socket, _epollFd);
					continue;
				}
				if (isSocketListening(socket))
					setupConnection(socket);
				else if (events[i].events & EPOLLIN)
					handleRequest(socket);
			}
		}
		catch (const std::exception &e)
		{
			Logger::error(e.what());
		}
	}

#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Cluster stopped running");
#endif
}

void Cluster::stop(void)
{
	Logger::warn("Stop triggered. Webserv will stop in moments...");
	isRunning = false;
}

/// @brief Checks if a socket is listening
/// @param socket The socket to check
/// @return true if the socket is listening, false otherwise
bool Cluster::isSocketListening(int socket) const
{
	return (std::find(_listenSockets.begin(), _listenSockets.end(), socket) !=
			_listenSockets.end());
}

/// @brief Sets up a new connection
/// @param socket The socket to set up
/// @throw std::runtime_error if the connection could not be set up
void Cluster::setupConnection(int socket)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Setting up connection");
#endif

	int clientFd = accept(socket, NULL, NULL);
	if (clientFd == -1)
	{
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to accept connection: " + reason);
	}
	setSocketToNonBlocking(clientFd);

	// Add client socket to epoll instance
	struct epoll_event ee;
	std::memset(&ee, '\0', sizeof(ee));
	ee.events = (EPOLLIN | EPOLLOUT | EPOLLET);
	ee.data.fd = clientFd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &ee) == -1)
	{
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to add client socket to epoll "
								 "instance: " +
								 reason);
	}

#ifdef DEBUG
	std::stringstream s;
	s << "New connection setup\nepoll_event added with fd: " BLU << clientFd;
	Logger::debug("Cluster", __func__, s.str());
#endif
}

/// @brief Sets a socket to non-blocking
/// @param socket The socket to set
/// @throw std::runtime_error if the socket could not be set to non-blocking
void Cluster::setSocketToNonBlocking(int socket)
{
	int flags = fcntl(socket, F_GETFL, 0);
	if ((flags == -1) || fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to set socket to non-blocking: " +
								 reason);
	}
}

/// @brief Handles a request
/// @param socket The socket to handle
void Cluster::handleRequest(int socket)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Handling request");
#endif

	char requestBuf[REQ_BUFF_SIZE] = {};
	ssize_t bytesRead = recv(socket, requestBuf, REQ_BUFF_SIZE, 0);
	if (bytesRead < 0)
	{
		killConnection(socket, _epollFd);
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to read request: " + reason);
	}
	else if (bytesRead == 0)
	{
		if (!_requestBuff[socket].empty())
			processRequest(socket, _requestBuff[socket]);
		killConnection(socket, _epollFd);
		return;
	}
	else
	{
		_requestBuff[socket].append(requestBuf, bytesRead);
		if (isRequestValid(_requestBuff[socket]))
		{
			processRequest(socket, _requestBuff[socket]);
			_requestBuff.erase(socket); // clear current connect's request buffer
		}
		else
		{ // If request is not valid, reset the socket
			struct epoll_event ee;
			std::memset(&ee, '\0', sizeof(ee));
			ee.events = (EPOLLIN | EPOLLOUT | EPOLLHUP);
			ee.data.fd = socket;

			if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, socket, &ee) == -1)
				killConnection(socket, _epollFd);
		}
	}

#ifdef DEBUG
	std::cout << "handling request on fd: " BLU << socket << NC << std::endl;
	Logger::debug("Cluster", __func__, "request handled");
#endif
}

/// @brief Checks if a request is valid
/// @param request The request to check
/// @return true if the request is valid, false otherwise
bool Cluster::isRequestValid(const std::string &request) const
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "checking if request is valid");
#endif

	// TODO: GO GABRIEL GO!!
	(void)request;
	return (true);

#ifdef DEBUG
	Logger::debug("Cluster", __func__, "request is valid");
#endif
}

/// @brief Processes a request
/// @param socket The socket to process
/// @param request The request to process
void Cluster::processRequest(int socket, const std::string &request)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "processing request");
#endif

	// TODO: process request
	// Go GABRIEL GO!!
	(void)socket;
	(void)request;

#ifdef DEBUG
	Logger::debug("Cluster", __func__, "request Processed");
#endif
}
/// @brief Kills a connection
/// @param socket The socket to kill
/// @param epollFd The epoll instance to remove the socket from
void Cluster::killConnection(int socket, int epollFd)
{
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "killing connection");
#endif

	close(socket);
	if (epoll_ctl(epollFd, EPOLL_CTL_DEL, socket, NULL) == -1)
	{
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to remove socket from epoll: " +
								 reason);
	}

#ifdef DEBUG
	std::cout << "epoll_event removed with fd: " BLU << socket << NC
			  << std::endl;
	Logger::debug("Cluster", __func__, "connection killed");
#endif
}

/* ************************************************************************** */
/*                                  Getters */
/* ************************************************************************** */

/// @brief Gets the virtual servers
/// @return A vector of virtual servers
std::vector<Cluster::VirtualServer> Cluster::getVirtualServers(void) const
{
	return (_virtualServers);
}

/// @brief Gets the servers
/// @return A vector of servers
const std::vector<const Server *> &Cluster::getServers(void) const
{
	return (_servers);
}

/// @brief Gets the listen sockets
/// @return A vector of listen sockets
const std::vector<int> &Cluster::getListeningSockets(void) const
{
	return (_listenSockets);
}

/// @brief Gets epoll instance fd
/// @return epoll instance fd
int Cluster::getEpollFd(void) const
{
	return (_epollFd);
}
