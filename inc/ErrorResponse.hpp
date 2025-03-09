/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:31:05 by passunca          #+#    #+#             */
/*   Updated: 2025/03/09 19:35:58 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_RESPONSE_HPP
#define ERROR_RESPONSE_HPP

#include "AResponse.hpp"

class ErrorResponse : public AResponse {
  public:
	ErrorResponse(const Server &, const HttpRequest &, short errorStatus);
	ErrorResponse(const ErrorResponse &);
	~ErrorResponse();

	std::string generateResponse();

private:
	short _errorStatus;

	ErrorResponse();
	const ErrorResponse &operator=(const ErrorResponse &);
};

#endif
