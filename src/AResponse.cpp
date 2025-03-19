/**
 * @defgroup AResponseModule AResponse Class Module
 * @{
 *
 * This module provides the AResponse class, which serves as an abstract base
 * class for handling HTTP responses. It includes constructors, a destructor,
 * and an assignment operator. The class is initialized with server
 * configuration and HTTP request details.
 *
 * @version 1.0
 */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:09:55 by passunca          #+#    #+#             */
/*   Updated: 2025/03/10 16:47:46 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/AResponse.hpp"

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Constructs an AResponse object.
 * @param server The server configuration.
 * @param request The HTTP request.
 *
 * Initializes the AResponse object with the given server configuration
 * and HTTP request.
 */
AResponse::AResponse(const Server &server, const HttpRequest &request, const short errorStatus)
    : _request(request), _server(server), _status(errorStatus) {}

/**
 * @brief Copy constructor for AResponse.
 * @param other The AResponse object to copy from.
 *
 * Initializes a new AResponse object as a copy of the given object.
 */
AResponse::AResponse(const AResponse &other)
    : _request(other._request), _response(other._response),
      _server(other._server), _locationRoute(other._locationRoute) {}

/**
 * @brief Destructor for AResponse.
 */
AResponse::~AResponse() {}

/* ************************************************************************** */
/*                                 Operators                                  */
/* ************************************************************************** */

/**
 * @brief Assignment operator for AResponse.
 * @param other The AResponse object to assign from.
 * @return A reference to this AResponse object.
 *
 * The assignment operator is intentionally left empty as this is an
 * abstract base class.
 */
const AResponse &AResponse::operator=(const AResponse &other) {
    (void)other;
    // Assignment intentionally does nothing for abstract base class
    return (*this);
}

/* ************************************************************************** */
/*                             PROTECTED METHODS                              */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                  Checkers                                  */
/* ************************************************************************** */

/**
 * @brief Checks if the request is for a CGI script.
 * @return True if the request URI matches the CGI extension, false otherwise.
 *
 * This method determines if the current request is intended for a CGI script
 * by comparing the URI's extension with the configured CGI extension for the
 * server.
 */
bool AResponse::isCGI() const {
    std::string cgiExt = _server.getCgiExt(_locationRoute);
    size_t dotPos = cgiExt.find_last_of('.');
    if ((!cgiExt.empty()) && (_request.uri.substr(dotPos) == cgiExt))
        return (true);
    return (false);
}

/**
 * @brief Checks if there is a return directive for the current location route.
 * @return True if a return directive is present, false otherwise.
 *
 * This method checks if the server configuration for the current location
 * route includes a return directive, which typically indicates a redirection.
 * It returns true if a valid return directive is found, otherwise false.
 */
bool AResponse::hasReturn() const {
    std::pair<short, std::string> redir = _server.getReturn(_locationRoute);

    if ((redir.first == -1) || (redir.second.empty()))
        return (false);
    return (true);
}

/**
 * @brief Loads the return directive into the response.
 *
 * This method retrieves the return directive for the current location route
 * from the server configuration and sets the response body and status code
 * accordingly. It also loads the necessary HTTP headers, including the
 * "Location" header for redirection if applicable, and sets the "Content-Type"
 * header to "application/octet-stream".
 */
void AResponse::loadReturn() {
    std::pair<short, std::string> redir = _server.getReturn(_locationRoute);
    _response.body = redir.second;
    _response.status = redir.first;
    loadHeaders();

    if ((redir.first == MOVED_PERMANENTLY) || (redir.first == FOUND))
        _response.headers.insert(std::make_pair("Location", redir.second));
    _response.headers.insert(
        std::make_pair("Content-Type", "application/octet-stream"));
}

/**
 * @brief Checks if the specified path is a directory.
 * @param path The path to check.
 * @return True if the path is a directory, false otherwise.
 *
 * This method uses the stat function to determine if the given path
 * corresponds to a directory. It retrieves the file status information
 * and checks the file type to see if it is a directory.
 */
bool AResponse::isDir(const std::string &path) const {
    struct stat info;

    stat(path.c_str(), &info);
    if ((info.st_mode & S_IFMT) == S_IFDIR) // is Directory?
        return (true);
    return (false);
}

