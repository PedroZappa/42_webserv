/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gfragoso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 10:54:11 by passunca          #+#    #+#             */
/*   Updated: 2025/03/22 22:37:32 by gfragoso         ###   ########.fr       */
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
    if (_cgiEnv == NULL) return;
    
    for (std::size_t i = 0; _cgiEnv[i] != NULL; ++i)
        delete[] _cgiEnv[i];
    delete[] _cgiEnv;
}

/* ************************************************************************** */
/*                                   Methods                                  */
/* ************************************************************************** */

/**
 * @brief Handle the CGI response by executing the CGI script and processing its
 * output.
 */
short CGI::generateResponse() {
    std::pair<short, std::string> result = execute(_path);
    if (result.first != OK) {
        _response.status = result.first;
        Logger::warn(result.second);
        return result.first;
    }

    std::string &output = result.second;
    std::size_t pos = output.find("\r\n\r\n");
    if (pos == std::string::npos) {
        _response.status = INTERNAL_SERVER_ERROR;
        return (INTERNAL_SERVER_ERROR);
    }
    
    std::string headers = output.substr(0, pos);
    std::string body = output.substr(pos + 4);
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
    return (OK);
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
std::pair<short, std::string> CGI::execute(const std::string &script) {
    int pipeIn[2], pipeOut[2];

    if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1)
        return (std::make_pair(INTERNAL_SERVER_ERROR, "Couldn't open pipes"));

    pid_t pid = fork();
    if (pid == -1)
        return (std::make_pair(INTERNAL_SERVER_ERROR, "Couldn't fork process"));

    if (pid == 0) {
        runScript(pipeIn, pipeOut, script);
        exit(0);
    }

    close(pipeIn[0]);
    close(pipeOut[1]);
    if (!_request.body.empty())
        write(pipeIn[1], _request.body.c_str(), _request.body.length());
    close(pipeIn[1]);

    std::pair<short, std::string> output = getOutput(pid, pipeOut);
    
    close(pipeOut[0]);
    return (output);
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
    { 
        // Set Child Memory Space Limit
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
 * @brief Set up the CGI environment variables.
 *
 * This function initializes the environment variables required for executing
 * a CGI script. It checks for necessary POST request headers and populates
 * a vector with key-value pairs representing the CGI environment. The
 * environment variables are then converted to a format suitable for execve.
 *
 * @return A status code indicating success or failure. Returns OK on success,
 * or INTERNAL_SERVER_ERROR if required headers are missing.
 */
short CGI::setCGIenv() {
    if (((_request.method == POST) && (getEnvVal("content-type").empty())) ||
        ((_request.method == POST) && (getEnvVal("content-length").empty())))
        return (INTERNAL_SERVER_ERROR);

    std::vector<std::string> cgiEnv;

    setEnvVar(cgiEnv, "REQUEST_METHOD", method2string(_request.method).c_str());
    setEnvVar(cgiEnv, "CONTENT_TYPE", getEnvVal("content-type").c_str());
    setEnvVar(cgiEnv, "SERVER_NAME", getServerName().c_str());
    setEnvVar(cgiEnv, "SERVER_PORT", getServerPort().c_str());
    setEnvVar(cgiEnv, "GATEWAY_INTERFACE", "CGI/1.1");
    setEnvVar(cgiEnv, "PATH_INFO", _path.c_str());
    setEnvVar(cgiEnv, "CONTENT_LENGTH", getEnvVal("content-length").c_str());
    setEnvVar(cgiEnv, "QUERY_STRING", getQueryFields().c_str());
    setEnvVar(cgiEnv, "SCRIPT_NAME", _request.uri.c_str());
    setEnvVar(cgiEnv, "SERVER_PROTOCOL", _request.protocolVersion.c_str());
    setEnvVar(cgiEnv, "SERVER_SOFTWARE", SERVER_NAME);

    std::string cookies = getCookies();
    if (!cookies.empty())
        setEnvVar(cgiEnv, "HTTP_COOKIE", cookies.c_str());

    _cgiEnv = vec2charArr(cgiEnv);
    return (OK);
}

/**
 * @brief Add an environment variable to the CGI environment vector.
 *
 * This function constructs a key-value pair string in the format "key=value"
 * and appends it to the provided environment vector.
 *
 * @param env The vector to which the environment variable will be added.
 * @param key The key of the environment variable.
 * @param varToAdd The value of the environment variable.
 */
void CGI::setEnvVar(std::vector<std::string> &env, std::string key,
                    std::string varToAdd) {
    env.push_back(key + "=" + varToAdd);
}

/**
 * @brief Retrieve the server name from the request headers.
 *
 * This function extracts the server name from the "host" header in the
 * request. If the header contains a port number, it is removed from the
 * returned server name.
 *
 * @return A string containing the server name, or an empty string if the
 * "host" header is not present.
 */
std::string CGI::getServerName() {
    std::multimap<std::string, std::string>::const_iterator it =
        _request.headers.find("host");
    if (it == _request.headers.end())
        return ("");

    std::string hostname = it->second;
    std::size_t colonPos = hostname.find_first_of(':');
    if (colonPos != std::string::npos)
        hostname = hostname.substr(0, colonPos);

    return (hostname);
}

/**
 * @brief Retrieve the server port from the request headers.
 *
 * This function extracts the server port from the "host" header in the
 * request. If the header does not contain a port number, an empty string
 * is returned.
 *
 * @return A string containing the server port, or an empty string if the
 * "host" header is not present or does not include a port number.
 */
std::string CGI::getServerPort() {
    std::multimap<std::string, std::string>::const_iterator it =
        _request.headers.find("host");
    if (it == _request.headers.end())
        return ("");

    std::string port = it->second;
    std::size_t colonPos = port.find_first_of(':');
    if (colonPos != std::string::npos)
        port = port.substr(colonPos + 1);
    return (port);
}

/**
 * @brief Construct a query string from the request's query parameters.
 *
 * This function iterates over the query parameters in the request and
 * constructs a query string in the format "key1=value1&key2=value2".
 *
 * @return A string containing the constructed query string.
 */
std::string CGI::getQueryFields() {
    std::string queryStr;
    std::multimap<std::string, std::string>::const_iterator it;
    for (it = _request.queryParams.begin(); it != _request.queryParams.end();
         ++it) {
        if (!queryStr.empty())
            queryStr += "&";
        queryStr += it->first + "=" + it->second;
    }
    return (queryStr);
}

/**
 * @brief Retrieve cookies from the request headers.
 *
 * This function extracts cookies from the "cookie" headers in the request
 * and concatenates them into a single string, separated by semicolons.
 *
 * @return A string containing the concatenated cookies, or an empty string
 * if no "cookie" headers are present.
 */
std::string CGI::getCookies() {
    std::string cookies;
    std::multimap<std::string, std::string>::const_iterator it;
    for (it = _request.headers.begin(); it != _request.headers.end(); ++it) {
        if (it->first == "cookie") {
            if (!cookies.empty())
                cookies += "; ";
            cookies += it->second;
        }
    }
    return (cookies);
}

/**
 * @brief Convert a vector of strings to an array of C-style strings.
 *
 * This function allocates memory for an array of C-style strings and
 * copies the contents of the provided vector into the array. The array
 * is terminated with a NULL pointer.
 *
 * @param env The vector of strings to be converted.
 * @return A pointer to the newly allocated array of C-style strings.
 */
char **CGI::vec2charArr(const std::vector<std::string> &env) {
    char **charArr = new char *[env.size() + 1];

    for (std::size_t i = 0; i < env.size() + 1; ++i) {
        charArr[i] = new char[env[i].size() + 1];
        std::strcpy(charArr[i], env[i].c_str());
    }
    charArr[env.size()] = NULL;

    return (charArr);
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
std::pair<short, std::string> CGI::getOutput(pid_t pid, int *pipeOut) {
    timeval startTime, currTime;

    gettimeofday(&startTime, NULL);
    currTime = startTime;
    while ((currTime.tv_sec - startTime.tv_sec) > TIMEOUT) {
        gettimeofday(&currTime, NULL);
        int status;

        if (waitpid(pid, &status, WNOHANG) != 0) {
            if (WEXITSTATUS(status) != 0)
                return (std::make_pair(INTERNAL_SERVER_ERROR, "Child didn't exit correctly."));
            
            std::size_t bytesRead;
            char buff[1024];
            std::string output;
            while ((bytesRead = read(pipeOut[0], buff, sizeof(buff)) > 0))
                output.append(buff, bytesRead);
            return (std::make_pair(OK, output));
        }
    }
    
    kill(pid, SIGKILL);
    return (std::make_pair(GATEWAY_TIMEOUT, "Child took too long to exit."));
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
    static std::vector<std::string> headers = std::vector<std::string>(6, "");
    headers.push_back("Accept");
    headers.push_back("Accept-Encoding");
    headers.push_back("Cache-Control");
    headers.push_back("Set-Cookie");
    headers.push_back("Via");
    headers.push_back("Forewarded");

    // static const std::vector<std::string> headers = {
    //     "Accept", "Accept-Encoding", "Cache-Control", "Set-Cookie",
    //     "Via",    "Forewarded"};
    // Use std::find to check if the key exists in the list
    return (std::find(headers.begin(), headers.end(), key) == headers.end());
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
