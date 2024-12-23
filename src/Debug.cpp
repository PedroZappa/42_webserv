/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:46:43 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 19:32:43 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Debug.hpp"
#include "../inc/Webserv.hpp"

const std::map<DebugStatus, std::string> createStatusMessages() {
    std::map<DebugStatus, std::string> messages;
    messages[INFO] = " INFO";
    messages[START] = " START";
    messages[ERROR] = " ERROR";
    messages[SUCCESS] = " SUCCESS";
    messages[MALLOC] = " MALLOC";
    messages[FREE] = " FREE";
    messages[MALLOC_ERROR] = " MALLOC_ERROR";
    messages[MALLOC_NOT_ALLOCATED] = " MALLOC_NOT_ALLOCATED";
    messages[FILE_ERROR] = " FILE_ERROR";
    messages[CHILD_EXIT] = " CHILD_EXIT";
    messages[EXIT_FAIL] = " EXIT_FAILURE";
    messages[FILE_NOT_FOUND] = " FILE_NOT_FOUND";
    messages[FILE_NOT_DELETED] = " FILE_NOT_DELETED";
    return messages;
}

const std::map<DebugStatus, std::string> statusMessages = createStatusMessages();

const std::map<DebugStatus, std::string> createStatusPrefixes() {
    std::map<DebugStatus, std::string> prefixes;
    prefixes[INFO] = "(ii)";
    prefixes[START] = "(>>)";
    prefixes[ERROR] = "(xx)";
    prefixes[SUCCESS] = "(**)";
    prefixes[MALLOC] = "(**)";
    prefixes[FREE] = "(**)";
    prefixes[MALLOC_ERROR] = "(xx)";
    prefixes[MALLOC_NOT_ALLOCATED] = "(xx)";
    prefixes[FILE_ERROR] = "(xx)";
    prefixes[CHILD_EXIT] = "(xx)";
    prefixes[EXIT_FAIL] = "(xx)";
    prefixes[FILE_NOT_FOUND] = "(xx)";
    prefixes[FILE_NOT_DELETED] = "(xx)";
    return prefixes;
}

const std::map<DebugStatus, std::string> statusPrefixes = createStatusPrefixes();


void debugLocus(const std::string &functionName,
		   DebugStatus status,
		   const std::string &customMessage) {
	if (statusMessages.count(status) == 0) {
		std::cerr << "Invalid status." << std::endl;
		return;
	}

	// Build the message
	std::ostringstream msg;
	msg << statusPrefixes.at(status) << " " << functionName << " "
		<< statusMessages.at(status);

	if (!customMessage.empty()) {
		msg << " -> " << customMessage;
	}

	// Print to the standard error stream
	std::cerr << msg.str() << std::endl;
}
