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

#include "../inc/Webserv.hpp"
#include "../inc/Logger.hpp"
#include "../inc/ConfParser.hpp"
#include "../inc/Server.hpp"
#include "../inc/Cluster.hpp"

Cluster *cluster = NULL;
void handleSignal(int code);

int main(int argc, char **argv)
{
	// Validate input arguments
	if (argc != 2)
	{
		Logger::error("Missing arguments! Usage: " + std::string(argv[0]) + " config_file");
		return (EXIT_FAILURE);
	}
	
	Logger::info("Starting Webserv");

#ifdef DEBUG
	std::stringstream s; s << "MAX_CLIENTS: " << MAX_CLIENTS; 
	Logger::debug(s.str());
#endif

	// Setup Signal (INT)
	signal(SIGINT, &handleSignal);

	// Parse Config
	std::string configFile = argc > 2 ? argv[1] : "conf/default.conf"; 
	ConfParser parser(configFile);

	std::vector<Server> servers;

	// Attempt to load Config
	try
	{
		parser.loadConf();
		servers = parser.getServers();
	}
	catch (std::exception &e)
	{
		Logger::error(e.what());
		return (EXIT_FAILURE);
	}

#ifdef DEBUG
	showContainer(__func__, "Loaded Servers", servers);
#endif

	try
	{
		// Init Server Cluster & Check for Duplicates
		cluster = new Cluster(servers);
		if (cluster->hasDuplicates())
		{
			Logger::error("Server config has duplicates");
			return (EXIT_FAILURE);
		}

#ifdef DEBUG
		showContainer(__func__, "Initialized Cluster", cluster.getVirtualServers());
#endif

		// Attemp to setup Cluster
		Logger::info("Setting up the cluster");
		cluster->setup();

		// Start running the cluster
		Logger::info("Ready to receive requests!");
		cluster->run();
		delete cluster;
	}
	catch (std::exception &e)
	{
		Logger::error(e.what());
		delete cluster;
		return (EXIT_FAILURE);
	}

	Logger::info("Webserv stopped");
	return (EXIT_SUCCESS);
}

void handleSignal(int code)
{
	(void)code;
	if (cluster == NULL)
	{
		Logger::warn("Signal caught, but the cluster is not active");
		std::exit(0);
	}

	std::cout << "\n";
	cluster->stop();
}
