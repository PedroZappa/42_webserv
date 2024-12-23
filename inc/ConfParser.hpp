/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 09:53:58 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 10:22:49 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

#include "Webserv.hpp"

class ConfParser {
  public:
	// Constructors
	ConfParser(void);
	ConfParser(const std::string &confFile);
	ConfParser(const ConfParser &copy);
	~ConfParser(void);
	
	// Operators
	ConfParser &operator=(const ConfParser &src);

	// Member Functions
	// TODO: Add Parsing Helper functions
  private:
	std::string _confFile;
	// TODO: Add Servers Vector (?)
};

#endif
