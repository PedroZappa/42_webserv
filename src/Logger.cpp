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

#include "../inc/Logger.hpp"

const std::string Logger::currentTime()
{
	std::time_t tm = std::time(NULL);
	std::tm *now = localtime(&tm);
	
	std::stringstream res;
	res
	<< std::setfill('0') << std::setw(2) << now->tm_hour << ":"
	<< std::setfill('0') << std::setw(2) << now->tm_min << ":"
	<< std::setfill('0') << std::setw(2) << now->tm_sec;
	return res.str();
}

void Logger::printLog(const std::string &header, const std::string &message, bool err)
{
	std::ostream &out = err ? std::cerr : std::cout; 
	
	out
	<< BBLK << currentTime()
	<< NC" " << std::left << std::setfill(' ') << std::setw(12) << header
	<< NC" " << message
	<< "\n";
}

void Logger::info(const std::string &message)
{
	printLog(BGRN"INFO", message, false);
}

void Logger::debug(const std::string &message)
{
#ifndef DEBUG
	return;
#endif
	printLog(BBLU"DEBUG", message, false);	
}

void Logger::debug(const std::string &funcName,
				   const std::string &message)
{
	debug("", funcName, message);
}

void Logger::debug(const std::string &className,
				   const std::string &funcName,
				   const std::string &message)
{
#ifndef DEBUG
	return;
#endif
	std::string func = "[" + (className != "" ? className + "::" : "") + funcName + "]";

	printLog(BBLU"DEBUG", func + " " + message, false);
}						

void Logger::warn(const std::string &message)
{
	printLog(YEL"WARN", message, false);
}

void Logger::error(const std::string &message)
{
	printLog(RED"ERROR", message, true);
}
