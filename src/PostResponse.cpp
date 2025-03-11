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

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

PostResponse::PostResponse(const Server &server,
						   const HttpRequest &request,
						   int clientFd,
						   int epollFd)
	: AResponse(server, request), _clientFd(clientFd), _epollFd(epollFd) {
}

PostResponse::PostResponse(const PostResponse &other) : AResponse(other) {
}

PostResponse::~PostResponse() {
}

/* ************************************************************************** */
/*                          Public Member Functions                           */
/* ************************************************************************** */

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
