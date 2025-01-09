/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:46:43 by passunca          #+#    #+#             */
/*   Updated: 2024/12/25 21:33:24 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Debug.hpp"

const t_debug_msg debug_msg[6] = {
    {" INFO", SHOW_MSG, 5, "(ii) ", BBLU},
    {" START", FSTART, 6, "(>>) ", GRN},
    {" END", FEND, 4, "(<<) ", MAG},
    {" ERROR", ERROR, 6, "(xx) ", RED},
    {" SUCCESS", SUCCESS, 8, "(🖔🖔)", BGRN},
    {NULL, 0, 0, NULL, NULL}
};

void debug(const std::string &className,
                const std::string &funcName,
                int status,
                const std::string &customMsg) {
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
        << "[" << className << "::" << funcName << "] " // Class and function name
        << msg_entry->msg;                   // Status message
    if (!customMsg.empty()) {
        msg << " -> " << customMsg;
    }
    // Print to standard error and reset color
    std::cerr << msg.str() << NC << std::endl;
}
