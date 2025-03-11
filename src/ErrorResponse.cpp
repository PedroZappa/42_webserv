/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 18:22:17 by passunca          #+#    #+#             */
/*   Updated: 2025/03/11 18:25:57 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ErrorResponse.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Constructs an ErrorResponse object with the specified server, request, and error status.
 * 
 * @param server The server configuration.
 * @param request The HTTP request that triggered the error.
 * @param errorStatus The HTTP error status code.
 */
ErrorResponse::ErrorResponse(const Server &server,
							 const HttpRequest &request,
							 short errorStatus)
	: AResponse(server, request), _errorStatus(errorStatus) {
}

/**
 * @brief Copy constructor for ErrorResponse.
 * 
 * @param other The ErrorResponse object to copy from.
 */
ErrorResponse::ErrorResponse(const ErrorResponse &other) : 
	AResponse(other), _errorStatus(other._errorStatus) {
}

/**
 * @brief Destructor for ErrorResponse.
 */
ErrorResponse::~ErrorResponse() {
}

/* ************************************************************************** */
/*                              Public Functions                              */
/* ************************************************************************** */

/**
 * @brief Generates the error response as a string.
 * 
 * @return A string containing the error page corresponding to the error status.
 */
std::string ErrorResponse::generateResponse() {
	return getErrorPage(_errorStatus);
}
