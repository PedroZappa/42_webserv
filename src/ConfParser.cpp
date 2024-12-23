/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:00:04 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 16:08:35 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfParser.hpp"
#include "../inc/Utils.hpp"

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
	// this->loadContext(serverBlocks);
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

/// @brief Gets the server blocks from the config file
/// @param file The config file to get the server blocks from
/// @return A vector of server blocks
std::vector<std::string> ConfParser::getServerBlocks(std::string &file) {
	std::vector<std::string> servers;
	std::string identifier;
	size_t start = 0;
	size_t endBlock = 0;
	size_t end = 0;

	while (file[start] != '\0') {             // Loop until end of file
		identifier = (file.substr(start, 6)); // Get server block identifier
		if (toLower(identifier) != "server")
			throw std::runtime_error("Invalid server block: no 'server' at "
									 "start");
		start += 6; // skip "server"

		while (std::isspace(file[start])) // Skip spaces
			++start;
		if (file[start] != '{')
			throw std::runtime_error("Invalid server block: no '{' at start");

		end = getBlockEnd(file, start);   // Get end of block
		while (std::isspace(file[start])) // Skip leading spaces
			++start;
		endBlock = end;
		while (std::isspace(file[endBlock])) // Skip trailing spaces
			--endBlock;

		// Add block to vector
		servers.push_back(file.substr(start, (endBlock - start + 1)));
		start = (endBlock + 1);                          // Skip '}'
		while (file[start] && std::isspace(file[start])) // Skip spaces
			++start;
	}
	return (servers);
}

size_t ConfParser::getBlockEnd(std::string &file, size_t start) {
	short depth = 0;
	while (file[start])
	{
		char c = file[start];
		if (c == '{')
			++depth;
		else if (c == '}')
			if (!--depth)
				return (start);
		++start;
	}
	throw std::runtime_error("Invalid server block: no '}' at end");
}


/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

std::vector<Server> ConfParser::getServers(void) const {
	return (this->_servers);
}
