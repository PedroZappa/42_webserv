/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:00:04 by passunca          #+#    #+#             */
/*   Updated: 2025/03/09 19:14:09 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARESPONSE_HPP
#define ARESPONSE_HPP

#include "HttpParser.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Webserv.hpp"

class Server;
class Location;

struct HttpResponse {
	unsigned short status;
	std::multimap<std::string, std::string> headers;
	std::string body;

	HttpResponse() : status(200) {
	}
};

class AResponse {
  public:
	// Constructors
	AResponse(const Server &, const HttpRequest &);
	AResponse(const AResponse &);
	// Assignment Operator
	const AResponse &operator=(const AResponse &);
	// Destructor 
	virtual ~AResponse();

	// Public Member Functions
	virtual std::string generateResponse() = 0;

  protected:
	HttpRequest _request;
	HttpResponse _response;
	const Server &_server;
	std::string _locationRoute;

};

std::ostream &operator<<(std::ostream &, const AResponse &);

#endif
