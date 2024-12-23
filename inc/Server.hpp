/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:45:52 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 10:52:56 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"

/// @brief Network Listening Endpoint
struct Listen {
	// Constructors
	Listen(void);
	Listen(const std::string &ip, const std::string &port)
		: ip(ip), port(port) {};
	~Listen(void);

	// Operators
	bool operator<(const Listen &rhs) const {
		if (this->ip != rhs.ip)
			return (this->ip < rhs.ip);
		return (this->port < rhs.port);
	}
	bool operator==(const Listen &rhs) const {
		return ((this->ip == rhs.ip) && (this->port == rhs.port));
	}

	// Attributes
	std::string ip;
	std::string port;
};

#endif
