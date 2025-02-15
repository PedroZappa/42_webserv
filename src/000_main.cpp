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
#include "../inc/Server.hpp"
#include "../inc/Webserv.hpp"
#include "../inc/Utils.hpp"
#include "../inc/Logger.hpp"

int main(int argc, char **argv)
{
	// Validate input arguments
	if (argc != 2)
	{
		Logger::error("Missing arguments! Usage: " + std::string(argv[0]) + " config_file");
		return (EXIT_FAILURE);
	}
	
	Logger::info("Starting Webserv");
	std::stringstream s; s << "MAX_CLIENTS: " << getMaxClients(); 
	Logger::debug(s.str());

	// TODO: Setup Signal Handling (SIGINT)

	// Parse Config
	ConfParser parser("conf/default.conf");
	if (argc == 2)
		parser = ConfParser(argv[1]);

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

	// Init Server Cluster & Check for Duplicates
	Cluster cluster(servers);
	if (cluster.hasDuplicates())
	{
		Logger::error("Server config has duplicates");
		return (EXIT_FAILURE);
	}

#ifdef DEBUG
	showContainer(__func__, "Initialized Cluster", cluster.getVirtualServers());
#endif

	// Attemp to setup Cluster
	try
	{
		cluster.setup();
	}
	catch (std::exception &e)
	{
		Logger::error(e.what());
		return (EXIT_FAILURE);
	}

	// TODO: Run Cluster

#ifdef DEBUG
	Logger::debug("Webserv stopped");
#endif

	return (EXIT_SUCCESS);
}
