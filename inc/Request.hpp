#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Utils.hpp"
#include <string>

typedef std::map<std::string, std::string> HeaderMap;

class Request {
	private:
		Method		_method;
		std::string	_host;
		std::string _userAgent;
		HeaderMap	_headers;

	public:

};

#endif