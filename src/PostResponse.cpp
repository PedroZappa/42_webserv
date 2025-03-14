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

/**
 * @class PostResponse
 * @brief Handles HTTP POST requests and generates appropriate responses.
 *
 * This class is responsible for processing HTTP POST requests, including
 * checking methods, parsing HTTP data, validating body size, handling file
 * uploads, and triggering CGI scripts if necessary.
 */

/**
 * @brief Global variable to store the response status.
 *
 * This variable holds the current status of the HTTP response being processed.
 * It is used throughout the PostResponse class to determine the appropriate
 * response to send back to the client based on the outcome of various checks
 * and operations.
 */
static unsigned short responseStatus = OK;

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
	loadHeaders();

	return (getResponseStr());
}

/* ************************************************************************** */
/*                              Private Methods                               */
/* ************************************************************************** */

/**
 * @brief Parses the HTTP request headers.
 * @return An unsigned short representing the response status.
 *
 * Checks for the presence of specific headers such as "expect" and
 * "content-length". Sends a 100 Continue response if necessary and
 * validates the request headers.
 */
unsigned short PostResponse::parseHttp() {
	if (hasHeader("expect"))
		if (!send100continue())
			return (responseStatus);
	if (!hasHeader("content-length") ||
		(hasHeader("transfer-encoding") && !isCGI()))
		responseStatus = BAD_REQUEST;
	return responseStatus;
}

/**
 * @brief Checks if a specific header is present in the request.
 * @param header The header to check for.
 * @return True if the header is present, false otherwise.
 */
bool PostResponse::hasHeader(const std::string &header) const {
	if (_request.headers.find(header) != _request.headers.end())
		return (true);
	return (false);
}

/**
 * @brief Sends a 100 Continue response to the client.
 * @return True if the response was sent successfully, false otherwise.
 *
 * Validates the "expect" header and sends a 100 Continue response
 * if the request is valid. Checks for content length and transfer
 * encoding headers to ensure the request is well-formed.
 */
bool PostResponse::send100continue() {
	if (_request.headers.find("expect")->second != "100-continue") {
		responseStatus = BAD_REQUEST;
		return (false);
	}
	if (!hasHeader("content-length") && !hasHeader("transfer-encoding")) {
		responseStatus = BAD_REQUEST;
		return (false);
	}
	if (hasHeader("content-length") &&
		string2number<ssize_t>(_request.headers.find("content-length")->second) >
			_server.getClientMaxBodySize()) {
		responseStatus = PAYLOAD_TOO_LARGE;
		return (false);
	}
	ssize_t sent = send(_clientFd, "HTTP/1.1 100 Continue\r\n\r\n", 28, 0);
	if (sent < 0) {
		responseStatus = INTERNAL_SERVER_ERROR;
		return (false);
	}

	return (true);
}
