/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 16:24:12 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 16:40:42 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETRESPONSE_HPP
#define GETRESPONSE_HPP

#include "../inc/AResponse.hpp"

/**
 * @class GetResponse
 * @brief Handles HTTP GET responses.
 *
 * This class is responsible for generating responses to HTTP GET requests.
 * It inherits from the AResponse class and provides methods to construct
 * and generate the response.
 */
class GetResponse : public AResponse {
  public:
	/**
	* @brief Constructors
	* */
	GetResponse(const Server &, const HttpRequest &);
	GetResponse(const GetResponse &);
	~GetResponse();

	// Public Methods
	std::string generateResponse();
	short loadFile(std::string &path);


  private:
	// Uninstantiable
	GetResponse();
	GetResponse &operator=(const GetResponse &);
};

#endif