const std::string AResponse::getIndexFile(const std::string &path) const {
    std::vector<std::string> indexFiles = _server.getIndex(_locationRoute);
    std::vector<std::string>::const_iterator it;
    for (it = indexFiles.begin(); it != indexFiles.end(); it++) {
        std::string file = getPath(path, *it);
        if (checkFile(path) == OK)
            return (file);
    }
    return ("");
}

bool AResponse::hasAutoIndex() const {
    if (_server.getAutoIdx(_locationRoute) == TRUE)
        return (true);
    return (false);
}

/**
 * @brief Checks if the HTTP method is allowed.
 * @return A status code indicating if the method is allowed or not.
 *
 * This method checks if the HTTP method of the request is within the
 * allowed methods for the server configuration and location route.
 */
short AResponse::checkMethod() const {
    const std::set<Method> allowedMethods =
        _server.getValidMethods(_locationRoute);
    std::set<Method>::const_iterator it = allowedMethods.find(_request.method);
    if (it == allowedMethods.end())
        return (METHOD_NOT_ALLOWED);
    return (OK);
}

/**
 * @brief Checks if the request body size exceeds the maximum allowed size.
 * @return A status code indicating if the body size is acceptable or too
 * large.
 *
 * This method compares the size of the HTTP request body with the maximum
 * allowed size specified in the server configuration for the current
 * location route. If the body size exceeds the limit, it returns a status
 * code indicating that the payload is too large; otherwise, it returns a
 * status code indicating that the body size is acceptable.
 */
short AResponse::checkBodySize() const {
    std::size_t bodySize = _server.getClientMaxBodySize();

    if (_request.body.size() > bodySize)
        return (PAYLOAD_TOO_LARGE);
    return (OK);
}

/**
 * @brief Checks the status of a file at the given path.
 * @param path The path to the file to check.
 * @return A status code indicating the result of the check.
 *
 * This method checks the status of a file at the specified path. It
 * verifies if the file exists, if it is accessible, and whether it is a
 * directory or a regular file. The method returns a status code indicating
 * whether the file was found, if access is forbidden, or if the file type
 * is incorrect.
 */
short AResponse::checkFile(const std::string &path) const {
    struct stat info;

    if (stat(path.c_str(), &info) != 0) { // File does not exist
        if (errno == ENOENT)
            return (NOT_FOUND);
        else if (errno == EACCES)
            return (FORBIDDEN);
    }
    // check if file is a directory (finishes with /)
    bool expectDir = !path.empty() && path[path.size() - 1] == '/';
    if (expectDir && (info.st_mode & S_IFMT) != S_IFDIR)
        return (NOT_FOUND);
    // Check if its not a directory or a regular file
    if ((!S_ISDIR(info.st_mode) && expectDir) || !S_ISREG(info.st_mode))
        return (FORBIDDEN);
    return (OK);
}

/* **************************************************************************
 */
/*                                  Getters */
/* **************************************************************************
 */

/**
 * @brief Constructs the full path for the current request.
 * @return A string representing the combined full path.
 *
 * This method retrieves the root directory from the server configuration
 * for the current location route and combines it with the request URI to
 * construct the full path. It ensures that the path is correctly formatted
 * with a single '/' character between the root and the URI.
 */
const std::string AResponse::getPath() const {
    std::string root = _server.getRoot(_locationRoute);
    return (getPath(root, _request.uri));
}

/**
 * @brief Constructs the full path by combining the root and relative path.
 * @param root The root directory path.
 * @param path The relative path to be appended to the root.
 * @return A string representing the combined full path.
 *
 * This method constructs the full path by appending the relative path to
 * the root directory. It ensures that there is exactly one '/' character
 * between the root and the relative path. If the relative path is empty,
 * the method returns the root directory.
 */
const std::string AResponse::getPath(const std::string &root,
                                     const std::string &path) const {
    if (path.empty())
        return (root);
    if (((path.at(path.size() - 1) == '/') && (root.at(0) != '/')) ||
        ((path.at(path.size() - 1) != '/') && (root.at(0) == '/')))
        return (root + path);
    else
        return (root + "/" + path);
}

