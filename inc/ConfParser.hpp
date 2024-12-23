/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:53:58 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 14:36:12 by passunca         ###   ########.fr       */
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

	// Getters 
	std::vector<Server> getServers(void) const;

	// TODO: Add Parsing Helper functions
  private:
	std::string _confFile;
	// TODO: Add Servers Vector (?)
};

#endif
