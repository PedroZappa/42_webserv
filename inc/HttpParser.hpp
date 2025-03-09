/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:01:42 by passunca          #+#    #+#             */
/*   Updated: 2025/03/08 19:38:04 by passunca         ###   ########.fr       */
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
 * @brief A class responsible for parsing HTTP requests.
 *
 * The HttpRequestParser class provides methods to parse HTTP request lines,
 * headers, and bodies, and to validate the request format. It updates the
 * HttpRequest object with parsed data and maintains the response status.
 *
 * This class handles the parsing of HTTP requests by breaking down the request
 * into its components: request line, headers, and body. It ensures that the
 * request adheres to the HTTP protocol standards and extracts necessary
 * information for further processing.
 *
 * The class is designed to be robust and efficient, handling various edge cases
 * and ensuring compliance with HTTP/1.1 standards. It supports common HTTP
 * methods and validates the structure and content of requests to prevent
 * malformed data from being processed.
 */
class HttpRequestParser {
  public:
	// Public methods
	static unsigned short parseHttp(const std::string &requestBuf,
									HttpRequest &httpReq);

  private:
	// Private helper methods
	static bool getRequestLine(HttpRequest &httpReq, const std::string &buffer);
	static bool getHeaderFields(HttpRequest &httpReq, const std::string &headers);
	static void parseQueries(HttpRequest &httpReq);

	// Checking
	static bool isMethodValid(const std::string &method);
	static bool isMethodImplemented(const std::string &method);
	static bool isUrlValid(const std::string &url);
	static bool isProtocolVersionValid(const std::string &protocolVersion);
	// Decoding
	static std::string decodeUrl(const std::string &encoded);
	// Trimming
	static std::string trim(const std::string &str);
	static void trimNull(std::string &str);

	// Private Constructors (uninstantiable)
	HttpRequestParser();
	~HttpRequestParser();
};

// Overload the stream insertion operator 
std::ostream &operator<<(std::ostream &os, const HttpRequest &httpReq);

#endif
