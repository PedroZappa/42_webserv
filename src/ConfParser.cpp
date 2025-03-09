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

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Default constructor for ConfParser.
 */
ConfParser::ConfParser(void) { }

/**
 * @brief Constructs a ConfParser with a specified configuration file.
 * @param confFile The path to the configuration file.
 */
ConfParser::ConfParser(const std::string &confFile)
	: _confFile(confFile) { }

/**
 * @brief Copy constructor for ConfParser.
 * @param copy The ConfParser object to copy from.
 */
ConfParser::ConfParser(const ConfParser &copy)
{
	*this = copy;
}

/**
 * @brief Destructor for ConfParser.
 */
ConfParser::~ConfParser(void) { }

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

/**
 * @brief Assignment operator for ConfParser.
 * @param src The ConfParser object to assign from.
 * @return A reference to the assigned ConfParser object.
 */
ConfParser &ConfParser::operator=(const ConfParser &src)
{
	if (this == &src) return (*this);

	_confFile = src._confFile;
	_servers = src._servers;
	
	return (*this);
}

/* ************************************************************************** */
/*                                  Parsing                                   */
/* ************************************************************************** */

/**
 * @brief Loads the configuration file and parses it into a vector of servers.
 * @throws std::runtime_error if the configuration file cannot be opened or is empty.
 */
void ConfParser::loadConf(void)
{
	Logger::info("Attemping to load config from file '" + _confFile + "'");

	std::ifstream in(_confFile.c_str()); // Open config file
	if (in.fail())
		throw std::runtime_error("Failed to open config file " + _confFile);

	// Read file
	std::string fileContent;
	std::getline(in, fileContent, '\0');
	in.close(); // Close file

	// Clean loaded file
	removeComments(fileContent);
#ifdef DEBUG
	Logger::debug("Removed comments from '" + _confFile + "'");
#endif

	removeSpaces(fileContent);
#ifdef DEBUG
	Logger::debug("Removed spaces from '" + _confFile + "'");
#endif

	if (fileContent.empty()) // Handle empty file
		throw std::runtime_error("Config file is empty");

#ifdef DEBUG
	Logger::debug("ConfParser", __func__, "Cleaned config file");
#endif

	// TODO: Handle quotes?

	// Get Server Blocks & load them
	std::vector<std::string> serverBlocks = getServerBlocks(fileContent);
#ifdef DEBUG
	showContainer(__func__, "Parsed Servers Blocks", serverBlocks);
#endif

	loadContext(serverBlocks);

	std::stringstream ss;
	ss << "Loaded " << serverBlocks.size() << " servers";
	Logger::info(ss.str());
}

/**
 * @brief Removes comments from the configuration file.
 * @param file The configuration file content to remove comments from.
 */
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

/**
 * @brief Removes spaces from the configuration file.
 * @param file The configuration file content to remove spaces from.
 */
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
	file = result.str(); // Replace the file content with the Processed result

	// Remove any trailing newline character in the final result
	if (!file.empty() && (file[file.length() - 1] == '\n'))
		file.erase(file.length() - 1);
}

/**
 * @brief Tokenizes a string into a vector of strings.
 * @param line The string to tokenize.
 * @return A vector of tokenized strings.
 */
std::vector<std::string> ConfParser::tokenizer(std::string &line)
{
#ifdef DEBUG
	Logger::debug("ConfParser", __func__, "Tokenizing line: " YEL + line + NC);
#endif

	std::vector<std::string> tks;
	std::stringstream ss(line);
	std::string val;

	while (ss >> val)
	{
		ConfParser::removeSpaces(val);
		if (val != "{")
			tks.push_back(val);
	}
	return (tks);
}

/**
 * @brief Extracts server blocks from the configuration file content.
 * @param file The configuration file content.
 * @return A vector of server block strings.
 * @throws std::runtime_error if a server block is invalid.
 */
std::vector<std::string> ConfParser::getServerBlocks(std::string &file)
{
#ifdef DEBUG
	Logger::debug("ConfParser", __func__, "Getting server blocks from config file " + _confFile);
#endif

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

#ifdef DEBUG
	std::ostringstream oss;
	oss << "Got server blocks from config file: " << servers.size() << " blocks";
	Logger::debug("ConfParser", __func__, oss.str());
#endif

	return (servers);
}

/**
 * @brief Finds the end of a server block in the configuration file content.
 * @param file The configuration file content.
 * @param start The starting position of the server block.
 * @return The position of the end of the server block.
 * @throws std::runtime_error if the server block is not properly closed.
 */
size_t ConfParser::getBlockEnd(std::string &file, size_t start)
{
#ifdef DEBUG
	Logger::debug("ConfParser", __func__, "Getting end of server block from config file " + _confFile);
#endif

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
				Logger::debug("ConfParser", __func__, "Found end of server block");
				return (start);
			}
		}
		++start;
	}
	throw std::runtime_error("Invalid server block: no '}' at end");
}

/**
 * @brief Loads the context from server blocks.
 * @param blocks The server blocks to load the context from.
 * @throws std::runtime_error if a server block is invalid or has no root.
 */
void ConfParser::loadContext(std::vector<std::string> &blocks)
{
#ifdef DEBUG
	Logger::debug("ConfParser", __func__, "Loading context from config file " + _confFile);
#endif
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
		_servers.push_back(server);

#ifdef DEBUG
		// std::map<std::string, Location> locations = server.getLocations();
		// std::map<std::string, Location>::iterator it;
		// for (it = locations.begin(); it != locations.end(); it++) {
		// 	std::cout << it->first << std::endl;
		// }
		Logger::debug("ConfParser", __func__, "Loaded context from config file " NC + _confFile);
#endif
	}
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

/**
 * @brief Retrieves the vector of servers.
 * @return A vector of Server objects.
 */
std::vector<Server> ConfParser::getServers(void) const
{
	return (_servers);
}

/**
 * @brief Extracts the identifier from a string.
 * @param str The string to extract the identifier from.
 * @return The extracted identifier.
 */
std::string ConfParser::getIdentifier(const std::string &str)
{
	std::string token;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (isalpha(str[i]))
			token += str[i];
		else
			break;
	}
#ifdef DEBUG
	Logger::debug("ConfParser", __func__, "Got identifier: " GRN + token + NC);
#endif
	return (token);
}

/* ************************************************************************** */
/*                                   Debug                                    */
/* ************************************************************************** */

/**
 * @brief Debugs the server locations by printing them.
 * @param serverN The server number.
 * @param route The route of the server location.
 */
void ConfParser::debugServerLocations(size_t serverN, const std::string &route)
{
	std::cout << "Server " << serverN << " location: " << route << std::endl;
	std::cout << "Root: " << _servers[serverN].getRoot() << std::endl;
}
