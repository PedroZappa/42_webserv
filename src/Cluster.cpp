/**
 * @defgroup ClusterModule Cluster Management Module
 * @{
 *
 * This module handles the management and operation of server clusters,
 * including setup, event handling, and connection management.
 *
 * @version 1.0
 */

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
#include "../inc/DeleteResponse.hpp"
#include "../inc/ErrorResponse.hpp"
#include "../inc/GetResponse.hpp"
#include "../inc/PostResponse.hpp"
#include "../inc/Utils.hpp"

/**
 * @brief Global variable to track the total size of files stored on the server.
 */
std::size_t storageSize = 0;
bool isRunning = true;

/* ************************************************************************** */
/*                          Constructor & Destructor                          */
/* ************************************************************************** */

/**
 * @brief Constructs a Cluster with a given set of servers.
 *
 * @param servers Vector of servers to add to the cluster.
 * @details Initializes the cluster by adding each server and its associated
 * virtual servers.
 */
Cluster::Cluster(const std::vector<Server> &servers)
	: _servers(), _epollFd(-1) {
	std::vector<Server>::const_iterator serverIt;
	for (serverIt = servers.begin(); serverIt != servers.end(); ++serverIt) {
		_servers.push_back(&(*serverIt)); // Add server to cluster
		std::vector<Socket> netAddrs = (*serverIt).getNetAddr();
		std::vector<std::string> names = (*serverIt).getServerName();

		std::vector<Socket>::const_iterator sockIt; // Add virtual servers
		for (sockIt = netAddrs.begin(); sockIt != netAddrs.end(); ++sockIt) {
			if (names.empty()) { // Add nameless virtual server
				struct VirtualServer vs;
				vs.ip = sockIt->ip;
				vs.port = sockIt->port;
				vs.name = "";
				vs.server = &(*serverIt);
				_virtualServers.push_back(vs);
			} else { // Add named virtual servers
				std::vector<std::string>::const_iterator nameIt;
				for (nameIt = names.begin(); nameIt != names.end(); ++nameIt) {
					struct VirtualServer vs;
					vs.ip = sockIt->ip;
					vs.port = sockIt->port;
					vs.name = *nameIt;
					vs.server = &(*serverIt);
					_virtualServers.push_back(vs);
				}
			}
		}
	}
}

/**
 * @brief Destroys the Cluster, closing all associated resources.
 *
 * @details Closes the epoll instance and all listening sockets.
 */
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

/**
 * @brief Accesses a server by index.
 *
 * @param idx The index of the server to access.
 * @return const Server& Reference to the server at the specified index.
 * @throw std::out_of_range if the index is out of range.
 */
const Server &Cluster::operator[](size_t idx) const {
	if (idx >= _servers.size())
		throw std::out_of_range("Index out of range");
	return (*_servers[idx]);
}

/**
 * @brief Overloads the << operator to output Cluster information.
 *
 * @param os The output stream.
 * @param cluster The Cluster object to output.
 * @return std::ostream& The output stream with Cluster information.
 */
std::ostream &operator<<(std::ostream &os, const Cluster &cluster) {
	// Print basic cluster information
	os << "Cluster Info:\n";

	// Print all Virtual Servers
	os << "Virtual Servers:\n";
	const std::vector<Cluster::VirtualServer> &virtualServers =
		cluster.getVirtualServers();
	for (size_t i = 0; i < virtualServers.size(); ++i) {
		os << "  [" << i << "] " << virtualServers[i] << "\n";
	}

	// Print Listening Sockets
	os << "Listening Sockets:\n";
	const std::vector<int> &listeningSockets = cluster.getListeningSockets();
	for (size_t i = 0; i < listeningSockets.size(); ++i) {
		os << "  [" << i << "] " << listeningSockets[i] << "\n";
	}

	// Print Epoll FD
	os << "Epoll FD: " << cluster.getEpollFd() << "\n";

	return (os);
}

/**
 * @brief Overloads the << operator to output VirtualServer information.
 *
 * @param os The output stream.
 * @param server The VirtualServer object to output.
 * @return std::ostream& The output stream with VirtualServer information.
 */
std::ostream &operator<<(std::ostream &os, const Cluster::VirtualServer &server) {
	os << "VirtualServer(Name: " << server.name << ", IP: " << server.ip
	   << ", Port: " << server.port << ")";
	return (os);
}

/* ************************************************************************** */
/*                                  Checkers                                  */
/* ************************************************************************** */

/**
 * @brief Checks for duplicate virtual servers in the cluster.
 *
 * @return true if duplicates exist, false otherwise.
 */
