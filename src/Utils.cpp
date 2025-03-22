/**
 * @defgroup Utils Utility Functions
 * @{
 *
 * This module provides utility functions for string manipulation, HTTP method
 * conversions, and time parsing. It includes functions to convert strings to
 * lowercase, map HTTP methods between enums and strings, and parse HTTP date
 * strings to time_t values.
 *
 * @version 1.0
 */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 15:20:31 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 18:36:39 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Utils.hpp"
#include "../inc/Webserv.hpp"

/* ************************************************************************** */
/*                                Convertions                                 */
/* ************************************************************************** */

/**
 * @brief Converts a string to lowercase.
 *
 * This function takes a string and converts all its characters to lowercase.
 *
 * @param str The input string to be converted.
 * @return A new string with all characters in lowercase.
 */
std::string toLower(const std::string &str) {
	std::string lower = str;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return (lower);
}

/**
 * @brief Converts an HTTP method enum to its string representation.
 *
 * This function maps an HTTP method enum value to its corresponding string
 * representation, such as "GET", "POST", etc.
 *
 * @param method The HTTP method enum to be converted.
 * @return A string representing the HTTP method.
 */
std::string method2string(Method method) {
	switch (method) {
	case GET:
		return ("GET");
	case POST:
		return ("POST");
	case DELETE:
		return ("DELETE");
	case HEAD:
		return ("HEAD");
	case PUT:
		return ("PUT");
	case CONNECT:
		return ("CONNECT");
	case OPTIONS:
		return ("OPTIONS");
	case TRACE:
		return ("TRACE");
	case PATCH:
		return ("PATCH");
	default:
		return ("UNKNOWN");
	}
}

/**
 * @brief Converts a string to its corresponding HTTP method enum.
 *
 * This function maps a string representation of an HTTP method to its
 * corresponding enum value. If the string does not match any known method,
 * it returns UNKNOWN.
 *
 * @param str The string representation of the HTTP method.
 * @return The corresponding HTTP method enum.
 */
Method string2method(const std::string &str) {
	if (str == "GET")
		return (GET);
	else if (str == "POST")
		return (POST);
	else if (str == "DELETE")
		return (DELETE);
	else if (str == "HEAD")
		return (HEAD);
	else if (str == "PUT")
		return (PUT);
	else if (str == "CONNECT")
		return (CONNECT);
	else if (str == "OPTIONS")
		return (OPTIONS);
	else if (str == "TRACE")
		return (TRACE);
	else if (str == "PATCH")
		return (PATCH);
	else
		return (UNKNOWN);
}

/**
 * @brief Converts an error code to its string representation.
 *
 * This function takes an error code of type ErrCodes and converts it to a string
 * representation using the number2string function.
 *
 * @param code The error code to be converted.
 * @return A string representing the error code.
 */
std::string err2string(ErrCodes code) {
    return number2string<int>(code);
}

/* ************************************************************************** */
/*                                    Time                                    */
/* ************************************************************************** */

static int getMonthFromStr(const std::string &month_str);

/**
 * @brief Parses an HTTP date string and converts it to time_t.
 *
 * This function takes a date string in the format "Wed, 11 Oct 2015 07:42:00 GMT"
 * and converts it to a time_t value representing the UTC time.
 *
 * @param httpTime The HTTP date string to be parsed.
 * @return The corresponding time_t value, or -1 if parsing fails.
 */
time_t getTime(const std::string &httpTime) {
	struct tm t;
	memset(&t, 0, sizeof(t));

	char monthStr[4];
	int year, day, hour, minute, second;

	// date ex.: "Wed, 11 Oct 2015 07:42:00 GMT"
	if (sscanf(httpTime.c_str(),
			   "%*3s, %d %3s %d %d:%d:%d GMT",
			   &day,
			   monthStr,
			   &year,
			   &hour,
			   &minute,
			   &second) != 6)
		return (-1);


	int month = getMonthFromStr(monthStr);
	if (month == -1)
		return (-1);

	// Set tm struct
	// https://en.cppreference.com/w/cpp/chrono/c/strftime
	strptime(monthStr, "%b", &t);
	t.tm_year = year - 1900;
	t.tm_mon = month;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = minute;
	t.tm_sec = second;
	t.tm_isdst = 0; // Not accounting for daylight savings

	// convert to UTC time  (using timegm to avoid timezone tweaks)
	time_t utc = timegm(&t);
	return (utc);
}

/**
 * @brief Converts a three-letter month abbreviation to its corresponding month index.
 *
 * This function maps a three-letter month abbreviation (e.g., "Jan", "Feb") to
 * its corresponding zero-based month index (e.g., 0 for January, 1 for February).
 *
 * @param month_str The three-letter month abbreviation.
 * @return The zero-based month index, or -1 if the abbreviation is invalid.
 */
static int getMonthFromStr(const std::string &month_str) {
	const char *months[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	for (int i = 0; i < 12; ++i)
		if (month_str == months[i])
			return i;
	return (-1);
}

/**
 * @brief Generates the current date and time in HTTP-date format.
 *
 * This function retrieves the current time, converts it to GMT, and formats it
 * as a string in the HTTP-date format: "Day, DD Mon YYYY HH:MM:SS GMT".
 *
 * @return A string representing the current date and time in HTTP-date format.
 */
std::string getHttpDate() {
	std::time_t now = std::time(0);
	std::tm *gmt = std::gmtime(&now);
	char buf[35];
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return (std::string(buf));
}

/** @} */

