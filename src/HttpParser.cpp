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
 *
 * The parser handles various HTTP methods and ensures compliance with HTTP/1.1
 * protocol standards. It checks for malformed requests and updates the response
 * status accordingly.
 *
 * @author passunca
 * @date 2025-03-08
 */

#include "../inc/HttpParser.hpp"
#include "../inc/Utils.hpp"
#include "../inc/Webserv.hpp"

/**
 * @brief Status of the HTTP response.
 *
 * This static variable holds the HTTP response status code, which is updated
 * during the parsing process to reflect the outcome of the request validation.
 * It is initialized to OK and modified as needed during parsing.
 */
static int responseStatus = OK;

/**
 * @class HttpRequestParser
 * @brief A class for parsing HTTP requests.
 *
 * The HttpRequestParser class provides methods to parse HTTP requests,
 * including the request line, headers, and body. It validates the request
 * format and extracts necessary information into an HttpRequest object.
 *
 * The parser handles various HTTP methods and ensures compliance with HTTP/1.1
 * protocol standards. It checks for malformed requests and updates the response
 * status accordingly.
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
 * The function checks for valid HTTP methods, decodes the URL, and ensures the
 * protocol version is supported.
 *
 * The method is designed to handle edge cases such as unsupported methods, invalid
 * URLs, and incorrect protocol versions, updating the response status to reflect
 * any issues encountered during parsing.
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
 * @brief Parses the header fields of an HTTP request.
 *
 * This function extracts key-value pairs from the header section of the HTTP
 * request. It validates the presence of a colon to separate keys and values and
 * handles special cases for date-related headers.
 *
 * @param httpReq The HttpRequest object to populate with parsed header data.
 * @param headers The string containing the header fields to be parsed.
 * @return True if the headers are successfully parsed and valid, false
 * otherwise.
 */
bool HttpRequestParser::getHeaderFields(HttpRequest &httpReq,
										const std::string &headers) {
	// check for colon
	size_t colonPos = headers.find_first_of(':');
	if (colonPos == std::string::npos) {
		responseStatus = BAD_REQUEST;
		return false;
	}

	// Extract key value pair
	std::string key = toLower(trim(headers.substr(0, colonPos)));
	std::string value = trim(headers.substr(colonPos + 1));
	if (key.empty() || value.empty()) {
		responseStatus = BAD_REQUEST;
		return false;
	}

	std::stringstream ss(value);
	std::string val;

	if ((key == "date") || (key == "if-modified-since") ||
		(key == "last-modified")) {
		httpReq.headers.insert(std::pair<std::string, std::string>(key, value));
		return true;
	}
	while (std::getline(ss, val, ',')) {
		val.erase(val.find_last_not_of(" \t\r\n") + 1);
		val.erase(0, val.find_first_not_of(" \t\r\n"));

		if (!val.empty()) // Ingnore empty fields
			httpReq.headers.insert(std::pair<std::string, std::string>(key, val));
	}
	return true;
}

/**
 * @brief Parses the query parameters from the URI of an HTTP request.
 *
 * This function extracts query parameters from the URI of the HttpRequest object.
 * It identifies the query string, splits it into key-value pairs, and populates
 * the queryParams map in the HttpRequest object with these pairs.
 *
 * @param httpReq The HttpRequest object whose URI will be parsed for query parameters.
 */
void HttpRequestParser::parseQueries(HttpRequest &httpReq) {
	std::size_t delimPos = httpReq.uri.find('?');
	if (delimPos == std::string::npos)
		return;

	std::string query = httpReq.uri.substr(delimPos + 1); // Findf query
	httpReq.uri = httpReq.uri.substr(0, delimPos);        // Extract query
	std::stringstream ss(query);
	std::string pair;

	while (std::getline(ss, pair, '&')) {
		size_t equalPos = pair.find('=');
		std::string key;
		std::string value;
		if (equalPos == std::string::npos)
			key = pair; // empty
		else {
			key = pair.substr(0, equalPos);
			value = pair.substr(equalPos + 1);
		}
		trim(key);
		trim(value);

		if (!key.empty())
			httpReq.queryParams.insert(
				std::pair<std::string, std::string>(key, value));
	}
	return;
}

/* ************************************************************************** */
/*                                  Checking                                  */
/* ************************************************************************** */

/**
 * @brief Validates the HTTP method.
 *
 * This function checks if the provided HTTP method is one of the supported
 * methods: GET, POST, or DELETE.
 *
 * @param method The HTTP method to validate.
 * @return True if the method is valid and supported, false otherwise.
 */
bool HttpRequestParser::isMethodValid(const std::string &method) {
	if (method == "GET" || method == "POST" || method == "DELETE")
		return true;
	return false;
}

