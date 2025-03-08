/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:19:55 by passunca          #+#    #+#             */
/*   Updated: 2025/03/08 19:41:01 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file HttpParser.cpp
 * @brief Implementation of HTTP request parsing logic.
 */

#include "../inc/HttpParser.hpp"
#include "../inc/Webserv.hpp"

/**
 * @brief Status of the HTTP response.
 */
static int responseStatus = OK;

/**
 * @brief Parses an HTTP request from a buffer.
 *
 * This function parses the HTTP request line, headers, and body from the given
 * request buffer and populates the HttpRequest object.
 *
 * @param requestBuf The buffer containing the raw HTTP request.
 * @param httpReq The HttpRequest object to populate with parsed data.
 * @return The HTTP response status code.
 */
unsigned short HttpRequestParser::parseHttp(const std::string &requestBuf,
											HttpRequest &httpReq) {
	responseStatus = OK;

	// Catch invalid requests
	if (requestBuf.empty() ||
		(requestBuf.find_first_of("\r\n\t") == std::string::npos)) {
		responseStatus = BAD_REQUEST;
		return responseStatus;
	}

	// Prepare Buffers & flags for parsing
	std::stringstream bufferStream(requestBuf);
	std::string buffer;
	bool requestLineParsed = false;
	bool headerParsed = false;

	while (std::getline(bufferStream, buffer)) {
		if (!requestLineParsed) { // Parse request lines
			if (!getRequestLine(httpReq, buffer))
				return responseStatus;
			requestLineParsed = true;
		} else if (!headerParsed && (buffer != "\r")) { // Header fields
			if (!getHeaderFields(httpReq, buffer))
				return responseStatus;
		} else if (buffer.empty() || (buffer == "\r")) { // Header End
			headerParsed = true;
			break;
		}
	}
	parseQueries(httpReq);

	// Get body
	std::string body((std::istreambuf_iterator<char>(bufferStream)),
					 std::istreambuf_iterator<char>());
	// Append body
	httpReq.body = body;

	return responseStatus;
}

bool HttpRequestParser::getRequestLine(HttpRequest &httpReq,
									   const std::string &buffer) {
	if (std::isspace(buffer[0]))
		return false;

	std::stringstream bufferStream(buffer);
	std::string method;

	bufferStream >> method;
	if (method.empty() || !isMethodValid(method)) {
		responseStatus = METHOD_NOT_ALLOWED;
		if (methodImplemented(method))
			responseStatus = NOT_IMPLEMENTED;

		return false;
	}
	
	std::string url;
	bufferStream >> url;
	std::string decodedUrl = decodeUrl(url);
	if (decodedUrl.empty() || !isUrlValid(decodedUrl)) {
		responseStatus = BAD_REQUEST;
		return false;
	}

	if (decodedUrl.size() > URL_MAX_SIZE) {
		responseStatus = URI_TOO_LONG;
		return false;
	}

	std::string protocolVersion;
	bufferStream >> protocolVersion;
	if (protocolVersion.empty() || !isProtocolVersionValid(protocolVersion)) {
		responseStatus = HTTP_VERSION_NOT_SUPPORTED;
		return false;
	}

	httpReq.method = string2method(method);
	httpReq.uri = trim(url);
	httpReq.decodedUri = decodedUrl;
	httpReq.protocolVersion = trim(protocolVersion);

	return true;
}
