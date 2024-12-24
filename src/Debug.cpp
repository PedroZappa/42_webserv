/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:46:43 by passunca          #+#    #+#             */
/*   Updated: 2024/12/24 10:19:04 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Debug.hpp"
#include "../inc/Error.h"
#include "../inc/Webserv.hpp"

const t_debug_msg debug_msg[14] = {
    {" INFO", SHOW_MSG, 5, "(ii) ", CYN},
    {" START", FSTART, 6, "(>>) ", BLU},
    {" ERROR", ERROR, 6, "(xx) ", RED},
    {" SUCCESS", SUCCESS, 8, "(xx) ", GRN},
    {" MALLOC", D_MALLOC, 7, "(**) ", RED},
    {" FREE", D_FREE, 5, "(**) ", RED},
    {" MALLOC_ERROR", MALLOC_ERROR, 13, "(xx) ", RED},
    {" MALLOC_NOT_ALLOC", MALLOC_NOT_ALLOCATED, 18, "(xx) ", CYN},
    {" FILE_ERROR", FILE_ERROR, 11, "(xx) ", RED},
    {" CHILD_EXIT", CHILD_EXIT, 11, "(xx) ", YEL},
    {" EXIT_FAILURE", EXIT_FAILURE, 14, "(xx) ", RED},
    {" FILE_NOT_FOUND", FILE_NOT_FOUND, 16, "(xx) ", RED},
    {" FILE_NOT_DELETED", FILE_NOT_DELETED, 17, "(xx) ", RED},
    {NULL, 0, 0, NULL, NULL}
};

void debugLocus(const std::string &functionName,
                int status,
                const std::string &customMessage) {
    // Find the appropriate debug message entry
    const t_debug_msg *msg_entry = NULL;
    for (int i = 0; debug_msg[i].msg != NULL; i++) {
        if (debug_msg[i].status == status) {
            msg_entry = &debug_msg[i];
            break;
        }
    }

    // Check if status is valid
    if (!msg_entry) {
        std::cerr << RED "Invalid status." NC << std::endl;
        return;
    }

    // Build the message using ostringstream
    std::ostringstream msg;
    msg << msg_entry->color                  // Color code
        << msg_entry->msg_header             // Header like "(ii) "
        << functionName                      // Function name
        << msg_entry->msg;                   // Status message

    if (!customMessage.empty()) {
        msg << " -> " << customMessage;
    }

    // Print to standard error and reset color
    std::cerr << msg.str() << NC << std::endl;
}
