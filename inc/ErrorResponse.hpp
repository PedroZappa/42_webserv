/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:31:05 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 16:45:55 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_RESPONSE_HPP
#define ERROR_RESPONSE_HPP

#include "AResponse.hpp"

/**
 * @class ErrorResponse
 * @brief Handles HTTP error responses.
 *
 * This class is responsible for generating responses for HTTP error statuses.
 * It inherits from the AResponse class and provides methods to construct
 * and generate the error response.
 */
class ErrorResponse : public AResponse {
  public:
	ErrorResponse(const Server &, const HttpRequest &, short errorStatus);
	ErrorResponse(const ErrorResponse &);
	~ErrorResponse();

	std::string generateResponse();

  private:
	short _errorStatus; /**< The HTTP error status code. */

	ErrorResponse();
	const ErrorResponse &operator=(const ErrorResponse &);
};

#endif
