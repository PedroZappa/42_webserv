/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:43:06 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 11:39:36 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

/* ************************************************************************** */
/*                                 Libraries                                  */
/* ************************************************************************** */

// C Libraries
#include <limits.h>
#include <stdlib.h> // EXIT_SUCCESS EXIT_FAILURE
#include <sys/socket.h> // SOMAXCONN

// C++ Libraries
#include <iomanip> // std::setw
#include <iostream> // std::cout std::cerr std::endl

// STL
#include <map>
#include <vector>
#include <set>

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

enum State {
	TRUE,
	FALSE,
	NIL
};

#endif
