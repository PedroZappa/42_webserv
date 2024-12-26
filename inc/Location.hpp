/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:07:38 by passunca          #+#    #+#             */
/*   Updated: 2024/12/26 11:56:40 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

class Location {
  public:
	// Constructors
	Location(void);
	Location(const Location &copy);
	~Location(void);

	// Operators
	Location &operator=(const Location &src);

	// Setup
	void initDirectiveMap(void);
	void setDirective(std::string &directive);

	// Getters
	long getCliMaxBodySize(void) const;
	std::map<short, std::string> getErrorPage(void) const;
	std::string getRoot(void) const;

	// Directive Handlers

  private:
	std::string _root;
	std::vector<std::string> _index;
	// State _autoIndex;
	long _cliMaxBodySize;
	std::map<short, std::string> _errorPage;
	std::set<Method> _validMethods;

	typedef void (Location::*DirHandler)(std::vector<std::string> &d);
	std::map<std::string, DirHandler> _directiveMap;
};

std::ostream &operator<<(std::ostream &os, const Location &ctx);

#endif