bool Cluster::hasDuplicates(void) const {
	std::set<const Server *> servers(_servers.begin(), _servers.end());
	return (servers.size() != _servers.size());
}

/* ************************************************************************** */
/*                                   Setup                                    */
/* ************************************************************************** */

/**
 * @brief Sets up the cluster's listening sockets and epoll instance.
 *
 * @details Initializes the epoll instance and configures all virtual server
 * sockets for listening.
 */
void Cluster::setup(void) {
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Starting Cluster Setup");
#endif

	setEpollFd(); // Create epoll instance
	std::set<Socket> sockets = getVirtualServerSockets();
	std::set<Socket>::const_iterator it; // To iterate through sockets

	for ((it = sockets.begin()); (it != sockets.end()); ++it) {
		Socket addr(it->ip, it->port);
		int fd = setSocket(it->ip, it->port);
		startListen(fd);
		setEpollSocket(fd);
	}

#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Cluster Setup Done");
#endif
}

/**
 * @brief Creates an epoll instance for the cluster.
 *
 * @throw std::runtime_error if the epoll instance cannot be created.
 */
void Cluster::setEpollFd(void) {
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

/**
 * @brief Retrieves the set of sockets for all virtual servers.
 *
 * @return std::set<Socket> A set of sockets representing the virtual servers.
 */
std::set<Socket> Cluster::getVirtualServerSockets(void) {
	std::vector<VirtualServer> virtualServers = getVirtualServers();
	std::set<std::string> portsToDelete;

	std::vector<VirtualServer>::const_iterator vsIt;
	for (vsIt = virtualServers.begin(); vsIt != virtualServers.end(); ++vsIt)
		if (vsIt->ip.empty())
			portsToDelete.insert(vsIt->port);

	std::vector<VirtualServer>::iterator vsIt2;
	for (vsIt2 = virtualServers.begin(); vsIt2 != virtualServers.end();)
		if ((portsToDelete.count(vsIt2->port) > 0) && !(vsIt2->ip.empty()))
			vsIt2 = virtualServers.erase(vsIt2);
		else
			++vsIt2;

	std::set<Socket> serversInterestList;
	std::vector<VirtualServer>::const_iterator vsIt3;
	for (vsIt3 = virtualServers.begin(); vsIt3 != virtualServers.end(); ++vsIt3) {
		Socket addr(vsIt3->ip, vsIt3->port);
		serversInterestList.insert(addr);
	}
	return (serversInterestList);
}

/**
 * @brief Configures a socket for a given IP and port.
 *
 * @param ip The IP address for the socket.
 * @param port The port for the socket.
 * @return int The file descriptor of the configured socket.
 * @throw std::runtime_error if the socket cannot be created or bound.
 */
int Cluster::setSocket(const std::string &ip, const std::string &port) {
#ifdef DEBUG
	Logger::debug(
		"Cluster", __func__, "setting up socket: " YEL + ip + ":" + port + NC);
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
	if (ip == "0.0.0.0")       // Bind to all avaliable interfaces
		addr.sin_addr.s_addr = INADDR_ANY;
	else if (ip == "localhost") {
		if (inet_aton("127.0.0.1", &addr.sin_addr) == 0)
			throw std::runtime_error("inet_addr: Invalid IP address");
	} else {
		if (inet_aton(ip.c_str(), &addr.sin_addr) == 0)
			throw std::runtime_error("inet_addr: Invalid IP address");
	}
	addr.sin_port = htons(string2number<int>(port));

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		close(fd);
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to bind socket to address: " + reason);
	}

#ifdef DEBUG
	std::cout << "socket created with fd: " BLU << fd << NC << std::endl;
	Logger::debug(
		"Cluster", __func__, "setting up socket: " YEL + ip + ":" + port + NC);
#endif

	return (fd);
}

/**
 * @brief Begins listening on a specified socket.
 *
 * @param socket The socket file descriptor to listen on.
 * @throw std::runtime_error if the socket cannot be listened on.
 */
void Cluster::startListen(int socket) {
	if (listen(socket, SOMAXCONN) == -1) {
		close(socket);
		throw std::runtime_error("Failed to listen on socket");
	}
}

/**
 * @brief Adds a socket to the epoll instance for monitoring.
 *
 * @param socket The socket file descriptor to add.
 * @throw std::runtime_error if the socket cannot be added to the epoll instance.
 */
