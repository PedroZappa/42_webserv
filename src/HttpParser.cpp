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
 *
 * This file contains the implementation of the HttpRequestParser class, which
 * is responsible for parsing HTTP requests, including the request line,
 * headers, and body. It validates the request format and extracts necessary
 * information into an HttpRequest object.
 */

#include "../inc/HttpParser.hpp"
#include "../inc/Utils.hpp"
#include "../inc/Webserv.hpp"

/**
 * @brief Status of the HTTP response.
 *
 * This static variable holds the HTTP response status code, which is updated
 * during the parsing process to reflect the outcome of the request validation.
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

/**
 * @brief Parses the request line of an HTTP request.
 *
 * This function extracts and validates the HTTP method, URL, and protocol version
 * from the request line. It updates the HttpRequest object with the parsed data.
 *
 * @param httpReq The HttpRequest object to populate with parsed data.
 * @param buffer The string containing the request line to be parsed.
 * @return True if the request line is successfully parsed and valid, false otherwise.
 */
bool HttpRequestParser::getRequestLine(HttpRequest &httpReq,
									   const std::string &buffer) {
	if (std::isspace(buffer[0]))
		return false;

	std::stringstream bufferStream(buffer);
	std::string method;

	bufferStream >> method;
	if (method.empty() || !isMethodValid(method)) {
		responseStatus = METHOD_NOT_ALLOWED;
		if (isMethodImplemented(method))
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

/**
 * @brief Trims whitespace characters from both ends of a string.
 *
 * This function removes leading and trailing whitespace characters from the
 * input string and returns the trimmed result.
 *
 * @param str The string to be trimmed.
 * @return A new string with leading and trailing whitespace removed.
 */
std::string HttpRequestParser::trim(const std::string &str) {
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, (end - start + 1));
}

/**
 * @brief Trims null characters from the end of a string.
 *
 * This function removes trailing null characters from the input string,
 * effectively resizing it to exclude any null terminators.
 *
 * @param str The string to be trimmed of null characters.
 */
void HttpRequestParser::trimNull(std::string &str) {
	size_t pos = str.find_first_not_of("\0");

	if (pos == std::string::npos)
		str.clear();
	else
		str.resize(pos + 1);
}
