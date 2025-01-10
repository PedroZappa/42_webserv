/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   000_main.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:08:08 by passunca          #+#    #+#             */
/*   Updated: 2025/01/10 17:35:43 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Cluster.hpp"
#include "../inc/ConfParser.hpp"
#include "../inc/Debug.hpp"
#include "../inc/Server.hpp"
#include "../inc/Webserv.hpp"

int main(int argc, char **argv) {
#ifdef DEBUG
	debug("webserv", __func__, FSTART, "Starting Webserv");
	std::cout << "MAX_CLIENTS: " << getMaxClients() << "" << std::endl;
#endif

	// Validate input arguments
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " config_file" << std::endl;
		return (EXIT_FAILURE);
	}
	// TODO: Setup Signal Handling (SIGINT)

	// Parse Config
	ConfParser parser("conf/default.conf");
	if (argc == 2)
		parser = ConfParser(argv[1]);

	// Declare servers vector
	std::vector<Server> servers;

	// Attempt to load Config
	try {
		parser.loadConf();
		servers = parser.getServers();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

#ifdef DEBUG
	showContainer(__func__, "Loaded Servers", servers);
#endif

	// Init Server Cluster & Check for Duplicates
	Cluster cluster(servers);
	if (cluster.hasDuplicates()) {
		std::cerr << "Error: Server config has duplicates" << std::endl;
		return (EXIT_FAILURE);
	}

	// Attemp tp Setup Cluster
	try {
		std::cout << "Setting up Cluster" << std::endl;
		cluster.setup();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	std::cout << "Cluster Setup" << std::endl;

	// TODO: Run Cluster

	return (EXIT_SUCCESS);
}