void Cluster::setEpollSocket(int socket) {
#ifdef DEBUG
	Logger::debug(
		"Cluster", __func__, "adding socket (epoll_event) to epoll instance");
#endif

	epoll_event ee;
	std::memset(&ee, '\0', sizeof(ee));
	ee.events = EPOLLIN;
	ee.data.fd = socket;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, socket, &ee) == -1) {
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to add socket to epoll instance: " +
								 reason);
	}

#ifdef DEBUG
	std::cout << "epoll_event added with fd: " BLU << socket << NC << std::endl;
	Logger::debug(
		"Cluster", __func__, "adding socket (epoll_event) to epoll instance");
#endif
}

/* ************************************************************************** */
/*                                    Run                                     */
/* ************************************************************************** */

/**
 * @brief Starts the cluster's main event loop.
 *
 * @details Continuously monitors and handles events on the cluster's sockets.
 */
void Cluster::run(void) {
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Started running cluster");
#endif

	std::vector<struct epoll_event> events(MAX_CLIENTS);
	while (isRunning) {
		try {
			int nEvents = epoll_wait(_epollFd, &events[0], MAX_CLIENTS, -1);
			if ((nEvents == -1) && (errno == EINTR)) // Loop exit condition
				continue;
			else if (nEvents == -1) {
				std::string reason = std::strerror(errno);
				throw std::runtime_error("epoll_wait failed: " + reason);
			}

			for (long i = 0; i < nEvents; ++i) {
				int socket = events[i].data.fd;
				if (events[i].events & EPOLLERR) {
					killConnection(socket, _epollFd);
					continue;
				}
				if (isSocketListening(socket))
					setupConnection(socket);
				else if (events[i].events & EPOLLIN)
					handleRequest(socket);
			}
		} catch (const std::exception &e) {
			Logger::error(e.what());
		}
	}

#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Cluster stopped running");
#endif
}

/**
 * @brief Stops the cluster's main event loop.
 *
 * @details Sets the running flag to false, signaling the loop to exit.
 */
void Cluster::stop(void) {
	Logger::warn("Stop triggered. Webserv will stop in moments...");
	isRunning = false;
}

/**
 * @brief Checks if a socket is currently listening.
 *
 * @param socket The socket file descriptor to check.
 * @return true if the socket is listening, false otherwise.
 */
bool Cluster::isSocketListening(int socket) const {
	return (std::find(_listenSockets.begin(), _listenSockets.end(), socket) !=
			_listenSockets.end());
}

/**
 * @brief Sets up a new client connection on a listening socket.
 *
 * @param socket The listening socket file descriptor.
 * @throw std::runtime_error if the connection cannot be established.
 */
void Cluster::setupConnection(int socket) {
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Setting up connection");
#endif

	int clientFd = accept(socket, NULL, NULL);
	if (clientFd == -1) {
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to accept connection: " + reason);
	}
	setSocketToNonBlocking(clientFd);

	// Add client socket to epoll instance
	struct epoll_event ee;
	std::memset(&ee, '\0', sizeof(ee));
	ee.events = (EPOLLIN | EPOLLOUT | EPOLLET);
	ee.data.fd = clientFd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &ee) == -1) {
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

/**
 * @brief Configures a socket to operate in non-blocking mode.
 *
 * @param socket The socket file descriptor to configure.
 * @throw std::runtime_error if the socket cannot be set to non-blocking.
 */
void Cluster::setSocketToNonBlocking(int socket) {
	int flags = fcntl(socket, F_GETFL, 0);
	if ((flags == -1) || fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to set socket to non-blocking: " +
								 reason);
	}
}

/**
 * @brief Handles incoming requests on a socket.
 *
 * @param socket The socket file descriptor to handle requests on.
 * @details Reads data from the socket and processes valid requests.
 */
