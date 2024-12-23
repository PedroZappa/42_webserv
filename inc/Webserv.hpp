/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:43:06 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 09:52:47 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

/* ************************************************************************** */
/*                                 Libraries                                  */
/* ************************************************************************** */

// C Libraries
#include <limits.h>

// C++ Libraries
#include <iomanip>

// STL
#include <map>
#include <vector>

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
