/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:46:52 by passunca          #+#    #+#             */
/*   Updated: 2024/12/24 10:16:04 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_HPP
#define DEBUG_HPP

/* ************************************************************************** */
/*                                 Libraries                                  */
/* ************************************************************************** */

#include "Error.h"
#include "Ansi.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

/* ************************************************************************** */
/*                              DEBUG Constants                               */
/* ************************************************************************** */

# ifndef _PRINT
#  define _PRINT 1
#  define _NOPRINT 0
# endif
# ifndef _SHOW_VARS_
#  define _SHOW_VARS_ 0
# endif
# ifndef _SHOW_MATRIX_
#  define _SHOW_MATRIX_ 1
# endif
# ifndef _SHOW_KEY_
#  define _SHOW_KEY_ 0
# endif
# ifndef MY_DEBUG
#  define MY_DEBUG 0
# endif
# ifndef MY_DEBUG_COLOR
#  define MY_DEBUG_COLOR 0
# endif
# ifndef FSTART
#  define FSTART 255
# endif
# ifndef MY_DEBUGMALLOC_
#  define MY_DEBUGMALLOC_ 0
# endif
# ifndef _MLX_TEST_
#  define _MLX_TEST_ 1
# endif
# ifndef _MOUSE_TEST_
#  define _MOUSE_TEST_ 1
# endif
# ifndef DEBUG_FREE
#  define DEBUG_FREE 0
# endif
# ifndef DEBUG_ALL
#  define DEBUG_ALL 1
# endif
# ifndef DEBUG_COLOR
#  define DEBUG_COLOR 1
# endif
# ifndef DEBUG_ARRAY
#  define DEBUG_ARRAY 1
# endif

/* ************************************************************************** */
/*                                 Structures                                 */
/* ************************************************************************** */

typedef struct s_debug_msg
{
	const char		*msg;
	int				status;
	int				len;
	const char		*msg_header;
	const char			*color;
}				t_debug_msg;


void debugLocus(const std::string &functionName,
				int status,
				const std::string &customMessage = "");

// // Template to print any container
// template <typename C>
// void showContainer(const std::string &functionName,
//                    const std::string &containerName,
//                    const C &container) {
//     debugLocus(functionName, INFO, "Printing container: " + containerName);
//
//     std::cerr << "[";
//     for (typename C::const_iterator it = container.begin(); it != container.end(); ++it) {
//         std::cerr << *it;
//         typename C::const_iterator next = it;
//         ++next;
//         if (next != container.end()) {
//             std::cerr << ", ";
//         }
//     }
//     std::cerr << "]" << std::endl;
// }
// Template to print any container
template <typename C>
void showContainer(const std::string &functionName,
                  const std::string &containerName,
                  const C &container) {
    // Use constant SHOW_MSG (which should correspond to INFO status in debug_msg)
    debugLocus(functionName, SHOW_MSG, "Printing container: " + containerName);
    
    std::ostringstream output;
    output << CYN << "[";  // Using CYN color for consistency with INFO messages
    
    for (typename C::const_iterator it = container.begin(); it != container.end(); ++it) {
        output << *it;
        typename C::const_iterator next = it;
        ++next;
        if (next != container.end()) {
            output << ", ";
        }
    }
    output << "]" << NC;  // Reset color at the end
    
    std::cerr << output.str() << std::endl;
}

#endif
