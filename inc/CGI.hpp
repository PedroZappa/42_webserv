/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */

/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 10:49:33 by passunca          #+#    #+#             */
/*   Updated: 2025/03/22 12:35:27 by passunca         ###   ########.fr       */

/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "AResponse.hpp"
#include "HttpParser.hpp"
#include <vector>

class CGI {
  public:
    CGI(HttpRequest &, HttpResponse &, const std::string &paqth);
    ~CGI();

    // Public Methods
    short generateResponse();
    std::pair<short, std::string> execute(const std::string &script);
    std::string getEnvVal(std::string key);

  private:
    const HttpRequest &_request;
    HttpResponse &_response;
    const std::string &_path;
    char **_cgiEnv;

    // Private Methods
    void runScript(int *pipeIn, int *pipeOut, const std::string &script);
    short setCGIenv();
    void setEnvVar(std::vector<std::string> &env, std::string key,
                   std::string varToAdd);
	std::string getServerName();
	std::string getServerPort();
	std::string getQueryFields();
	std::string getCookies();
	char **vec2charArr(const std::vector<std::string> &);

    std::pair<short, std::string> getOutput(pid_t, int *pipeOut);
    // Parse
    std::multimap<std::string, std::string>
    parseCGIheaders(const std::string &headers);

    bool hasSingleValue(std::string &key);
};

#endif