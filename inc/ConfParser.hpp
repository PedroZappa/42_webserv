/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:53:58 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 14:51:32 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

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
	void removeComments(std::string &file);
	void removeSpaces(std::string &file);

	// Getters
	std::vector<Server> getServers(void) const;

	// TODO: Add Parsing Helper functions

  private:
	std::string _confFile;
	std::vector<Server> _servers;
};

#endif
