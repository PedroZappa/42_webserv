/**
 * @defgroup AResponseModule AResponse Class Module
 * @{
 *
 * This module provides the AResponse class, which serves as an abstract base
 * class for handling HTTP responses. It includes constructors, a destructor,
 * and an assignment operator. The class is initialized with server
 * configuration and HTTP request details.
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
	: _request(request), _server(server) {
}

/**
 * @brief Copy constructor for AResponse.
 * @param other The AResponse object to copy from.
 *
 * Initializes a new AResponse object as a copy of the given object.
 */
AResponse::AResponse(const AResponse &other)
	: _request(other._request), _response(other._response),
	  _server(other._server), _locationRoute(other._locationRoute) {
}

/**
 * @brief Destructor for AResponse.
 */
AResponse::~AResponse() {
}

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

/* ************************************************************************** */
/*                            Protected Methods                               */
/* ************************************************************************** */

/**
 * @brief Checks if the HTTP method is allowed.
 * @return A status code indicating if the method is allowed or not.
 *
 * This method checks if the HTTP method of the request is within the
 * allowed methods for the server configuration and location route.
 */
short AResponse::checkMethod() const {
	const std::set<Method> allowedMethods =
		_server.getValidMethods(_locationRoute);
	std::set<Method>::const_iterator it = allowedMethods.find(_request.method);
	if (it == allowedMethods.end())
		return (METHOD_NOT_ALLOWED);
	return (OK);
}

/**
 * @brief Checks if the request body size exceeds the maximum allowed size.
 * @return A status code indicating if the body size is acceptable or too large.
 *
 * This method compares the size of the HTTP request body with the maximum
 * allowed size specified in the server configuration for the current
 * location route. If the body size exceeds the limit, it returns a status
 * code indicating that the payload is too large; otherwise, it returns a
 * status code indicating that the body size is acceptable.
 */
short AResponse::checkBodySize() const {
	if (_request.body.size() > _server.getClientMaxBodySize(_locationRoute))
		return (PAYLOAD_TOO_LARGE);
	return (OK);
}

/** @} */
