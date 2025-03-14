/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 16:29:20 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 16:39:30 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTRESPONSE_HPP
#define POSTRESPONSE_HPP

#include "../inc/AResponse.hpp"

/**
 * @struct File
 * @brief Represents a file to be uploaded.
 *
 * This structure holds information about a file, including its name, full path,
 * content type, and content.
 */
struct File {
	std::string name;
	std::string path; // Full Path of File Name
	std::string type; // Content type

	std::string content;
};

/**
 * @class PostResponse
 * @brief Handles HTTP POST responses.
 *
 * This class is responsible for generating responses to HTTP POST requests.
 * It inherits from the AResponse class and provides methods to construct
 * and generate the response.
 */
class PostResponse : public AResponse {
  public:
	/**
	 * @brief Constructors
	 * */
	PostResponse(const Server &, const HttpRequest &, int clientFd, int epollFd);
	PostResponse(const PostResponse &);
	~PostResponse();

	// Public Methods
	std::string generateResponse();

  private:
	std::vector<std::multimap<std::string, std::string> >
		_body;                              /**< Body of the response */
	std::map<int, std::string> _fileBuffer; /**< Buffer for file data */
	std::string _limit;             /**< Boundary limit for the response */
	struct File _file2upload;       /**< File to be uploaded */
	int _clientFd;                  /**< Client socket file descriptor */
	int _epollFd;                   /**< Epoll instance file descriptor */
	unsigned short _responseStatus; /**< Response status code */

	// Unusable
	PostResponse();
	PostResponse &operator=(const PostResponse &);

	// Private Methods
	unsigned short parseHttp();
	bool hasHeader(const std::string &header) const;
	bool send100continue();

	short checkBody();
	const std::string getLimit();
	const std::vector<std::multimap<std::string, std::string> >
	getBody(const std::string &limit);
	const std::multimap<std::string, std::string> getFields(const std::string &str);

	short getFile();
	std::string getFieldValue(const std::string &header,
							  const std::string &field);

	short uploadFile();
	short checkForm();
	//
};

#endif
