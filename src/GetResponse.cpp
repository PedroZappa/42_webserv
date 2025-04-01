/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gfragoso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 18:06:31 by passunca          #+#    #+#             */
/*   Updated: 2025/03/22 22:25:32 by gfragoso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/GetResponse.hpp"

#include "../inc/AResponse.hpp"
#include "../inc/CGI.hpp"

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
    : AResponse(server, request, OK) {};

/**
 * @brief Copy constructor for GetResponse.
 *
 * Creates a copy of an existing GetResponse object.
 *
 * @param obj The GetResponse object to copy.
 */
GetResponse::GetResponse(const GetResponse &obj) : AResponse(obj) {}

/**
 * @brief Destructor for GetResponse.
 *
 * Cleans up resources used by the GetResponse object.
 */
GetResponse::~GetResponse() {}

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
 *         - NOT_MODIFIED if the file has not been modified since the last
 * request.
 */
short GetResponse::loadFile(std::string &path) {
    if (isCGI()) {
        std::string root = _server.getRoot(_locationRoute);
        CGI cgi(
            _request,
            _response,
            root,
            path
        );
        if ((_status = cgi.generateResponse()) != OK)
            getErrorPage();
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
            try {
                time_t requestTime = getTime(it->second);
                time_t fileTime = getTime(lastModified);
                if (requestTime <= fileTime)
                    return (NOT_MODIFIED);
            } catch (const std::exception &e) {
                // Log error parsing the date
                std::stringstream s;
                s << "Error parsing date headers: " << e.what();
                Logger::error(s.str());
            }
        }
        // Load file content into the response body
        _response.body.assign((std::istreambuf_iterator<char>(file)),
                              (std::istreambuf_iterator<char>()));
        file.close();

        // Check for specific download path pattern
        if (_request.uri.compare(0, 10, "/download/") == 0 ||
            _request.uri == "/download") {
            size_t lastSlash = _request.uri.find_last_of('/');
            std::string filename = _request.uri.substr(lastSlash + 1);
            if (filename.empty())
                filename = "download";
            _response.headers.insert(std::make_pair(
                std::string("Content-Disposition"),
                std::string("attachment; filename=\"" + filename + "\"")));
        }
        setMimeType(path);
    }
    loadHeaders();
    return (_status);
}

/**
 * @brief Generates the HTTP response based on the request and server
 * configuration.
 *
 * This method processes the HTTP request to generate an appropriate response.
 * It first sets the location route and checks the HTTP method for validity.
 * If the method is not allowed, it returns an error page. If a redirect is
 * required, it loads the redirect information and returns the response string.
 * The method then determines the file path and checks its validity. If the
 * path is a file, it loads the file content. If the path is a directory, it
 * attempts to load an index file or generate a directory listing if auto-index
 * is enabled. If none of these conditions are met, it returns a forbidden error
 * page.
 *
 * @return A string containing the generated HTTP response.
 */
std::string GetResponse::generateResponse() {
    setLocationRoute();
    if ((_status = checkMethod()) != OK)
        return getErrorPage();
    if (hasReturn()) {
        loadReturn();
        return (getResponseStr());
    }
    std::string path = getPath();

    if ((_status = checkFile(path)) != OK)
        return getErrorPage();

    if (!isDir(path)) {
        if ((_status = loadFile(path)) != OK)
            return getErrorPage();
    } else {
        // Is a directory
        std::string idxFile = getIndexFile(path);
        if (!idxFile.empty() && (checkFile(idxFile) == OK)) {
            if ((_status = loadFile(idxFile)) != OK)
                return getErrorPage();
        } else if (hasAutoIndex()) {
            if ((_status = loadDirectoryListing(path)) != OK)
                return getErrorPage();
        } else {
            _status = FORBIDDEN;
            return getErrorPage();
        }
    }
    return (getResponseStr());
}

/** @} */