/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gfragoso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 23:25:52 by gfragoso          #+#    #+#             */
/*   Updated: 2025/03/22 00:44:23 by gfragoso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CGI.hpp"

CGI::CGI(HttpRequest &request, HttpResponse &response, const std::string &path)
    : _request(request), _response(response), _path(path), _cgiEnv(NULL) {}

CGI::~CGI() {
    if (_cgiEnv == NULL) return;
    for (size_t i = 0; _cgiEnv[i] != NULL; i++)
        delete[] _cgiEnv[i];
    delete[] _cgiEnv;
}

std::string CGI::createOutput(pid_t pid, int *pipe) {
    timeval start, curr;
    int status;
    
    gettimeofday(&start, NULL);
    gettimeofday(&curr, NULL);

    while (curr.tv_sec - start.tv_sec < TIMEOUT) {
        gettimeofday(&curr, NULL);
        
        if (waitpid(pid, &status, WNOHANG) != 0) {
            if (WEXITSTATUS(status) != 0)
                throw std::runtime_error("500 - CGI didn't exit correctly");
            
            char buffer[4096];
            ssize_t bytes;
            std::string output;
            while ((bytes = read(pipe[0], buffer, sizeof(buffer))) > 0) {
                output.append(buffer, bytes);
            }
        }
    }

    kill(pid, SIGKILL);
    throw std::runtime_error("504 - Timed Out");
}

std::string CGI::run() {
    int in[2], out[2];
    
    if (pipe(in) == -1 || pipe(out) == -1)
        throw std::runtime_error("Pipes failed to create");

    pid_t pid = fork();
    if (pid == -1)
        throw std::runtime_error("Fork failed");
    
    if (pid == 0) {
        // TODO: CGI runner
    }
    
    close(in[0]);
    close(out[1]);
    
    if (!_request.body.empty()) {
        write(in[1], _request.body.c_str(), _request.body.size());
    }
    
    close(in[1]);

    return createOutput(pid, out);
}

short CGI::execute()
{
    std::string output;
    try {
        output = run();
    } catch (const std::exception &e) {
        int status = std::atoi(e.what());
        Logger::error(e.what());
        return status;
    }

    size_t pos = output.find("\r\n\r\n");
    if (pos == std::string::npos) {
        _response.status = INTERNAL_SERVER_ERROR;
        return (INTERNAL_SERVER_ERROR);
    }

    std::string body = output.substr(pos + 4);
    // TODO: Headers
    _response.body = body;
    return (OK);
}