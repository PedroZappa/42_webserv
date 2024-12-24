/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:00:04 by passunca          #+#    #+#             */
/*   Updated: 2024/12/24 18:50:40 by passunca         ###   ########.fr       */
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
	this->_servers = src._servers;
	return (*this);
}

/* ************************************************************************** */
/*                                  Parsing                                   */
/* ************************************************************************** */

/// @brief Loads the config file and parses it into a vector of servers
/// @throws std::runtime_error if the config file cannot be opened
void ConfParser::loadConf(void) {
#ifdef DEBUG
	debugLocus(__func__, FSTART, "loading config file " + _confFile);
#endif
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
#ifdef DEBUG
	debugLocus(__func__, SHOW_MSG, "cleaned config file:\n" NC + fileContent);
#endif

	// TODO: Handle quotes?

	// Get Server Blocks & load them
	std::vector<std::string> serverBlocks = this->getServerBlocks(fileContent);
	this->loadContext(serverBlocks);

#ifdef DEBUG
	std::ostringstream ss;
	ss << "Loaded " << serverBlocks.size() << " servers";
	showContainer(__func__, "Parsed Servers Blocks", serverBlocks);
	debugLocus(__func__, FEND, ss.str());
#endif
}

/// @brief Removes comments from the config file
/// @param file The config file to remove comments from
void ConfParser::removeComments(std::string &file) {
#ifdef DEBUG
	debugLocus(
		__func__, FSTART, "removing comments from config file\n" YEL + file + NC);
#endif
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
#ifdef DEBUG
	debugLocus(
		__func__, FSTART, "removing spaces from config file: " YEL + file + NC);
#endif
	std::istringstream stream(file);
	std::string line;
	std::ostringstream result;

	if (file.empty())
		return;
	while (std::getline(stream, line)) {
		// Remove leading and trailing spaces from the line
		size_t start = line.find_first_not_of(" \t");
		size_t end = line.find_last_not_of(" \t");
		if (start == std::string::npos)
			continue; // Skip empty lines
		// Extract the trimmed line into result
		result << line.substr(start, (end - start + 1)) << '\n';
	}
	file = result.str(); // Replace the file content with the processed result

	// Remove any trailing newline character in the final result
	if (!file.empty() && (file[file.length() - 1] == '\n'))
		file.erase(file.length() - 1);
}

/// @brief Tokenizes a string into a vector of strings
/// @param line The string to tokenize
/// @return A vector of strings
std::vector<std::string> ConfParser::tokenizer(std::string &line) {
#ifdef DEBUG
	debugLocus(__func__, FSTART, "tokenizing line: " YEL + line + NC);
#endif
	std::vector<std::string> tks;
	std::stringstream ss(line);
	std::string val;

	while (ss >> val) {
		ConfParser::removeSpaces(val);
		tks.push_back(val);
	}
	return (tks);
}

/// @brief Gets the server blocks from the config file
/// @param file The config file to get the server blocks from
/// @return A vector of server blocks
std::vector<std::string> ConfParser::getServerBlocks(std::string &file) {
#ifdef DEBUG
	debugLocus(
		__func__, FSTART, "getting server blocks from config file " + _confFile);
#endif
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
#ifdef DEBUG
	std::ostringstream oss;
	oss << "got " << servers.size() << " blocks";
	debugLocus(__func__, FEND, oss.str());
#endif
	return (servers);
}

/// @brief Gets the end of a server block
/// @param file The config file to get the end of the server block from
/// @param start The start of the server block
/// @return The end of the server block
size_t ConfParser::getBlockEnd(std::string &file, size_t start) {
#ifdef DEBUG
	debugLocus(__func__,
			   FSTART,
			   "getting end of server block from config file " + _confFile);
#endif
	short depth = 0;
	while (file[start]) {
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

void ConfParser::loadContext(std::vector<std::string> &blocks) {
#ifdef DEBUG
	debugLocus(__func__, FSTART, "loading context from config file " + _confFile);
#endif
	std::vector<std::string>::iterator it;
	std::string line;
	std::string key;

	for (it = blocks.begin(); it != blocks.end(); it++) {
		std::istringstream block(*it);           // Create stringstream
		std::streampos startPos = block.tellg(); // Save start position
		Server server;

		while (std::getline(block, line, ';')) {
			removeSpaces(line);
			if (line.empty())
				throw std::runtime_error("Invalid server block: empty");

			std::istringstream lineRead(line);
			lineRead >> key;
			if (toLower(key) == "location") {
				// Process location block
				size_t endPos = (*it).find('}', startPos);
				server.setLocation((*it), startPos, endPos);
				std::getline(block, line, '}');
			} else
				server.setDirective(line);
			startPos = block.tellg();
		}
		// TODO: Check for duplicates
		// TODO: Check for empty server blocks

		this->_servers.push_back(server);
#ifdef DEBUG
		showContainer(__func__, "Servers", _servers);
		debugLocus(
			__func__, FEND, "loaded context from config file " + _confFile);
#endif
	}
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

std::vector<Server> ConfParser::getServers(void) const {
#ifdef DEBUG
	debugLocus(__func__, FSTART, "getting servers from object");
#endif

	return (this->_servers);
}
