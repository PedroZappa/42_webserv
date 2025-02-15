/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:00:04 by passunca          #+#    #+#             */
/*   Updated: 2024/12/25 21:13:02 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfParser.hpp"
#include "../inc/Utils.hpp"
#include <sstream>

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */
ConfParser::ConfParser(void) { }

ConfParser::ConfParser(const std::string &confFile)
	: _confFile(confFile) { }

ConfParser::ConfParser(const ConfParser &copy)
{
	*this = copy;
}

ConfParser::~ConfParser(void) { }

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

ConfParser &ConfParser::operator=(const ConfParser &src)
{
	if (this == &src) return (*this);

	this->_confFile = src._confFile;
	this->_servers = src._servers;
	
	return (*this);
}

/* ************************************************************************** */
/*                                  Parsing                                   */
/* ************************************************************************** */

/// @brief Loads the config file and parses it into a vector of servers
/// @throws std::runtime_error if the config file cannot be opened
void ConfParser::loadConf(void)
{
	_DEBUG(FEND, "Loaded config file " + _confFile);

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

	_DEBUG(FEND, "Cleaned config file");

	// TODO: Handle quotes?

	// Get Server Blocks & load them
	std::vector<std::string> serverBlocks = getServerBlocks(fileContent);
	this->loadContext(serverBlocks);

	std::stringstream ss;
	ss << "Loaded " << serverBlocks.size() << " servers";
	Logger::info(ss.str());
	// showContainer(__func__, "Parsed Servers Blocks", serverBlocks);
	// debug(typeid(*this).name(), __func__, FEND, ss.str());
	// _DEBUG(FEND, "parsed config file " + ss.str());
}

/// @brief Removes comments from the config file
/// @param file The config file to remove comments from
void ConfParser::removeComments(std::string &file)
{
	if (file.empty()) return;
	size_t comment = file.find('#');
	while (comment != std::string::npos) {
		size_t eol = file.find('\n', comment);
		file.erase(comment, (eol - comment));
		comment = file.find('#');
	}
}

/// @brief Removes spaces from the config file
/// @param file The config file to remove spaces from
void ConfParser::removeSpaces(std::string &file)
{
	std::string line;
	std::ostringstream result;
	std::istringstream stream(file);

	if (file.empty())
		return;
	while (std::getline(stream, line))
	{
		// Remove leading and trailing spaces from the line
		size_t start = line.find_first_not_of(" \t\n");
		size_t end = line.find_last_not_of(" \t\n");
		if ((start == std::string::npos) || ((start == end) && (end == '\n')))
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
std::vector<std::string> ConfParser::tokenizer(std::string &line)
{
	Logger::debug("ConfParser", __func__, FSTART, "tokenizing line: " YEL + line + NC);
	std::vector<std::string> tks;
	std::stringstream ss(line);
	std::string val;

	while (ss >> val) {
		ConfParser::removeSpaces(val);
		if (val != "{")
			tks.push_back(val);
	}
	return (tks);
}

/// @brief Gets the server blocks from the config file
/// @param file The config file to get the server blocks from
/// @return A vector of server blocks
std::vector<std::string> ConfParser::getServerBlocks(std::string &file)
{
	_DEBUG(FSTART, "getting server blocks from config file " + _confFile);
	std::vector<std::string> servers;
	std::string identifier;
	size_t start = 0;
	size_t endBlock = 0;
	size_t end = 0;

	while (file[start])
	{                       // Loop until end of file
		identifier = this->getIdentifier(file); // Get server block identifier
		if (toLower(identifier) != "server")
			throw std::runtime_error("Invalid server block: no 'server' at "
									 "start");
		start += std::strlen(identifier.c_str());

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
		start = (end + 1);                               // Skip '}'
		while (file[start] && std::isspace(file[start])) // Skip trailing spaces
			++start;
	}

	std::ostringstream oss;
	oss << "Got server blocks from config file: " << servers.size() << " blocks";
	_DEBUG(FEND, oss.str());

	return (servers);
}

/// @brief Gets the end of a server block
/// @param file The config file to get the end of the server block from
/// @param start The start of the server block
/// @return The end of the server block
size_t ConfParser::getBlockEnd(std::string &file, size_t start)
{
	_DEBUG(FSTART, "Getting end of server block from config file " + _confFile);
	short depth = 0;
	while (file[start])
	{
		char c = file[start];
		if (c == '{')
			++depth;
		else if (c == '}')
		{
			--depth;
			if (depth == 0)
			{
				_DEBUG(FEND, "end of server block found");
				return (start);
			}
		}
		++start;
	}
	throw std::runtime_error("Invalid server block: no '}' at end");
}

/// @brief Loads the context from the config file
/// @param blocks The server blocks to load the context from
void ConfParser::loadContext(std::vector<std::string> &blocks)
{
	_DEBUG(FSTART, "loaded context from config file " + _confFile);
	std::vector<std::string>::iterator it;
	std::string line;
	std::string brace;

	for (it = blocks.begin(); it != blocks.end(); it++)
	{
		std::istringstream block(*it);           // Create stringstream
		std::streampos startPos = block.tellg(); // Save start position
		Server server;

		while (std::getline(block, line, ';'))
		{
			removeSpaces(line);
			if (line.empty())
				throw std::runtime_error("Invalid server block: empty");

			std::istringstream lineRead(line);
			lineRead >> brace;                  // Skip closing braces
			if (toLower(brace) == "location") // Get location block
			{
				// Process location block
				size_t locationEnd = (*it).find("}", startPos);
				server.setLocation((*it), startPos, locationEnd);
				std::getline(block, line, '}');
			}
			else if (toLower(brace) == "}")
			{
				break;
			}
			else server.setDirective(line); // Get server directive
			startPos = block.tellg();
		}
		// TODO: Check for duplicates
		if (server.getRoot().empty())
			throw std::runtime_error("Invalid server block: no root");
		this->_servers.push_back(server);

		// std::map<std::string, Location> locations = server.getLocations();
		// std::map<std::string, Location>::iterator it;
		// for (it = locations.begin(); it != locations.end(); it++) {
		// 	std::cout << it->first << std::endl;
		// }
		_DEBUG(FEND, "Loaded context from config file " NC + _confFile);
	}
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

/// @brief Returns the vector of servers
/// @return A vector of Server objects
std::vector<Server> ConfParser::getServers(void) const
{
	return (this->_servers);
}

/// @brief Returns the identifier of a server block
/// @param str The string to get the identifier from
/// @return The identifier of the server block
std::string ConfParser::getIdentifier(const std::string &str)
{
	_DEBUG(FSTART, "got identifier:");
	std::string token;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (isalpha(str[i]))
			token += str[i];
		else
			break;
	}
	_DEBUG(FEND, "got identifier: " GRN + token + NC);
	return (token);
}

/* ************************************************************************** */
/*                                   Debug                                    */
/* ************************************************************************** */

void ConfParser::debugServerLocations(size_t serverN, const std::string &route)
{
	std::cout << "Server " << serverN << " location: " << route << std::endl;
	std::cout << "Root: " << _servers[serverN].getRoot() << std::endl;
}