void Cluster::handleRequest(int socket) {
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "Handling request");
#endif

	char requestBuf[REQ_BUFF_SIZE] = {};
	ssize_t bytesRead = recv(socket, requestBuf, REQ_BUFF_SIZE, 0);
	if (bytesRead < 0) {
		killConnection(socket, _epollFd);
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to read request: " + reason);
	} else if (bytesRead == 0) {
		if (!_requestBuff[socket].empty())
			processRequest(socket, _requestBuff[socket]);
		killConnection(socket, _epollFd);
		return;
	} else { // Handle request
		_requestBuff[socket].append(requestBuf, bytesRead);
		if (isRequestValid(_requestBuff[socket])) {
			processRequest(socket, _requestBuff[socket]);
			_requestBuff.erase(socket); // clear current connect's request buffer
		} else { // If request is not valid, reset the socket
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

/**
 * @brief Validates an incoming request.
 *
 * @param request The request string to validate.
 * @return true if the request is valid, false otherwise.
 */
bool Cluster::isRequestValid(const std::string &request) const {
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "checking if request is valid");
#endif

	// Extract Headers
	std::size_t headerEnd = request.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return (false); // Incomplete Headers

	std::string headers = request.substr(0, headerEnd);
	if (headers.empty())
		return (false); // Empty Headers

	// Check for `Content-Length`
	std::size_t contentLengthPos = headers.find("Content-Length:");
	if (contentLengthPos != std::string::npos) {
		std::size_t contentLengthStart =
			(contentLengthPos + std::string("Content-Length:").length());
		std::size_t contentLengthEnd = headers.find("\r\n", contentLengthStart);
		if (contentLengthEnd != std::string::npos) {
			std::string contentLengthStr = headers.substr(
				contentLengthStart, contentLengthEnd - contentLengthStart);
			std::size_t contentLength =
				string2number<std::size_t>(contentLengthStr);
			// +4 to account for \r\n\r\n
			return (request.size() >= (headerEnd + 4 + contentLength));
		}
	}
	// Check for `Transfer-Encoding: chunked`
	if (headers.find("Transfer-Encoding: chunked") != std::string::npos)
		// Look for end of chunked transfer
		return (request.find("0\r\n\r\n") != std::string::npos);

#ifdef DEBUG
	Logger::debug("Cluster", __func__, "request is valid");
#endif
	// Assumme request is complete if no `Content-Length` or
	// `Transfer-Encoding: chunked` is present
	return (true);
}

/**
 * @brief Processes a valid request.
 *
 * @param socket The socket file descriptor associated with the request.
 * @param request The request string to process.
 * @details
 */
void Cluster::processRequest(int socket, const std::string &request) {
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "processing request");
#endif

	HttpRequest req;
	unsigned short errorStatus = HttpRequestParser::parseHttp(request, req);
	std::string response = getResponse(req, errorStatus, socket);

	ssize_t toSend = send(socket, request.c_str(), request.size(), 0);
	if (toSend == -1) {
		killConnection(socket, _epollFd);
	}

	killConnection(socket, _epollFd);

#ifdef DEBUG
	Logger::debug("Cluster", __func__, "request Processed");
#endif
}

/**
 * @brief Generates a response for a given HTTP request.
 *
 * @param request The HTTP request to process.
 * @param errorStatus The error status code, if any.
 * @param socket The socket file descriptor associated with the request.
 * @return std::string The generated HTTP response.
 * @details Determines the appropriate response type based on the request method
 * and error status, then generates and returns the response.
 */
const std::string Cluster::getResponse(HttpRequest &request,
									   unsigned short &errorStatus,
									   int socket) {
	AResponse *responseCtrl;
	const Server *server = getContext(request, socket);

	if (errorStatus != OK)
		responseCtrl = new ErrorResponse(*server, request, errorStatus);
	else {
		switch (static_cast<int>(request.method)) {
		case GET:
			responseCtrl = new GetResponse(*server, request);
			break;
		case POST:
			responseCtrl = new PostResponse(*server, request, socket, _epollFd);
			break;
		case DELETE:
			responseCtrl = new DeleteResponse(*server, request);
			break;
		}
		/// TODO: Add other methods
	}

	std::string response = responseCtrl->generateResponse();

	delete responseCtrl;
	return (response);
}

/**
 * @brief Retrieves the server context for a given HTTP request and socket.
 *
 * @param request The HTTP request containing headers.
 * @param socket The socket file descriptor associated with the request.
 * @return const Server* Pointer to the server context.
 * @details Determines the appropriate server context based on the request's
 * hostname and the socket's address. If multiple servers match, it returns
 * the first server with a matching hostname. If no server matches, it returns
 * the first server found.
 */
