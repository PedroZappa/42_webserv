/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:53:58 by passunca          #+#    #+#             */
/*   Updated: 2024/12/24 18:35:21 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

#include "Server.hpp"
#include "Webserv.hpp"

class Server;

class ConfParser {
  public:
	// Constructors
	ConfParser(void);
	ConfParser(const std::string &confFile);
	ConfParser(const ConfParser &copy);
	~ConfParser(void);

	// Operators
	ConfParser &operator=(const ConfParser &src);

	// Parsing
	void loadConf(void);
	static void removeComments(std::string &file);
	static void removeSpaces(std::string &file);
	static std::vector<std::string> tokenizer(std::string &line);

	std::vector<std::string> getServerBlocks(std::string &file);
	size_t getBlockEnd(std::string &file, size_t start);

	// Getters
	std::vector<Server> getServers(void) const;
	std::string getIdentifier(const std::string &str);

	// Setters
	void loadContext(std::vector<std::string> &serverBlocks);

	// Debug
	void debugServerLocations(size_t serverN, const std::string &route);

  private:
	std::string _confFile;
	std::vector<Server> _servers;
};

#endif
