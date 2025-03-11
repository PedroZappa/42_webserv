/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 18:19:52 by passunca          #+#    #+#             */
/*   Updated: 2025/03/11 18:19:55 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/PostResponse.hpp"
#include "../inc/Utils.hpp"
#include "../inc/Webserv.hpp"

static unsigned short responseStatus = OK;

/**
 * @class PostResponse
 * @brief Handles HTTP POST requests and generates appropriate responses.
 *
 * This class is responsible for processing HTTP POST requests, including
 * checking methods, parsing HTTP data, validating body size, handling file
 * uploads, and triggering CGI scripts if necessary.
 */

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Constructs a PostResponse object.
 * @param server Reference to the server configuration.
 * @param request Reference to the HTTP request.
 * @param clientFd File descriptor for the client connection.
 * @param epollFd File descriptor for epoll instance.
 */
PostResponse::PostResponse(const Server &server,
						   const HttpRequest &request,
						   int clientFd,
						   int epollFd)
	: AResponse(server, request), _clientFd(clientFd), _epollFd(epollFd) {
}

/**
 * @brief Copy constructor for PostResponse.
 * @param other Another PostResponse object to copy from.
 */
PostResponse::PostResponse(const PostResponse &other) : AResponse(other) {
}

/**
 * @brief Destructor for PostResponse.
 */
PostResponse::~PostResponse() {
}

/* ************************************************************************** */
/*                          Public Member Functions                           */
/* ************************************************************************** */

/**
 * @brief Generates the HTTP response for a POST request.
 * @return A string containing the HTTP response.
 *
 * This function processes the POST request by checking the method, parsing
 * the HTTP data, validating the body size, and handling file uploads. If
 * the request is not a CGI request, it performs form checks, body checks,
 * and file uploads, generating a default upload response upon success.
 * If the request is a CGI request, it triggers the CGI script.
 */
std::string PostResponse::generateResponse() {
	unsigned short status = OK;
	setLocationRoute();

	if ((status = checkMethod()) != OK)
		return getErrorPage(status);
	if ((status = parseHttp()) != OK)
		return getErrorPage(status);
	if ((status = checkBodySize()) != OK)
		return getErrorPage(status);

	if (!isCGI()) {
		if ((status = checkForm()) != OK)
			return getErrorPage(status);

		if ((status = checkBody()) != OK)
			return getErrorPage(status);

		if ((status = getFile()) != OK)
			return getErrorPage(status);

		if ((status = uploadFile()) != OK)
			return getErrorPage(status);
		_response.body = generateDefaultUploadResponse();
		_response.status = CREATED;
	} else { //  Trigger CGI
			 // TODO:
	}
	loadCommonHeaders();

	return (getResponseStr());
}

/**
 * @brief Generates a default HTML response for successful file uploads.
 * @return A string containing the HTML response.
 *
 * This function returns a simple HTML page indicating that the file upload
 * was successful, with a link to return to the index page.
 */
static std::string generateDefaultUploadResponse() {
	return "<!DOCTYPE html>\n"
		   "<html lang=\"en\">\n"
		   "<head>\n"
		   "\t<meta charset=\"UTF-8\">\n"
		   "\t<meta name=\"viewport\" content=\"width=device-width, "
		   "initial-scale=1.0\">\n"
		   "\t<link rel=\"icon\" href=\"assets/favicon.ico\" "
		   "type=\"image/x-icon\">\n"
		   "\t<link rel=\"stylesheet\" href=\"assets/css/style.css\">\n"
		   "\t<title>Upload Successful</title>\n"
		   "</head>\n"
		   "<body>\n"
		   "\t<h1>File Uploaded Successfully!</h1>\n"
		   "\t<p>Your file has been uploaded.</p>\n"
		   "\t<a href=\"index.html\">Back to Index</a>\n"
		   "</body>\n"
		   "</html>\n";
}