/*
 * @brief Retrieves the last modified date of a file.
 * @param path The path to the file.
 * @return A string representing the last modified date in GMT format.
 *
 * This method retrieves the last modified date of the specified file
 * and returns it as a string formatted according to the HTTP-date
 * specification (RFC 7231), in GMT.
 */
std::string AResponse::getLastModifiedDate(const std::string &path) const {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) != 0)
        return "";
    struct tm *tm = gmtime(&fileStat.st_mtime);
    char dateBuf[30];
    std::strftime(dateBuf, sizeof(dateBuf), "%a, %d %b %Y %H:%M:%S GMT", tm);
    return (std::string(dateBuf));
}

/* ************************************************************************** */
/*                                   Utils */
/* ************************************************************************** */

/**
 * @brief Initializes a map of HTTP status codes to their corresponding
 * messages.
 * @return A map where the key is the HTTP status code and the value is the
 * status message.
 *
 * This function creates and returns a map that associates HTTP status codes
 * with their respective textual descriptions. The map includes status codes
 * for informational responses, successful responses, redirection messages,
 * client error responses, and server error responses.
 */
static std::map<short, std::string> initStatusMessages() {
    std::map<short, std::string> m;
    m.insert(std::make_pair(100, "Continue"));
    m.insert(std::make_pair(101, "Switching Protocols"));

    m.insert(std::make_pair(200, "OK"));
    m.insert(std::make_pair(201, "Created"));
    m.insert(std::make_pair(202, "Accepted"));
    m.insert(std::make_pair(203, "Non-Authoritative Information"));
    m.insert(std::make_pair(204, "No Content"));
    m.insert(std::make_pair(205, "Reset Content"));
    m.insert(std::make_pair(206, "Partial Content"));

    m.insert(std::make_pair(300, "Multiple Choices"));
    m.insert(std::make_pair(301, "Moved Permanently"));
    m.insert(std::make_pair(302, "Found"));
    m.insert(std::make_pair(303, "See Other"));
    m.insert(std::make_pair(304, "Not Modified"));
    m.insert(std::make_pair(305, "Use Proxy"));
    m.insert(std::make_pair(307, "Temporary Redirect"));
    m.insert(std::make_pair(308, "Permanent Redirect"));

    m.insert(std::make_pair(400, "Bad Request"));
    m.insert(std::make_pair(401, "Unauthorized"));
    m.insert(std::make_pair(402, "Payment Required"));
    m.insert(std::make_pair(403, "Forbidden"));
    m.insert(std::make_pair(404, "Not Found"));
    m.insert(std::make_pair(405, "Method Not Allowed"));
    m.insert(std::make_pair(406, "Not Acceptable"));
    m.insert(std::make_pair(407, "Proxy Authentication Required"));
    m.insert(std::make_pair(408, "Request Timeout"));
    m.insert(std::make_pair(409, "Conflict"));
    m.insert(std::make_pair(410, "Gone"));
    m.insert(std::make_pair(411, "Length Required"));
    m.insert(std::make_pair(412, "Precondition Failed"));
    m.insert(std::make_pair(413, "Payload Too Large"));
    m.insert(std::make_pair(414, "URI Too Long"));
    m.insert(std::make_pair(415, "Unsupported Media Type"));
    m.insert(std::make_pair(416, "Range Not Satisfiable"));
    m.insert(std::make_pair(417, "Expectation Failed"));
    m.insert(std::make_pair(421, "Misdirected Request"));
    m.insert(std::make_pair(422, "Unprocessable Content"));
    m.insert(std::make_pair(426, "Upgrade Required"));

    m.insert(std::make_pair(500, "Internal Server Error"));
    m.insert(std::make_pair(501, "Not Implemented"));
    m.insert(std::make_pair(502, "Bad Gateway"));
    m.insert(std::make_pair(503, "Service Unavailable"));
    m.insert(std::make_pair(504, "Gateway Timeout"));
    m.insert(std::make_pair(505, "HTTP Version Not Supported"));
    return m;
}

/*
 * @brief A constant map of HTTP status codes to their corresponding
 * messages.
 *
 * This map is initialized using the initStatusMessages function and
 * provides a convenient way to look up the textual description of an HTTP
 * status code.
 */
