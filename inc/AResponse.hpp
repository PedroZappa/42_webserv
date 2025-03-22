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
#include "Utils.hpp"
#include "Webserv.hpp"
#include <vector>

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

    HttpResponse() : status(OK) {}
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
    AResponse(const Server &, const HttpRequest &, const short);
    AResponse(const AResponse &);
    const AResponse &operator=(const AResponse &);
    virtual ~AResponse();

    // Public Member Functions
    virtual std::string generateResponse() = 0;
	short getStatus() const;

  protected:
    HttpRequest _request;       /**< The HTTP request. */
    HttpResponse _response;     /**< The HTTP response. */
    const Server &_server;      /**< Reference to the server configuration. */
    std::string _locationRoute; /**< The location route. */
	short _status;		 		/**< The HTTP status code. */

    // Checkers
    bool isCGI() const;
    // GetResponse
    bool hasReturn() const;
    void loadReturn();
    bool isDir(const std::string &path) const;
    const std::string getIndexFile(const std::string &path) const;
    bool hasAutoIndex() const;

	// Utils
    short loadDirectoryListing(const std::string &path);
	std::string addFileEntry(std::string& name, const std::string& path);


    // PostRequest
    short checkMethod() const;
    short checkBodySize() const;
    short checkFile(const std::string &path) const;

    // Getters
    std::string getLastModifiedDate(const std::string &path) const;
    const std::string getResponseStr() const;
    const std::string getPath() const;
    const std::string getPath(const std::string &root,
                              const std::string &path) const;

    // Setters
    void setMimeType(const std::string &path);
    void loadHeaders();
    // ErrorResponse
    const std::string getErrorPage(short status);
    // PostResponse
    void setLocationRoute();
};

/**
 * @brief Output stream operator for AResponse.
 * @param os The output stream.
 * @param response The AResponse object.
 * @return A reference to the output stream.
 */
std::ostream &operator<<(std::ostream &, const AResponse &);

#endif

