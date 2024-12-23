/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:00:04 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 15:17:51 by passunca         ###   ########.fr       */
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
	confFile.close(); // Close file

	// Clean loaded file
	this->removeComments(fileContent);
	this->removeSpaces(fileContent);
	if (fileContent.empty()) // Handle empty file
		throw std::runtime_error("Config file is empty");

	// TODO: Handle quotes?

	// Get Server Blocks & load them
	std::vector<std::string> serverBlocks = this->getServerBlocks(fileContent);
	this->loadContext(serverBlocks);
}

/// @brief Removes comments from the config file
/// @param file The config file to remove comments from
void ConfParser::removeComments(std::string &file) {
	if (file.empty())
		return;
	size_t comment = file.find('#');
	while (comment != std::string::npos) {
		size_t eol = file.find('\n', comment);
		file.erase(comment, (eol - comment));
		comment = file.find('#');
	}
}

/// @brief Removes spaces from the config file
/// @param file The config file to remove spaces from
void ConfParser::removeSpaces(std::string &file) {
	if (file.empty())
		return;
	// remove leading spaces
	while (std::isspace(file[0]))
		file.erase(file.begin());
	if (file.empty())
		return;
	// remove trailing spaces
	while (std::isspace(file[(file.length() - 1)]))
		file.erase(file.end() - 1);
}

std::vector<std::string> ConfParser::getServerBlocks(std::string &file) {
	std::vector<std::string> servers;
	std::string identifier;
	size_t start = 0;
	size_t endBlock = 0;
	size_t end = 0;
	
	while (file[start] != '\0') {
		identifier = (file.substr(start, 6));
		if (toLower(identifier) != "server") 
	}
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

std::vector<Server> ConfParser::getServers(void) const {
	return (this->_servers);
}
