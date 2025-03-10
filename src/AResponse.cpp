/**
 * @defgroup AResponseModule AResponse Class Module
 * @{
 *
 * This module provides the AResponse class, which serves as an abstract base class
 * for handling HTTP responses. It includes constructors, a destructor, and an
 * assignment operator. The class is initialized with server configuration and
 * HTTP request details.
 *
 * @version 1.0
 */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:09:55 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 16:47:46 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/AResponse.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Constructs an AResponse object.
 * @param server The server configuration.
 * @param request The HTTP request.
 *
 * Initializes the AResponse object with the given server configuration
 * and HTTP request.
 */
AResponse::AResponse(const Server &server, const HttpRequest &request)
	: _server(server), _request(request) {
}

/**
 * @brief Copy constructor for AResponse.
 * @param other The AResponse object to copy from.
 *
 * Initializes a new AResponse object as a copy of the given object.
 */
AResponse::AResponse(const AResponse &other) 
    : _server(other._server), _request(other._request) {
}

/**
 * @brief Destructor for AResponse.
 */
AResponse::~AResponse() {}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

/**
 * @brief Assignment operator for AResponse.
 * @param other The AResponse object to assign from.
 * @return A reference to this AResponse object.
 *
 * The assignment operator is intentionally left empty as this is an
 * abstract base class.
 */
const AResponse &AResponse::operator=(const AResponse &other) {
	(void)other;
	// Assignment intentionally does nothing for abstract base class
	return (*this);
}

/** @} */

