/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:43:06 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 15:22:11 by passunca         ###   ########.fr       */
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

#endif
