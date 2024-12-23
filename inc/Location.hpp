/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 18:07:38 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 19:11:37 by passunca         ###   ########.fr       */
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
	void setDirective(std::string &directive);

	// Getters

	// Directive Handlers

  private:
	std::string _root;
	std::vector<std::string> _index;
	// State _autoIndex;
	// long _maxBodySize;
	std::set<Method> _validMethods;
};

std::ostream &operator<<(std::ostream &os, const Location &ctx);

#endif
