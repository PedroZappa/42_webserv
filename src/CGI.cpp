/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 10:54:11 by passunca          #+#    #+#             */
/*   Updated: 2025/03/22 11:15:24 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CGI.hpp"
#include "../inc/AResponse.hpp"
#include <cstddef>

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

CGI::CGI(HttpRequest &request, HttpResponse &response, const std::string &path)
    : _request(request), _response(response), _path(path) {}

CGI::~CGI() {
    if (_cgiEnv) {
        for (std::size_t i = 0; _cgiEnv[i] != NULL; ++i)
            delete[] _cgiEnv[i];
        delete[] _cgiEnv;
    }
}

/* ************************************************************************** */
/*                               Public Methods                               */
/* ************************************************************************** */

void CGI::handleCGIresponse() {
    std::string cgiOut = execCGI(_path);
    if (std::atoi(cgiOut.c_str()) != 0) {
        _response.status = std::atoi(cgiOut.c_str());
        return;
    }

std:
    size_t pos = cgiOut.find("\r\n\r\n");
    if (pos != std::string::npos) {
        std::string headers = cgiOut.substr(0, pos);
        std::string body = cgiOut.substr(pos + 4);
        std::multimap<std::string, std::string> headerEnv =
            parseCGIheaders(headers);

        // Keep existing Headers
        std::multimap<std::string, std::string>::const_iterator it;
        for (it = headerEnv.begin(); it != headerEnv.end(); ++it) {
            // Check if header is set
            std::multimap<std::string, std::string>::iterator resIt =
                _response.headers.find(it->first);
			if (resIt == _response.headers.end())
				_response.headers.insert(*it);
        }
		_response.body = body;
    } else
		_response.status = INTERNAL_SERVER_ERROR;
}

std::multimap<std::string, std::string>
CGI::parseCGIheaders(const std::string &headers) {}
