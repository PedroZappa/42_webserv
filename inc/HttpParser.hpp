/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:01:42 by passunca          #+#    #+#             */
/*   Updated: 2025/03/08 19:17:22 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file HttpParser.hpp
 * @brief Defines the HttpRequest structure for parsing HTTP requests.
 */

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "Webserv.hpp"

/**
 * @struct HttpRequest
 * @brief Represents an HTTP request with its components.
 */
struct HttpRequest {
	// Request Line
	enum Method method;
	std::string uri;
	std::string decodedUri;
	std::string protocolVersion;

	// Header
	std::multimap<std::string, std::string> headers;
	// Query Parameters
	std::multimap<std::string, std::string> queryParams;
	// Request Body
	std::string body;

	// Constructors
	HttpRequest() : method(UNKNOWN) {};
};

/**
 * @class HttpRequestParser
 * @brief Provides functionality to parse HTTP requests.
 */
class HttpRequestParser {
  public:
	// Public methods
	static unsigned short parseHttp(const std::string &requestBuf,
									HttpRequest &httpReq);

  private:
	// Private helper methods

	// Private Constructors (uninstantiable)
	HttpRequestParser();
	~HttpRequestParser();
};

#endif