const Server *Cluster::getContext(const HttpRequest &request, int socket) {
	const Socket addr = getSocketAddress(socket);
	std::string hostname = getHostnameFromRequest(request);
	size_t colonPos = hostname.find_first_of(":");
	if (colonPos != std::string::npos)
		hostname.resize(colonPos);

	std::vector<const Server *> validServers;
	std::vector<const Server *>::const_iterator it;
	for (it = _servers.begin(); it != _servers.end(); ++it) {
		std::vector<Socket> netAddrs = (*it)->getNetAddr();

		std::vector<Socket>::const_iterator sockIt;
		for (sockIt = netAddrs.begin(); sockIt != netAddrs.end(); ++sockIt)
			if (*sockIt == addr)
				validServers.push_back(*it);
	}

	// If no address was found, check matching piort
	if (validServers.empty()) {
		std::vector<const Server *>::const_iterator it;
		for (it = _servers.begin(); it != _servers.end(); ++it) {
			std::vector<Socket> netAddrs = (*it)->getNetAddr();

			std::vector<Socket>::const_iterator sockIt;
			for (sockIt = netAddrs.begin(); sockIt != netAddrs.end(); ++sockIt)
				if (sockIt->port == addr.port)
					validServers.push_back(*it);
		}
	}

	// if multiple servers are found...
	if (validServers.size() > 1) {
		std::vector<const Server *>::const_iterator it;
		for (it = validServers.begin(); it != validServers.end(); ++it) {
			std::vector<std::string> serverNames = (*it)->getServerName();

			if (std::find(serverNames.begin(), serverNames.end(), hostname) !=
				serverNames.end())
				return &(**it); // return first matching server
		}
	}

	// if no server was found, return first server
	// Return a reference to avoid dangling pointer
	if (!validServers.empty())
		return (validServers.front());
	throw std::runtime_error("No valid server found for the request");
}

/**
 * @brief Retrieves the socket address for a given socket.
 *
 * @param socket The socket file descriptor to retrieve the address for.
 * @return const Socket The socket address containing IP and port.
 */
const Socket Cluster::getSocketAddress(int socket) {
	struct sockaddr addr;
	socklen_t addrLen = sizeof(addr);
	struct sockaddr_in *addrIn;
	Socket address;

	if (getsockname(socket, &addr, &addrLen) == -1) {
		// Handle error or use a default address
		address.ip = "0.0.0.0";
		address.port = "0";
		return address;
	}
	addrIn = reinterpret_cast<struct sockaddr_in *>(&addr);
	address.ip = inet_ntoa(addrIn->sin_addr);
	address.port = ntohs(addrIn->sin_port);

	return (address);
}

/**
 * @brief Extracts the hostname from an HTTP request.
 *
 * This function iterates over the headers of the given HTTP request to find
 * the "Host" header, which contains the hostname. It performs a
 * case-insensitive comparison to locate the header.
 *
 * @param request The HTTP request containing headers.
 * @return const std::string The extracted hostname or an empty string if the
 * "Host" header is not found.
 */
const std::string Cluster::getHostnameFromRequest(const HttpRequest &request) {
	std::multimap<std::string, std::string>::const_iterator hostname;
	for (hostname = request.headers.begin(); hostname != request.headers.end();
		 ++hostname) {
		if (strcasecmp(hostname->first.c_str(), "host") == 0) {
			return (hostname->second);
		}
	}
	return ("");
}

/**
 * @brief Terminates a connection and removes it from the epoll instance.
 *
 * @param socket The socket file descriptor to close.
 * @param epollFd The epoll instance file descriptor.
 * @throw std::runtime_error if the socket cannot be removed from the epoll instance.
 */
void Cluster::killConnection(int socket, int epollFd) {
#ifdef DEBUG
	Logger::debug("Cluster", __func__, "killing connection");
#endif

	close(socket);
	if (epoll_ctl(epollFd, EPOLL_CTL_DEL, socket, NULL) == -1) {
		std::string reason = std::strerror(errno);
		throw std::runtime_error("Failed to remove socket from epoll: " + reason);
	}

#ifdef DEBUG
	std::cout << "epoll_event removed with fd: " BLU << socket << NC << std::endl;
	Logger::debug("Cluster", __func__, "connection killed");
#endif
}

/* ************************************************************************** */
/*                                  Getters */
/* ************************************************************************** */

/**
 * @brief Retrieves the list of virtual servers in the cluster.
 *
 * @return std::vector<VirtualServer> A vector of virtual servers.
 */
std::vector<Cluster::VirtualServer> Cluster::getVirtualServers(void) const {
	return (_virtualServers);
}

/**
 * @brief Retrieves the list of servers in the cluster.
 *
 * @return const std::vector<const Server*>& A reference to the vector of servers.
 */
const std::vector<const Server *> &Cluster::getServers(void) const {
	return (_servers);
}

/**
 * @brief Retrieves the list of listening sockets.
 *
 * @return const std::vector<int>& A reference to the vector of listening sockets.
 */
const std::vector<int> &Cluster::getListeningSockets(void) const {
	return (_listenSockets);
}

/**
 * @brief Retrieves the file descriptor of the epoll instance.
 *
 * @return int The epoll instance file descriptor.
 */
int Cluster::getEpollFd(void) const {
	return (_epollFd);
}
/** @} */
