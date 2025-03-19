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

/**
 * @class ErrorResponse
 * @brief Handles the generation of error responses for HTTP requests.
 *
 * This class is responsible for creating error responses based on the server
 * configuration, the HTTP request that triggered the error, and the specific
 * error status code.
 */

#include "../inc/ErrorResponse.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Constructs an ErrorResponse object.
 *
 * Initializes the ErrorResponse with the given server configuration, HTTP
 * request, and error status code.
 *
 * @param server The server configuration.
 * @param request The HTTP request that triggered the error.
 * @param errorStatus The HTTP error status code.
 */

ErrorResponse::ErrorResponse(const Server &server, const HttpRequest &request,
                             short errorStatus)
    : AResponse(server, request, errorStatus) {}

/**
 * @brief Copy constructor.
 *
 * Creates a copy of the given ErrorResponse object.
 *
 * @param other The ErrorResponse object to copy from.
 */
ErrorResponse::ErrorResponse(const ErrorResponse &other)
    : AResponse(other) {}

/**
 * @brief Destructor.
 *
 * Cleans up resources used by the ErrorResponse object.
 */
ErrorResponse::~ErrorResponse() {}

/* ************************************************************************** */
/*                              Public Functions                              */
/* ************************************************************************** */

/**
 * @brief Generates the error response.
 *
 * Constructs and returns a string representing the error page corresponding to
 * the error status.
 *
 * @return A string containing the error page.
 */
std::string ErrorResponse::generateResponse() {
    return getErrorPage();
}
