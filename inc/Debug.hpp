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

#include "Server.hpp"
#include "Webserv.hpp"

/* ************************************************************************** */
/*                                   Enums                                    */
/* ************************************************************************** */

typedef enum e_debug {
	ERROR,
	FSTART,
	FEND,
	SUCCESS,
	SHOW_MSG,
} t_debug;

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
	typename C::const_iterator it;

	for (it = cont.begin(); it != cont.end(); ++it) {
		output << *it;
		typename C::const_iterator next = it;
		++next;
		if (next != cont.end())
			output << "\n";
	}

	std::cerr << output.str() << std::endl;
}

#endif
