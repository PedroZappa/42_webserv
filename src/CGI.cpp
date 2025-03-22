/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 10:54:11 by passunca          #+#    #+#             */
/*   Updated: 2025/03/22 12:26:43 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CGI.hpp"
#include "../inc/AResponse.hpp"
#include <cstddef>
#include <map>
#include <sys/resource.h>
#include <unistd.h>

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Construct a new CGI object.
 *
 * @param request The HTTP request object.
 * @param response The HTTP response object.
 * @param path The path to the CGI script.
 */
CGI::CGI(HttpRequest &request, HttpResponse &response, const std::string &path)
    : _request(request), _response(response), _path(path) {}

/**
 * @brief Destroy the CGI object and clean up resources.
 */
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

/**
 * @brief Handle the CGI response by executing the CGI script and processing its
 * output.
 */
void CGI::handleCGIresponse() {
    std::string cgiOut = execCGI(_path);
    if (std::atoi(cgiOut.c_str()) != 0) {
        _response.status = std::atoi(cgiOut.c_str());
        return;
    }

    std::size_t pos = cgiOut.find("\r\n\r\n");
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

std::string CGI::execCGI(const std::string &script) {
    int pipeIn[2];
    int pipeOut[2];
    std::string cgiOut;

    if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1)
        return (err2string(INTERNAL_SERVER_ERROR));

    pid_t pid = fork();
    if (pid == -1)
        return (err2string(INTERNAL_SERVER_ERROR));

    if (pid == 0)
        runScript(pipeIn, pipeOut, script);
    else {
        close(pipeIn[0]);
        close(pipeOut[1]);
        if (!_request.body.empty())
            write(pipeIn[1], _request.body.c_str(), _request.body.length());
        close(pipeIn[1]);

        cgiOut = getCGIout(pid, pipeOut);
    }
    close(pipeOut[0]);
    return (cgiOut);
}

void CGI::runScript(int *pipeIn, int *pipeOut, const std::string &script) {
	dup2(pipeIn[0],STDIN_FILENO);
	close(pipeIn[1]);
	dup2(pipeOut[1],STDOUT_FILENO);
	close(pipeIn[0]);

	short status = setCGIenv();
	if (status != OK)
		exit(EXIT_FAILURE);
	{ // Set Child Memory Space Limit
		struct rlimit lim;
		lim.rlim_cur = (200 * KB * KB);
		lim.rlim_max = (200 * KB * KB);
		setrlimit(RLIMIT_AS, &lim);
	}

	std::string dir = script.substr(0, script.find_last_of("/"));
	if (chdir(dir.c_str()) == -1)
		exit(EXIT_FAILURE);

	char *argv[] = { const_cast<char *>(script.c_str()), NULL };
	if (execve(script.c_str(), argv, _cgiEnv) == -1)
		exit(EXIT_FAILURE);
}



std::string CGI::getCGIout(pid_t, int *pipOut) {

}

/**
 * @brief Parse CGI headers from a string and return them as a multimap.
 *
 * @param headers The string containing CGI headers.
 * @return A multimap of header key-value pairs.
 */
std::multimap<std::string, std::string>
CGI::parseCGIheaders(const std::string &headers) {
    std::multimap<std::string, std::string> headerEnv;
    std::istringstream ss(headers);
    std::string line;

    while (std::getline(ss, line)) {
        std::size_t colonPos = line.find(": ");
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);
            headerEnv.insert(std::make_pair(key, value));
        }
    }
    return (headerEnv);
    ;
}
