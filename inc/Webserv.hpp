/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:43:06 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 18:38:28 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

/* ************************************************************************** */
/*                                 Libraries                                  */
/* ************************************************************************** */

#include "Ansi.h"
#include "Logger.hpp"

// C Libraries
#include <arpa/inet.h> // inet_aton()
#include <fcntl.h>     // O_NONBLOCK F_GETFL F_SETFL
#include <limits.h>
#include <netinet/in.h> // struct sockaddr_in INADDR_ANY
#include <signal.h>     // signal
#include <sys/epoll.h>  // epoll_create()
#include <sys/socket.h> // SOMAXCONN
#include <sys/stat.h>   // stat()
#include <unistd.h>     // close()

// C++ Libraries
#include <climits>  // LONG_MAX
#include <cstring>  // std::memset
#include <fstream>  // std::ifstream
#include <iomanip>  // std::setw
#include <iostream> // std::cout std::cerr std::endl
#include <sstream>  // std::stringstream
#include <typeinfo> // typeid

// STL
#include <algorithm> // std::transform
#include <map>       // std::map
#include <set>       // std::set
#include <vector>    // std::vector

/* ************************************************************************** */
/*                                  Constants */
/* ************************************************************************** */

#define SERVER_NAME "webserv"
#define URL_MAX_SIZE 8192
#define MAX_EPOLL_FD_PATH "/proc/sys/fs/epoll/max_user_watches"

/// @brief Get the maximum number of open fds for epoll
static int getMaxClients() {
	// Get the maximum number of open fds for epoll
	std::ifstream maxClients(MAX_EPOLL_FD_PATH);
	int val = 666; // Default value

	if (maxClients.is_open()) { // if file opened successfully
		maxClients >> val;      // Assign first int in file stream to val
		return (val);
	}

	Logger::error("Failed to open " MAX_EPOLL_FD_PATH);
	return (val);
}

// Unit constants
#define KB 1024
#define MB 1048576
#define GB 1073741824

#define SERVER_PORT 8080
#define TIMEOUT 5
#define MAX_PORTS ((64 * KB) - 1)
#define MAX_BODY_SIZE MB
#define REQ_BUFF_SIZE (2 * KB)

const int MAX_CLIENTS = getMaxClients();

/* ************************************************************************** */
/*                                Enumerations                                */
/* ************************************************************************** */

enum Method { // Keep only the methods that we want to implement
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	PATCH,
	UNKNOWN
};

enum State { TRUE, FALSE, UNSET };

enum ErrCodes {
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,

	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE_INFORMATION = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,

	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	TEMPORARY_REDIRECT = 307,
	PERMANENT_REDIRECT = 308,

	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTHENTICATION_REQUIRED = 407,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PRECONDITION_FAILED = 412,
	PAYLOAD_TOO_LARGE = 413,
	URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	RANGE_NOT_SATISFIABLE = 416,
	EXPECTATION_FAILED = 417,
	IM_A_TEA_POT = 418,
	MISDIRECTED_REQUEST = 421,
	UNPROCESSABLE_ENTITY = 422,
	UPGRADE_REQUIRED = 426,

	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

#endif
