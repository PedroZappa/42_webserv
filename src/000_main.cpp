/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   000_main.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:08:08 by passunca          #+#    #+#             */
/*   Updated: 2024/12/25 21:11:45 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfParser.hpp"
#include "../inc/Debug.hpp"
#include "../inc/Server.hpp"
#include "../inc/Webserv.hpp"

int main(int argc, char **argv) {
#ifdef DEBUG
	debugLocus("webserv", __func__, FSTART, "Starting Webserv");
#endif
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

	// If you want to see locations for a specific server:
	if (!servers.empty()) {
		const std::map<std::string, Location> &locations =
			servers[0].getLocations();

		std::cerr << CYN "Locations for first server:" NC << std::endl;
		std::map<std::string, Location>::const_iterator it;
		for (it = locations.begin(); it != locations.end(); ++it) {
			std::cerr << "Location " << it->first << ": " << it->second
					  << std::endl;
		}
	}
	
#endif

	// TODO: Init Server Cluster

	// TODO: Setup Cluster

	// TODO: Run Cluster

	return (EXIT_SUCCESS);
}
