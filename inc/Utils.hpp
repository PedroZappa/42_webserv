/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 15:18:14 by passunca          #+#    #+#             */
/*   Updated: 2024/12/24 10:36:22 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "Server.hpp"
#include "Webserv.hpp"
#include "Debug.hpp"
#include "Error.h"

class Server;

/* ************************************************************************** */
/*                                Convertions                                 */
/* ************************************************************************** */

std::string toLower(const std::string &str);

template <typename T> std::string nToStr(T num);
template <typename T> T strToN(const std::string &str);

#endif
