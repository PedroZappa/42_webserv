/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 10:49:33 by passunca          #+#    #+#             */
/*   Updated: 2025/03/22 12:15:45 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "AResponse.hpp"
#include "HttpParser.hpp"

class CGI {
  public:
    CGI(HttpRequest &, HttpResponse &, const std::string &paqth);
    ~CGI();

    // Public Methods
    void handleCGIresponse();
    std::string execCGI(const std::string &script);
    std::string getEnvVal(std::string key);

  private:
    const HttpRequest &_request;
    HttpResponse &_response;
    const std::string &_path;
    char **_cgiEnv;

    // Private Methods
    // Parse
    std::multimap<std::string, std::string>
    parseCGIheaders(const std::string &headers);

    void runScript(int *pipeIn, int *pipeOut, const std::string &script);
	short setCGIenv();
    std::string getCGIout(pid_t, int *pipOut);
};

#endif
