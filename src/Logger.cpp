/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gfragoso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/15 00:27:10 by gfragoso          #+#    #+#             */
/*   Updated: 2025/02/15 18:38:27 by gfragoso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/** 
* @defgroup debug Debug Functions
* @{
*/

#include "../inc/Logger.hpp"

/* ************************************************************************** */
/*                                    Time                                    */
/* ************************************************************************** */

/// @brief Get current time
/// @return Current time
const std::string Logger::currentTime() {
	std::time_t tm = std::time(NULL);
	std::tm *now = localtime(&tm);

	std::stringstream res;
	res << std::setfill('0') << std::setw(2) << now->tm_hour << ":"
		<< std::setfill('0') << std::setw(2) << now->tm_min << ":"
		<< std::setfill('0') << std::setw(2) << now->tm_sec;
	return res.str();
}

/* ************************************************************************** */
/*                                  Loggers                                   */
/* ************************************************************************** */

/// @brief Print a log
/// @param header The header of the log
/// @param message The message of the log
/// @param err True if the log is an error
void Logger::printLog(const std::string &header,
					  const std::string &message,
					  bool err) {
	std::ostream &out = err ? std::cerr : std::cout;

	out << BBLK << currentTime() << NC " " << std::left << std::setfill(' ')
		<< std::setw(12) << header << NC " " << message << "\n";
}

/// @brief Print an info log
void Logger::info(const std::string &message) {
	printLog(BGRN "INFO", message, false);
}

/// @brief Print a debug log
/// @param message The message of the log
void Logger::debug(const std::string &message) {
#ifndef DEBUG
	return;
#endif
	printLog(BBLU "DEBUG", message, false);
}

/* ************************************************************************** */
/*                                   Debug                                    */
/* ************************************************************************** */

/// @brief Print a debug log
/// @param funcName The name of the function
/// @param message The message of the log
void Logger::debug(const std::string &funcName, const std::string &message) {
	debug("", funcName, message);
}

/// @brief Print a debug log
/// @param funcName The name of the function
/// @param message The message of the log
/// @param className The name of the class
void Logger::debug(const std::string &className,
				   const std::string &funcName,
				   const std::string &message) {
#ifndef DEBUG
	return;
#endif
	std::string func =
		"[" + (className != "" ? className + "::" : "") + funcName + "]";

	printLog(BBLU "DEBUG", func + " " + message, false);
}

/// @brief Print a warning log
/// @param message The message of the log
void Logger::warn(const std::string &message) {
	printLog(YEL "WARN", message, false);
}

/// @brief Print a error log
/// @param message The message of the log
void Logger::error(const std::string &message) {
	printLog(RED "ERROR", message, true);
}
