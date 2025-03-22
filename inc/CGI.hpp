/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gfragoso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 23:25:28 by gfragoso          #+#    #+#             */
/*   Updated: 2025/03/22 00:41:52 by gfragoso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "../inc/HttpParser.hpp"
#include "../inc/AResponse.hpp"
#include <string>
#include <map>
#include <sys/time.h>
#include <sys/wait.h>

class CGI {
  private:
    HttpRequest _request;
    HttpResponse _response;
    const std::string _path;
    char **_cgiEnv;

    std::string run();
    std::string createOutput(pid_t, int *);
    
  public:
    CGI(HttpRequest &, HttpResponse &, const std::string &_path);
    ~CGI();

    short execute();
};

#endif