/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 10:54:11 by passunca          #+#    #+#             */
/*   Updated: 2025/03/22 12:36:30 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CGI.hpp"
#include "../inc/AResponse.hpp"
#include <cstddef>
#include <cstdlib>
#include <iterator>
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
/*                                   Methods                                  */
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

/**
 * @brief Execute a CGI script and retrieve its output.
 *
 * This function sets up pipes for inter-process communication and forks a child
 * process to execute the CGI script. The script's output is captured and
 * returned as a string. If any errors occur during the setup or execution, an
 * appropriate error message is returned.
 *
 * @param script The path to the CGI script to be executed.
 * @return A string containing the output from the CGI script, or an error
 * message if the script fails.
 */
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

/**
 * @brief Executes the CGI script in a child process.
 *
 * This function sets up the necessary environment and file descriptors
 * for executing a CGI script. It duplicates the input and output pipes
 * to the standard input and output, sets environment variables, and
 * applies memory limits to the child process. It then changes the
 * working directory to the script's directory and executes the script
 * using execve.
 *
 * @param pipeIn An array of two integers representing the input pipe.
 * @param pipeOut An array of two integers representing the output pipe.
 * @param script The path to the CGI script to be executed.
 */
void CGI::runScript(int *pipeIn, int *pipeOut, const std::string &script) {
    dup2(pipeIn[0], STDIN_FILENO);
    close(pipeIn[1]);
    dup2(pipeOut[1], STDOUT_FILENO);
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

    char *argv[] = {const_cast<char *>(script.c_str()), NULL};
    if (execve(script.c_str(), argv, _cgiEnv) == -1)
        exit(EXIT_FAILURE);
}

/**
 * @brief Retrieve the output from a CGI script.
 *
 * This function reads the output from a CGI script executed in a child process.
 * It waits for the child process to complete and reads the data from the output
 * pipe. If the child process does not complete within the specified timeout, it
 * is terminated.
 *
 * @param pid The process ID of the child process running the CGI script.
 * @param pipeOut An array of two integers representing the output pipe.
 * @return A string containing the output from the CGI script, or an error
 * message if the script fails.
 */
std::string CGI::getCGIout(pid_t pid, int *pipeOut) {
    std::string cgiOut;
    std::size_t bytesRead;
    char buff[1024];
    struct timeval startTime;
    struct timeval currTime;

    gettimeofday(&startTime, NULL);
    while (true) {
        gettimeofday(&currTime, NULL);
        int status;

        if (waitpid(pid, &status, WNOHANG) != 0) {
            if (WEXITSTATUS(status) != 0)
                return (err2string(INTERNAL_SERVER_ERROR));
            while ((bytesRead = read(pipeOut[0], buff, sizeof(buff)) > 0))
                cgiOut.append(buff, bytesRead);
            break;
        }
        if ((currTime.tv_sec - startTime.tv_sec) > TIMEOUT) {
            kill(pid, SIGKILL);
            return (err2string(GATEWAY_TIMEOUT));
        }
    }
    return (cgiOut);
}

/**
 * @brief Retrieve the value of an environment variable from the request
 * headers.
 *
 * This function searches for the specified key in the request headers and
 * returns its associated value. If the key has multiple values, they are
 * concatenated into a single string, separated by commas.
 *
 * @param key The key of the environment variable to retrieve.
 * @return A string containing the value of the environment variable, or an
 * empty string if the key is not found.
 */
std::string CGI::getEnvVal(std::string key) {
    std::pair<std::multimap<std::string, std::string>::const_iterator,
              std::multimap<std::string, std::string>::const_iterator>
        range = _request.headers.equal_range(key);
    if (hasSingleValue(key)) {
        if (range.first != range.second)
            return (range.first->second);
    } else {
        std::string val;
        std::multimap<std::string, std::string>::const_iterator it;
        for (it = range.first; it != range.second; ++it) {
            if (!val.empty())
                val += ", ";
            val += it->second;
        }
        return (val);
    }
    return ("");
}

/**
 * @brief Check if a header key is expected to have a single value.
 *
 * This function determines whether a given HTTP header key is expected
 * to have a single value or multiple values. It uses a static list of
 * known headers that can have multiple values to make this determination.
 *
 * @param key The header key to check.
 * @return True if the header is expected to have a single value, false if it
 * can have multiple values.
 */
bool CGI::hasSingleValue(std::string &key) {
    // Static list of headers that are not single-value
    static const std::vector<std::string> headers = {
        "Accept", "Accept-Encoding", "Cache-Control", "Set-Cookie",
        "Via",    "Forewarded"};
    // Use std::find to check if the key exists in the list
    if (std::find(headers.begin(), headers.end(), key) != headers.end())
        return (false);
    return (true);
}

/* ************************************************************************** */
/*                                   Parse                                    */
/* ************************************************************************** */

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
