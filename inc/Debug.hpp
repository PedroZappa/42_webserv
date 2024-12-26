/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:46:52 by passunca          #+#    #+#             */
/*   Updated: 2024/12/24 17:54:55 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_HPP
#define DEBUG_HPP

/* ************************************************************************** */
/*                                 Libraries                                  */
/* ************************************************************************** */

#include "Webserv.hpp"

#include "Ansi.h"
#include "Error.h"
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

/* ************************************************************************** */
/*                              DEBUG Constants                               */
/* ************************************************************************** */

#ifndef _PRINT
#define _PRINT 1
#define _NOPRINT 0
#endif
#ifndef _SHOW_VARS_
#define _SHOW_VARS_ 0
#endif
#ifndef _SHOW_MATRIX_
#define _SHOW_MATRIX_ 1
#endif
#ifndef _SHOW_KEY_
#define _SHOW_KEY_ 0
#endif
#ifndef FSTART
#define FSTART 255
#endif
#ifndef FEND
#define FEND 254
#endif
#ifndef DEBUG_FREE
#define DEBUG_FREE 0
#endif
#ifndef DEBUG_ALL
#define DEBUG_ALL 1
#endif
#ifndef DEBUG_COLOR
#define DEBUG_COLOR 1
#endif
#ifndef DEBUG_ARRAY
#define DEBUG_ARRAY 1
#endif

/* ************************************************************************** */
/*                                 Structures                                 */
/* ************************************************************************** */

typedef struct s_debug_msg {
	const char *msg;
	int status;
	int len;
	const char *msg_header;
	const char *color;
} t_debug_msg;

/* ************************************************************************** */
/*                                   Macros                                   */
/* ************************************************************************** */

#define DEBUG_LOCUS(status, msg) \
	debugLocus(typeid(*this).name(), __func__, status, msg)

/* ************************************************************************** */
/*                            Function Prototypes                             */
/* ************************************************************************** */

void debugLocus(const std::string &className,
				const std::string &funcName,
				int status,
				const std::string &customMsg);
// void debugLocus(const std::string &funcName,
// 				int status,
// 				const std::string &customMessage = "");

/* ************************************************************************** */
/*                                 Templates                                  */
/* ************************************************************************** */

/// @brief Template to print containers
/// @tparam C The container type
/// @param funcName The name of the function
/// @param contName The name of the container
/// @param cont The container
template <typename C>
void showContainer(const std::string &funcName,
				   const std::string &contName,
				   const C &cont) {
	// Use constant SHOW_MSG (which should correspond to INFO status in debug_msg)
	debugLocus(
		typeid(cont).name(), funcName, SHOW_MSG, "Printing cont: " + contName);

	std::ostringstream output;

	for (typename C::const_iterator it = cont.begin(); it != cont.end(); ++it) {
		output << *it;
		typename C::const_iterator next = it;
		++next;
		if (next != cont.end()) {
			output << "\n";
		}
	}

	std::cerr << output.str() << std::endl;
}

#endif
