/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:00:04 by passunca          #+#    #+#             */
/*   Updated: 2025/03/11 18:26:42 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARESPONSE_HPP
#define ARESPONSE_HPP

#include "HttpParser.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Webserv.hpp"

class Server;
class Location;

/**
 * @struct HttpResponse
 * @brief Represents an HTTP response.
 *
 * This structure holds the status code, headers, and body of an HTTP response.
 */
struct HttpResponse {
	unsigned short status;                           /**< HTTP status code. */
	std::multimap<std::string, std::string> headers; /**< HTTP headers. */
	std::string body;                                /**< HTTP response body. */

	HttpResponse() : status(200) {
	}
};

/**
 * @class AResponse
 * @brief Abstract base class for HTTP responses.
 *
 * This class provides a common interface for generating HTTP responses.
 * Derived classes must implement the generateResponse() method.
 */
class AResponse {
  public:
	// Constructors
	AResponse(const Server &, const HttpRequest &);
	AResponse(const AResponse &);
	const AResponse &operator=(const AResponse &);
	virtual ~AResponse();

	// Public Member Functions
	virtual std::string generateResponse() = 0;

  protected:
	HttpRequest _request;       /**< The HTTP request. */
	HttpResponse _response;     /**< The HTTP response. */
	const Server &_server;      /**< Reference to the server configuration. */
	std::string _locationRoute; /**< The location route. */

	// Checkers
	bool isCGI() const;
	// PostRequest
	short checkMethod() const;
	short checkBodySize() const;
	short checkFile(const std::string &path) const;

	// Utils
	std::string getLastModifiedDate(const std::string &path) const;
	const std::string getResponseStr() const;

	// Response Construction Helpers
	void setMimeType(const std::string& path);
	void loadCommonHeaders(); // TODO:
	// ErrorResponse
	const std::string getErrorPage(int errorStatus);// TODO:
	// PostResponse
	void setLocationRoute(); // TODO:
};

/**
 * @brief Output stream operator for AResponse.
 * @param os The output stream.
 * @param response The AResponse object.
 * @return A reference to the output stream.
 */
std::ostream &operator<<(std::ostream &, const AResponse &);

#endif
