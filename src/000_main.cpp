/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   000_main.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:08:08 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 16:03:59 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Webserv.hpp"
#include "../inc/ConfParser.hpp"
#include "../inc/Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		// TODO: Implement Better Failure Handlers
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
	
	// TODO: Init Server Cluster
	
	// TODO: Setup Cluster
	
	// TODO: Run Cluster
	
	return (EXIT_SUCCESS);
}
