/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:43:06 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 15:25:43 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

/* ************************************************************************** */
/*                                 Libraries                                  */
/* ************************************************************************** */

// C Libraries
#include <arpa/inet.h> // inet_aton()
#include <limits.h>
#include <netinet/in.h> // struct sockaddr_in INADDR_ANY
#include <unistd.h>     // close()
#include <sys/socket.h> // SOMAXCONN

// C++ Libraries
#include <iomanip>  // std::setw
#include <iostream>	// std::cout std::cerr std::endl
#include <cstring>	// std::memset
#include <fstream>	// std::ifstream

// STL
#include <map>			// std::map
#include <set>			// std::set
#include <vector>		// std::vector
#include <algorithm>	// std::transform

/* ************************************************************************** */
/*                                  Defines                                   */
/* ************************************************************************** */

#define SERVER_NAME "webserv"
#define SERVER_PORT 8080
#define MAX_CLIENTS 100
#define TIMEOUT 5

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
	PATCH
};

enum State { TRUE, FALSE, NIL };

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