const std::map<short, std::string> STATUS_MESSAGES = initStatusMessages();

/**
 * @brief Constructs the HTTP response string.
 * @return A string representing the complete HTTP response.
 *
 * This method constructs and returns the HTTP response string, which
 * includes the status line, headers, and body. The status line is composed
 * of the HTTP version, status code, and status message. Headers are
 * appended in the format "Header-Name: Header-Value", followed by the
 * response body.
 */
const std::string AResponse::getResponseStr() const {
    std::map<short, std::string>::const_iterator itStat =
        STATUS_MESSAGES.find(_response.status);
    std::string msg = (itStat != STATUS_MESSAGES.end()) ? itStat->second : "";

    std::string headerStr;
    std::multimap<std::string, std::string>::const_iterator itH;
    for (itH = _response.headers.begin(); itH != _response.headers.end();
         ++itH) {
        headerStr += itH->first + ": " + itH->second + "\r\n";
    }

    return "HTTP/1.1 " + number2string<short>(_response.status) + " " + msg +
           "\r\n" + headerStr + "\r\n" + _response.body;
}

/**
 * @brief Initializes a map of MIME types to their corresponding content
 * types.
 * @return A map where the key is the file extension and the value is the
 * MIME type.
 *
 * This function creates and returns a map that associates file extensions
 * with their respective MIME types. The map includes MIME types for text,
 * image, audio/video, application, Microsoft, and other formats.
 */
static std::map<std::string, std::string> initMimeTyes() {
    std::map<std::string, std::string> mimeTypes;

    // Text formats
    mimeTypes["html"] = "text/html";
    mimeTypes["htm"] = "text/html";
    mimeTypes["css"] = "text/css";
    mimeTypes["csv"] = "text/csv";
    mimeTypes["txt"] = "text/plain";
    mimeTypes["xml"] = "application/xml";

    // Image formats
    mimeTypes["png"] = "image/png";
    mimeTypes["jpg"] = "image/jpeg";
    mimeTypes["jpeg"] = "image/jpeg";
    mimeTypes["gif"] = "image/gif";
    mimeTypes["bmp"] = "image/bmp";
    mimeTypes["ico"] = "image/x-icon";

    // Audio/Video formats
    mimeTypes["mp3"] = "audio/mpeg";
    mimeTypes["wav"] = "audio/wav";
    mimeTypes["mp4"] = "video/mp4";
    mimeTypes["avi"] = "video/x-msvideo";
    mimeTypes["mov"] = "video/quicktime";

    // Application formats
    mimeTypes["json"] = "application/json";
    mimeTypes["js"] = "application/javascript";
    mimeTypes["pdf"] = "application/pdf";
    mimeTypes["zip"] = "application/zip";
    mimeTypes["tar"] = "application/x-tar";
    mimeTypes["gz"] = "application/gzip";
    mimeTypes["exe"] = "application/octet-stream";
    mimeTypes["bin"] = "application/octet-stream";

    // Microsoft formats
    mimeTypes["doc"] = "application/msword";
    mimeTypes["docx"] = "application/"
                        "vnd.openxmlformats-officedocument.wordprocessingml."
                        "document";
    mimeTypes["xls"] = "application/vnd.ms-excel";
    mimeTypes["xlsx"] = "application/"
                        "vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mimeTypes["ppt"] = "application/vnd.ms-powerpoint";
    mimeTypes["pptx"] = "application/"
                        "vnd.openxmlformats-officedocument.presentationml."
                        "presentation";

    // Other formats
    mimeTypes["svg"] = "image/svg+xml";
    mimeTypes["woff"] = "font/woff";
    mimeTypes["woff2"] = "font/woff2";

    return mimeTypes;
}

/**
 * @brief Generates a default error page for a given HTTP status code.
 * @param stat The HTTP status code for which the error page is generated.
 * @return A string containing the HTML content of the default error page.
 *
 * This function creates a default error page in HTML format for the
 * specified HTTP status code. It retrieves the status message associated
 * with the code and constructs an HTML page with a title and message
 * indicating the error. The page includes basic styling to center the text
 * and displays the server name at the bottom.
 */
