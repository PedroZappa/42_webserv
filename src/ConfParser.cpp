/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:00:04 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 14:53:31 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfParser.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */
ConfParser::ConfParser(void) {
}

ConfParser::ConfParser(const std::string &confFile) : _confFile(confFile) {
}

ConfParser::ConfParser(const ConfParser &copy) {
	*this = copy;
}

ConfParser::~ConfParser(void) {
}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

ConfParser &ConfParser::operator=(const ConfParser &src) {
	if (this == &src)
		return (*this);
	this->_confFile = src._confFile;
	// TODO: Copy servers vector
	return (*this);
}

/* ************************************************************************** */
/*                                  Parsing                                   */
/* ************************************************************************** */

/// @brief Loads the config file and parses it into a vector of servers
/// @throws std::runtime_error if the config file cannot be opened
void ConfParser::loadConf(void) {
	std::ifstream confFile(this->_confFile.c_str()); // Open config file
	if (!confFile.is_open())
		throw std::runtime_error("Failed to open config file " + _confFile);

	// Read file
	std::string fileContent;
	std::getline(confFile, fileContent, '\0');
	confFile.close();

	// Clean file
	this->removeComments(fileContent);
	this->removeSpaces(fileContent);
	
	if (fileContent.empty()) // Handle empty file
		throw std::runtime_error("Config file is empty");
	
	// TODO: Parse file
}

/// @brief Removes comments from the config file
/// @param file The config file to remove comments from
void ConfParser::removeComments(std::string &file) {
	// TODO: Remove comments
}

/// @brief Removes spaces from the config file
/// @param file The config file to remove spaces from
void ConfParser::removeSpaces(std::string &file) {
	// TODO: Remove spaces
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

std::vector<Server> ConfParser::getServers(void) const {
	return (this->_servers);
}
