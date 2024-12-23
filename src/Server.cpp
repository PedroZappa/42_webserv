/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 11:33:13 by passunca          #+#    #+#             */
/*   Updated: 2024/12/23 13:14:49 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

Server::Server(void) {
	// NginX Default server
	// TODO: Push back index.html/index.htm to _index vewctor
	// TODO: Initialize Directives Map
	// TODO: Init Set of HTTP request methods (Declare it in class?)
	// TODO: Insert methods into _validMethods vector
	// TODO: set _return to Pair(-1, "") (empty)
}

Server::Server(const Server &copy) : methods(copy.methods) {
	// TODO: User initialization list to get values to copy
}

Server::~Server(void) {
}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

Server &Server::operator=(const Server &src) {
	// TODO: Init same vars as Copy Constructor
	return (*this);
}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

std::vector<Socket> Server::getNetAddr(void) const {
	return (this->_netAddr);
};
