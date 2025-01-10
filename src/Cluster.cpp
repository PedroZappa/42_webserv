/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:12:12 by passunca          #+#    #+#             */
/*   Updated: 2025/01/10 12:40:05 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Cluster.hpp"

/* ************************************************************************** */
/*                          Constructor & Destructor                          */
/* ************************************************************************** */

/// @brief Default constructor
/// @details Creates a parameterized cluster
Cluster::Cluster(const std::vector<Server> &servers) {
	(void)servers;
}

Cluster::~Cluster() {

}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

/// @brief Index operator
/// @param idx The index to get
/// @details Returns the server at the specified index
const Server &Cluster::operator[](size_t idx) const {
	if (idx >= _servers.size())
		throw std::out_of_range("Index out of range");
	return (*_servers[idx]);
}

/* ************************************************************************** */
/*                                   Setup                                    */
/* ************************************************************************** */



/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */
