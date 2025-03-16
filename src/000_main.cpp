/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   000_main.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 12:29:52 by passunca          #+#    #+#             */
/*   Updated: 2025/03/09 12:29:59 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @defgroup main
 * @{
 *
 * @brief Entry point for the Webserv application.
 *
 * This module contains the main function which initializes and runs the Webserv
 * application. It handles configuration parsing, signal handling, and server
 * cluster management.
 *
 * @version 1.0
 * @date 2025-01-10
 *
 * @file 000_main.cpp
 *
 * @details
 * The main function validates input arguments, sets up signal handling, parses
 * the configuration file, initializes the server cluster, and starts handling
 * requests. It ensures graceful shutdown on receiving signals and logs the
 * application status.
 *
 * @section usage Usage
 *
 * To run the Webserv application, provide the configuration file as a
 * command-line argument:
 *
 * @code
 * ./webserv config_file
 * @endcode
 *
 * If no configuration file is provided, the default configuration file
 * "conf/default.conf" is used.
 *
 * @section signal_handling Signal Handling
 *
 * The application handles SIGINT signals to ensure a graceful shutdown of the
 * server cluster.
 *
 * @section logging Logging
 *
 * The application uses the Logger class to log information, warnings, and
 * errors. In DEBUG mode, additional debug information is logged.
 *
 * @section configuration Configuration
 *
 * The configuration is parsed using the ConfParser class, and the server
 * cluster is managed using the Cluster class. The application checks for
 * duplicate server configurations before starting the cluster.
 *
 * @}
 */

/**
 * @file 000_main.cpp
 * @brief Entry point for the Webserv application.
 *
 * This file contains the main function which initializes and runs the Webserv
 * application. It handles configuration parsing, signal handling, and server
 * cluster management.
 *
 * @version 1.0
 * @date 2025-01-10
 * @author passunca
 */

#include "../inc/Cluster.hpp"
#include "../inc/ConfParser.hpp"
#include "../inc/Logger.hpp"
#include "../inc/Server.hpp"
#include "../inc/Webserv.hpp"

/**
 * @brief Global pointer to the Cluster instance.
 *
 * This pointer is used to manage the server cluster and handle signals.
 */
Cluster *cluster = NULL;

void handleSignal(int code);

/**
 * @brief Main function for the Webserv application.
 *

 * This function initializes the Webserv application, parses the configuration
 * file, sets up the server cluster, and starts handling requests. It also
 * manages signal handling for graceful shutdown.

 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int Returns EXIT_SUCCESS on successful execution, EXIT_FAILURE
 * otherwise.
 */
int main(int argc, char **argv) {
    // Validate input arguments
    if (argc != 2) {
        Logger::error("Missing arguments! Usage: " + std::string(argv[0]) +
                      " config_file");
        return (EXIT_FAILURE);
    }

    Logger::info("Starting Webserv");

#ifdef DEBUG
    std::stringstream s;
    s << "MAX_CLIENTS: " << MAX_CLIENTS;
    Logger::debug(s.str());

#endif

    // Setup Signal (INT)
    signal(SIGINT, &handleSignal);

    // Parse Config
    std::string configFile = argc > 2 ? argv[1] : "conf/default.conf";
    ConfParser parser(configFile);


    std::vector<Server> servers;

    // Attempt to load Config
    try {
        parser.loadConf();
        servers = parser.getServers();
    } catch (std::exception &e) {
        Logger::error(e.what());
        return (EXIT_FAILURE);
    }


#ifdef DEBUG
    showContainer(__func__, "Loaded Servers", servers);
#endif

    try {
        // Init Server Cluster & Check for Duplicates
        cluster = new Cluster(servers);
        if (cluster->hasDuplicates()) {
            Logger::error("Server config has duplicates");
            return (EXIT_FAILURE);
        }

#ifdef DEBUG
        showContainer(__func__, "Initialized Cluster",
                      cluster->getVirtualServers());
#endif

        // Attemp to setup Cluster
        Logger::info("Setting up the cluster");
        cluster->setup();

        // Start running the cluster
        Logger::info("Ready to receive requests!");
        cluster->run();
        delete cluster;
    } catch (std::exception &e) {
        Logger::error(e.what());
        delete cluster;
        return (EXIT_FAILURE);
    }

    Logger::info("Webserv stopped");
    return (EXIT_SUCCESS);

}

/**
 * @brief Handles the SIGINT signal to stop the server cluster.
 *
 * This function is triggered when a SIGINT signal is received. It ensures that
 * the server cluster is stopped gracefully if it is active.
 *
 * @param code The signal code received.
 */
void handleSignal(int code) {
    (void)code;
    if (cluster == NULL) {
        Logger::warn("Signal caught, but the cluster is not active");
        std::exit(0);
    }

    std::cout << "\n";
    cluster->stop();

}
/** @} */
