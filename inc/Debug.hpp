/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:46:52 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 19:32:55 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
class Container;

// Debugging options
enum DebugStatus {
	INFO,
	START,
	ERROR,
	SUCCESS,
	MALLOC,
	FREE,
	MALLOC_ERROR,
	MALLOC_NOT_ALLOCATED,
	FILE_ERROR,
	CHILD_EXIT,
	EXIT_FAIL,
	FILE_NOT_FOUND,
	FILE_NOT_DELETED
};

const std::map<DebugStatus, std::string> createStatusMessages();

const std::map<DebugStatus, std::string> createStatusPrefixes();

void debugLocus(const std::string &functionName,
				DebugStatus status,
				const std::string &customMessage = "");

// Template to print any container
template <typename C>
void showContainer(const std::string &functionName,
                   const std::string &containerName,
                   const C &container) {
    debugLocus(functionName, INFO, "Printing container: " + containerName);

    std::cerr << "[";
    for (typename C::const_iterator it = container.begin(); it != container.end(); ++it) {
        std::cerr << *it;
        typename C::const_iterator next = it;
        ++next;
        if (next != container.end()) {
            std::cerr << ", ";
        }
    }
    std::cerr << "]" << std::endl;
}
#endif