static std::string loadDefaultErrorPage(short stat) {
    std::map<short, std::string>::const_iterator itStat =
        STATUS_MESSAGES.find(stat);
    std::string msg = (itStat != STATUS_MESSAGES.end()) ? itStat->second : "";
    std::string res = "<!DOCTYPE html>\n"
                      "<html lang=\"en\">\n"
                      "<head>\n"
                      "\t<meta charset=\"UTF-8\">\n"
                      "\t<meta name=\"viewport\" "
                      "content=\"width=device-width, "
                      "initial-scale=1.0\">\n"
                      "\t<title>" +
                      msg +
                      "</title>\n"
                      "\t<style>\n"
                      "\t\th1, p {\n"
                      "\t\t\ttext-align: center;\n"
                      "\t\t}\n"
                      "\t</style>\n"
                      "</head>\n"
                      "<body>\n"
                      "\t<div>\n"
                      "\t\t<h1>" +
                      number2string<short>(stat) + " " + msg +
                      "</h1>\n"
                      "<hr>\n"
                      "\t\t<p>" +
                      SERVER_NAME +
                      "</p>\n"
                      "\t</div>\n"
                      "</body>\n"
                      "</html>";
    return res;
}

static std::string getDirName(const std::string &path) {
    std::string dirName;
    std::string::size_type endPos = path.find_last_not_of('/');
    if (endPos == std::string::npos)
        dirName = path;
    std::string::size_type pos = path.find_last_of('/', endPos);
    if (pos != std::string::npos)
        dirName = path.substr(pos, (endPos - pos + 1));

    return (dirName + "/");
}

static std::string getFileSize(const std::string &path) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) != 0)
        return "";
    size_t size = fileStat.st_size;
    std::string sizeBuffer =
        (S_ISDIR(fileStat.st_mode) ? "-" : number2string<size_t>(size));
    return sizeBuffer;
}

std::string AResponse::addFileEntry(std::string &name,
                                    const std::string &path) {
    std::string fullPath = getPath(path, name);
    std::string date = getLastModifiedDate(fullPath);
    std::string size = getFileSize(fullPath);
    std::string displayName = name;
    if (isDir(path))
        displayName += '/';
    if (displayName.length() > 51)
        displayName = displayName.substr(0, 48) + "..>";
    if ((name != "./") && (name != "../"))
        name = getPath(_request.uri, name);
    std::string blanksL;
    if (displayName.length() < 51)
        blanksL =
            std::string((51 - displayName.length()), ' '); // Pad with blanks
    std::stringstream fileEntry;
    std::string blanksR = "                  ";
    if (displayName == "../")
        fileEntry << "<a href=\"" + name + "\">" + displayName + "</a> \n";
    else
        fileEntry << "<a href=\"" + name + "\">" + displayName + "</a> " +
                         blanksL + date + blanksR + size + "\n";
    return (fileEntry.str());
}

short AResponse::loadDirectoryListing(const std::string &path) {
    DIR *dir = opendir(path.c_str());
    if (dir == NULL)
        return (FORBIDDEN);

    std::string dirName = getDirName(path);
    _response.body = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " +
                     dirName + "</title>\n</head>\n<body>\n<h1>Index of " +
                     dirName + "</h1>\n<hr>\n<pre>";

    struct dirent *entry;
    std::vector<std::string> dirs;
    std::vector<std::string> files;
    while ((entry = readdir(dir)) != NULL) {
        if (isDir(getPath(path, entry->d_name)))
            dirs.push_back(entry->d_name);
        else
            files.push_back(entry->d_name);
    }
    // Sort alphabetically
    std::sort(dirs.begin(), dirs.end());
    std::sort(files.begin(), files.end());
    dirs.erase(dirs.begin()); // delete '.'

    std::vector<std::string>::iterator it;
    for (it = dirs.begin(); it != dirs.end(); it++) {
        std::string entryName = *it;
        _response.body += addFileEntry(entryName, path);
    }
    for (it = files.begin(); it != files.end(); it++) {
        std::string entryName = *it;
        _response.body += addFileEntry(entryName, path);
    }
    _response.body += "</pre>\n<hr></body>\n</html>\n";
    closedir(dir);
    loadHeaders();
    _response.headers.insert(std::make_pair("Content-Type", "text/html"));

    return (OK);
}

