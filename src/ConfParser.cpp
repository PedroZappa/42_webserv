/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:00:04 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 10:05:33 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfParser.hpp"

// Constructors
ConfParser::ConfParser(void) {
}

ConfParser::ConfParser(const std::string &confFile) : _confFile(confFile) {
}

ConfParser::ConfParser(const ConfParser &copy) {
	*this = copy;
}

ConfParser::~ConfParser(void) {
}

// Operators
ConfParser &ConfParser::operator=(const ConfParser &src) {
	if (this == &src)
		return (*this);
	this->_confFile = src._confFile;
	// TODO: Copy servers vector
	return (*this);
}
