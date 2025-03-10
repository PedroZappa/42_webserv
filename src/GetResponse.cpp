/**
 * @defgroup GetResponseModule GetResponse Module
 * @{
 *
 * This module handles the construction and management of HTTP GET responses.
 * It processes HTTP requests, loads requested files, and prepares appropriate
 * HTTP responses, including handling CGI scripts and managing headers.
 *
 * @version 1.0
 */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 18:06:31 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 18:48:24 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/GetResponse.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Constructs a GetResponse object.
 *
 * Initializes a GetResponse object using the provided server and HTTP request.
 *
 * @param server The server configuration used for the response.
 * @param request The HTTP request to be processed.
 */
GetResponse::GetResponse(const Server &server, const HttpRequest &request)
	: AResponse(server, request) {};

/**
 * @brief Copy constructor for GetResponse.
 *
 * Creates a copy of an existing GetResponse object.
 *
 * @param obj The GetResponse object to copy.
 */
GetResponse::GetResponse(const GetResponse &obj) : AResponse(obj) {
}

/**
 * @brief Destructor for GetResponse.
 *
 * Cleans up resources used by the GetResponse object.
 */
GetResponse::~GetResponse() {
}

/* ************************************************************************** */
/*                               Public Methods                               */
/* ************************************************************************** */

/**
 * @brief Loads the requested file and prepares the HTTP response.
 *
 * This method checks if the request is for a CGI script or a regular file.
 * It handles the "If-Modified-Since" header to determine if the file has been
 * modified since the last request. If the file is not modified, it returns
 * a NOT_MODIFIED status. Otherwise, it loads the file content into the response
 * body and sets the appropriate headers, including content disposition for
 * downloads.
 *
 * @param path The path to the file to be loaded.
 * @return A status code indicating the result of the operation.
 *         - OK if the file is successfully loaded.
 *         - INTERNAL_SERVER_ERROR if the file cannot be opened.
 *         - NOT_MODIFIED if the file has not been modified since the last request.
 */
short GetResponse::loadFile(std::string &path) {
	if (isCGI()) {
		// TODO: Handle CGI Response
	} else {
		std::ifstream file(path.c_str());
		if (!file.is_open())
			return (INTERNAL_SERVER_ERROR);

		// Check for "If-Modified-Since Header"
		std::multimap<std::string, std::string> headers = _request.headers;
		std::multimap<std::string, std::string>::iterator it;
		it = headers.find("If-Modified-Since");
		if (it != headers.end()) {
			std::string lastModified = getLastModifiedDate(path);
			if (getTime(it->second) <= getTime(lastModified))
				return (NOT_MODIFIED);
		}
		// Load file content into the response body
		_response.body.assign((std::istreambuf_iterator<char>(file)),
							  (std::istreambuf_iterator<char>()));
		file.close();

		if (_request.uri.find("download") != std::string::npos) {
			size_t lastSlash = _request.uri.find_last_of('/');
			std::string filename = _request.uri.substr(lastSlash + 1);
			if (filename.empty()) 
				filename = "download";
			_response.headers.insert(std::make_pair(
				std::string("Content-Disposition"),
				std::string("attachment; filename=\"" + filename + "\"")
			));
		}
		setMimeType(path);
	}
	loadCommonHeaders();
	return (OK);
}
/** @} */