/* **************************************************************************
 */
/*                                  Setters */
/* **************************************************************************
 */

/**
 * @brief Sets the MIME  type for the response based on the file extension.
 * @param path The path to the file whose MIME type is to be determined.
 *
 * This method determines the MIME type of a file based on its extension
 * and sets the "Content-Type" header in the response accordingly. If the
 * file extension is not recognized, it defaults to
 * "application/octet-stream".
 *
 * @note MIME (Multipurpose Internet Mail Extensions)
 */
void AResponse::setMimeType(const std::string &path) {
    static std::map<std::string, std::string> mimeTs = initMimeTyes();

    std::size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string ext = path.substr(dotPos + 1);
        std::map<std::string, std::string>::iterator it = mimeTs.find(ext);
        if (it != mimeTs.end()) {
            _response.headers.insert(
                std::make_pair("Content-Type", it->second));
            return;
        }
    }
    _response.headers.insert( // Nginx Defaults
        std::make_pair("Content-Type", "application/octet-stream"));
}

/**
 * @brief Loads the HTTP headers for the response.
 *
 * This method populates the HTTP headers for the response object. It sets
 * the "Connection" header to "keep-alive", the "Content-Length" header if
 * the response body is not empty, the "Date" header with the current HTTP
 * date, the "Server" header with the server name, and the "Cache-Control"
 * header to "no-cache".
 */
void AResponse::loadHeaders() {
    _response.headers.insert(std::make_pair("Connection", "keep-alive"));
    if (_response.body.size() > 0)
        _response.headers.insert(std::make_pair(
            "Content-Length",
            number2string<unsigned long>(_response.body.size())));
    _response.headers.insert(std::make_pair("Date", getHttpDate()));
    _response.headers.insert(std::make_pair("Server", SERVER_NAME));
    _response.headers.insert(std::make_pair("Cache-Control", "no-cache"));
}

/**
 * @brief Retrieves the error page for a given HTTP status code.
 * @param errStat The HTTP status code for which the error page is
 * retrieved.
 * @return A string representing the complete HTTP response with the error
 * page.
 *
 * This method retrieves the error page associated with the specified HTTP
 * status code. It first checks if a custom error page is configured for the
 * status code and attempts to load it. If the custom error page is not
 * found or cannot be loaded, a default error page is generated. The method
 * then loads the necessary HTTP headers and constructs the complete HTTP
 * response string.
 */
const std::string AResponse::getErrorPage() {
    static std::map<short, std::string> errPages =
        _server.getErrorPages(_locationRoute);
    _response.status = _status;

    std::map<short, std::string>::const_iterator it = errPages.find(_status);
    if (it != errPages.end()) {
        std::string path = getPath(_server.getRoot(_locationRoute), it->second);
        if (checkFile(path) == OK) {
            std::ifstream file(path.c_str());
            _response.body.assign(std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>());
        }
    }
    if (_response.body.empty())
        _response.body = loadDefaultErrorPage(_status);
    loadHeaders();
    return (getResponseStr());
}

/**
 * @brief Sets the location route for the current request.
 *
 * This method determines the best matching location route for the current
 * HTTP request URI from the server's configured locations. It first checks
 * for an exact match of the request URI in the server's locations. If no
 * exact match is found, it searches for the longest matching prefix among
 * the available location routes. The location route is then set to the
 * best match found.
 */
void AResponse::setLocationRoute() {
    std::map<std::string, Location> serverLoci = _server.getLocations();

    std::map<std::string, Location>::iterator it =
        serverLoci.find(_request.uri);
    if (it != serverLoci.end()) {
        _locationRoute = it->first;
        return;
    }

    std::size_t bestMatchLen = 0;
    std::string bestMatchRoute = "";
    for (it = serverLoci.begin(); it != serverLoci.end(); ++it) {
        if (_request.uri.compare(0, it->first.size(), it->first) == 0) {
            std::size_t len = it->first.size();
            if (len > bestMatchLen) {
                bestMatchLen = len;
                bestMatchRoute = it->first;
            }
        }
    }
    _locationRoute = bestMatchRoute;
}

short AResponse::getStatus() const {
	return _status;
}

/** @} */
