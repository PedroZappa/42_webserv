/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteResponse.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 16:31:39 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 16:31:49 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DELETE_RESPONSE_HPP
#define DELETE_RESPONSE_HPP

#include "AResponse.hpp"
#include "HttpParser.hpp"
#include "Webserv.hpp"

/**
 * @class DeleteResponse
 * @brief Handles the generation of HTTP DELETE responses.
 *
 * This class is responsible for creating responses to HTTP DELETE requests.
 * It inherits from the AResponse class and implements the necessary logic
 * to generate appropriate responses based on the server and request details.
 */
class DeleteResponse : public AResponse {
  public:
	// Constructors
	DeleteResponse(const Server &, const HttpRequest &);
	DeleteResponse(const DeleteResponse &);
	~DeleteResponse();

	// Getters
	std::string generateResponse();

	//  Checkers
	bool isDirEmpty(const std::string &path) const;

	// Actions
	short deleteFile(const std::string &path);
	short deleteDir(const std::string &path);

  private:
	// Unusable
	DeleteResponse();
	DeleteResponse &operator=(const DeleteResponse &);
};

#endif