/**
 * @brief Checks if the HTTP method is implemented.
 *
 * This function determines whether the provided HTTP method is one of the
 * implemented methods: PUT, HEAD, OPTIONS, or PATCH.
 *
 * @param method The HTTP method to check for implementation.
 * @return True if the method is implemented, false otherwise.
 */
bool HttpRequestParser::isMethodImplemented(const std::string &method) {
	if (method == "PUT" || method == "HEAD" || method == "OPTIONS" ||
		method == "PATCH")
		return true;
	return false;
}

/**
 * @brief Validates the format of a given URL.
 *
 * This function checks if the provided URL adheres to the expected format
 * for HTTP requests. It ensures that the URL starts with a '/', does not
 * contain fragment identifiers, contains at most one '?', and has a valid
 * number of '=' and '&' characters when a '?' is present.
 *
 * @param url The URL string to validate.
 * @return True if the URL is valid according to the specified rules, false otherwise.
 *
 * @details The validation rules are as follows:
 * - The URL must start with a '/' character.
 * - Fragment identifiers (indicated by '#') are not allowed.
 * - The URL can contain at most one '?' character.
 * - If a '?' is present, the number of '=' characters must match the number
 *   of '&' characters plus one.
 */
bool HttpRequestParser::isUrlValid(const std::string &url) {
	if (url[0] != '/')
		return false;
	// No fragment urls allowed
	if (url.find("#") != std::string::npos)
		return false;
	// Check for a single ?
	if (std::count(url.begin(), url.end(), '?') > 1)
		return false;
	// The number of '=' must match the number of '&' plus one when a '?' is present.
	if ((url.find('?') != std::string::npos) &&
		(std::count(url.begin(), url.end(), '=') !=
		 std::count(url.begin(), url.end(), '&') + 1))
		return false;
	return true;
}

/**
 * @brief Validates the protocol version of an HTTP request.
 *
 * This function checks if the provided protocol version is one of the supported
 * versions: HTTP/1.1, HTTP/1.0, or HTTP/0.9.
 *
 * @param protocolVersion The protocol version string to validate.
 * @return True if the protocol version is valid and supported, false otherwise.
 */
bool HttpRequestParser::isProtocolVersionValid(const std::string &protocolVersion) {
	if ((protocolVersion == "HTTP/1.1") || (protocolVersion == "HTTP/1.0") ||
		(protocolVersion == "HTTP/0.9"))
		return true;
	return false;
}

/* ************************************************************************** */
/*                                  Decoding                                  */
/* ************************************************************************** */

/**
 * @brief Decodes a percent-encoded URL string.
 *
 * This function converts percent-encoded characters in a URL to their ASCII
 * equivalents. It processes the input string and returns a decoded version.
 *
 * @param encoded The percent-encoded URL string to decode.
 * @return A decoded string with percent-encoded characters converted to ASCII.
 */
std::string HttpRequestParser::decodeUrl(const std::string &encoded) {
	std::ostringstream decoded;
	for (size_t i = 0; i < encoded.length(); ++i) {
		if ((encoded[i] == '%') && ((i + 2) < encoded.length()) &&
			std::isxdigit(encoded[i + 1]) && std::isxdigit(encoded[i + 2])) {
			int value;
			std::istringstream ss(encoded.substr((i + 1), 2));
			ss >> std::hex >> value;
			decoded << static_cast<char>(value);
			i += 2;
		} else
			decoded << encoded[i];
	}
	return decoded.str();
}

/* ************************************************************************** */
/*                                  Trimming */
/* ************************************************************************** */

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

/* ************************************************************************** */
/*                                 Overloads                                  */
/* ************************************************************************** */

std::ostream &operator<<(std::ostream &os, const HttpRequest &httpReq) {
	os << BGRN "HTTP Request:" NC << std::endl;
	os << BYEL "Method: " NC << method2string(httpReq.method) << std::endl;
	os << BYEL "URI: " NC << httpReq.uri << std::endl;
	os << BYEL "Protocol Version: " NC << httpReq.protocolVersion << std::endl;
	os << BYEL "Headers: " NC << std::endl;
	std::multimap<std::string, std::string>::const_iterator it;
	for (it = httpReq.headers.begin(); it != httpReq.headers.end(); ++it)
		os << "\t" << it->first << ": " << it->second << std::endl;
	os << BYEL "Query Fields: " NC << std::endl;
	std::multimap<std::string, std::string>::const_iterator itq;
	for (it = httpReq.queryParams.begin(); itq != httpReq.queryParams.end();
		 ++itq)
		os << "\t" << it->first << ": " << it->second << std::endl;
	os << BYEL "Body: " NC << std::endl << httpReq.body << std::endl;

	return (os);
}
