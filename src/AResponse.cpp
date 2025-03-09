/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:09:55 by passunca          #+#    #+#             */
/*   Updated: 2025/03/09 19:12:36 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/AResponse.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

AResponse::AResponse(const Server &server, const HttpRequest &request)
	: _server(server), _request(request) {
}

AResponse::AResponse(const AResponse &other) 
    : _server(other._server), _request(other._request) {
}

AResponse::~AResponse() {}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

const AResponse &AResponse::operator=(const AResponse &other) {
	(void)other;
	// Assignment intentionally does nothing for abstract base class
	return (*this);
}
