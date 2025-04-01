/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gfragoso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 18:19:52 by passunca          #+#    #+#             */
/*   Updated: 2025/03/22 22:05:23 by gfragoso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/PostResponse.hpp"
#include "../inc/Utils.hpp"
#include "../inc/Webserv.hpp"
#include "../inc/CGI.hpp"

/**
 * @class PostResponse
 * @brief Handles HTTP POST requests and generates appropriate responses.
 *
 * This class is responsible for processing HTTP POST requests, including
 * checking methods, parsing HTTP data, validating body size, handling file
 * uploads, and triggering CGI scripts if necessary.
 */

/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Constructs a PostResponse object.
 * @param server Reference to the server configuration.
 * @param request Reference to the HTTP request.
 * @param clientFd File descriptor for the client connection.
 * @param epollFd File descriptor for epoll instance.
 */
PostResponse::PostResponse(const Server &server, const HttpRequest &request,
                           int clientFd, int epollFd)
    : AResponse(server, request, OK), _clientFd(clientFd), _epollFd(epollFd) {}

/**
 * @brief Copy constructor for PostResponse.
 * @param other Another PostResponse object to copy from.
 */
PostResponse::PostResponse(const PostResponse &other) : AResponse(other) {}

/**
 * @brief Destructor for PostResponse.
 */
PostResponse::~PostResponse() {}

/* ************************************************************************** */
/*                          Public Member Functions                           */
/* ************************************************************************** */

/**
 * @brief Generates a default HTML response for successful file uploads.
 * @return A string containing the HTML response.
 *
 * This function returns a simple HTML page indicating that the file upload
 * was successful, with a link to return to the index page.
 */
static std::string generateDefaultUploadResponse() {
    return "<!DOCTYPE html>\n"
           "<html lang=\"en\">\n"
           "<head>\n"
           "\t<meta charset=\"UTF-8\">\n"
           "\t<meta name=\"viewport\" content=\"width=device-width, "
           "initial-scale=1.0\">\n"
           "\t<link rel=\"icon\" href=\"assets/favicon.ico\" "
           "type=\"image/x-icon\">\n"
           "\t<link rel=\"stylesheet\" href=\"assets/css/style.css\">\n"
           "\t<title>Upload Successful</title>\n"
           "</head>\n"
           "<body>\n"
           "\t<h1>File Uploaded Successfully!</h1>\n"
           "\t<p>Your file has been uploaded.</p>\n"
           "\t<a href=\"index.html\">Back to Index</a>\n"
           "</body>\n"
           "</html>\n";
}

/**
 * @brief Generates the HTTP response for a POST request.
 * @return A string containing the HTTP response.
 *
 * This function processes the POST request by checking the method, parsing
 * the HTTP data, validating the body size, and handling file uploads. If
 * the request is not a CGI request, it performs form checks, body checks,
 * and file uploads, generating a default upload response upon success.
 * If the request is a CGI request, it triggers the CGI script.
 */
std::string PostResponse::generateResponse() {
    setLocationRoute();

    if ((_status = checkMethod()) != OK)
        return getErrorPage();
    if ((_status = parseHttp()) != OK)
        return getErrorPage();
    if ((_status = checkBodySize()) != OK)
        return getErrorPage();

    if (!isCGI()) {
        if ((_status = checkForm()) != OK)
            return getErrorPage();

        if ((_status = checkBody()) != OK)
            return getErrorPage();

        if ((_status = getFile()) != OK)
            return getErrorPage();

        if ((_status = uploadFile()) != OK)
            return getErrorPage();
        _response.body = generateDefaultUploadResponse();
		_status = CREATED;
        _response.status = CREATED;
    } else {
        std::string root = _server.getRoot(_locationRoute);
        std::string path = getPath();
		CGI cgi(_request,
            _response,
            root,
            path
        );
		if ((_status = cgi.generateResponse()) != OK)
			getErrorPage();
    }
    loadHeaders();

    return (getResponseStr());
}

/* ************************************************************************** */
/*                              Private Methods                               */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                parseHttp()                                 */
/* ************************************************************************** */

/**
 * @brief Parses the HTTP request headers.
 * @return An unsigned short representing the response status.
 *
 * Checks for the presence of specific headers such as "expect" and
 * "content-length". Sends a 100 Continue response if necessary and
 * validates the request headers.
 */
unsigned short PostResponse::parseHttp() {
    if (hasHeader("expect"))
        if (!send100continue())
            return (_status);
    if (!hasHeader("content-length") ||
        (hasHeader("transfer-encoding") && !isCGI()))
        _status = BAD_REQUEST;
    return _status;
}

/**
 * @brief Checks if a specific header is present in the request.
 * @param header The header to check for.
 * @return True if the header is present, false otherwise.
 */
bool PostResponse::hasHeader(const std::string &header) const {
    if (_request.headers.find(header) != _request.headers.end())
        return (true);
    return (false);
}

/**
 * @brief Sends a 100 Continue response to the client.
 * @return True if the response was sent successfully, false otherwise.
 *
 * Validates the "expect" header and sends a 100 Continue response
 * if the request is valid. Checks for content length and transfer
 * encoding headers to ensure the request is well-formed.
 */
bool PostResponse::send100continue() {
    if (_request.headers.find("expect")->second != "100-continue") {
        _status = BAD_REQUEST;
        return (false);
    }
    if (!hasHeader("content-length") && !hasHeader("transfer-encoding")) {
        _status = BAD_REQUEST;
        return (false);
    }
    if (hasHeader("content-length") &&
        string2number<ssize_t>(
            _request.headers.find("content-length")->second) >
            _server.getClientMaxBodySize()) {
        _status = PAYLOAD_TOO_LARGE;
        return (false);
    }
    ssize_t sent = send(_clientFd, "HTTP/1.1 100 Continue\r\n\r\n", 28, 0);
    if (sent < 0) {
        _status = INTERNAL_SERVER_ERROR;
        return (false);
    }

    return (true);
}

/* ************************************************************************** */
/*                                CheckBody()                                 */
/* ************************************************************************** */

/**
 * @brief Checks the body of the HTTP request for validity.
 * @return A short representing the response status.
 *
 * This function checks if the "content-type" header is present and if it
 * indicates a multipart form. It retrieves the boundary limit and extracts
 * the body content based on this limit. Returns BAD_REQUEST if the checks
 * fail, otherwise returns OK.
 */
short PostResponse::checkBody() {
    if (hasHeader("content-type") &&
        _request.headers.find("content-type")->second.find("multipart/") == 0) {
        _limit = getLimit();
        if (_limit.empty())
            return (BAD_REQUEST);
        _body = getBody(_limit);
        if (_body.empty())
            return (BAD_REQUEST);
        return (OK);
    }
    
    return (BAD_REQUEST);
}

/**
 * @brief Retrieves the boundary limit from the "content-type" header.
 * @return A string representing the boundary limit.
 *
 * This function searches for the "boundary=" parameter within the
 * "content-type" header and returns its value. If the header or boundary
 * is not found, it returns an empty string.
 */
const std::string PostResponse::getLimit() {
    std::multimap<std::string, std::string>::const_iterator contentTypeHeader =
        _request.headers.find("content-type");
    if (contentTypeHeader == _request.headers.end())
        return "";

    const std::string contentType = contentTypeHeader->second;
    if (contentType.empty())
        return "";

    std::size_t limitPositrion = contentType.find("boundary=");
    if (limitPositrion == std::string::npos)
        return "";

    const std::string limit = contentType.substr(limitPositrion + 9);

    return (limit);
}

/**
 * @brief Extracts the body content based on the boundary limit.
 * @param limit The boundary limit used to parse the body.
 * @return A vector of maps containing the parsed body fields.
 *
 * This function parses the request body using the provided boundary limit,
 * extracting each part into a map of headers and content. It returns a
 * vector of these maps, representing the multipart form data.
 */
const std::vector<std::multimap<std::string, std::string> >
PostResponse::getBody(const std::string &limit) {
    std::vector<std::multimap<std::string, std::string> > body;
    std::string fullDelimiter = "--" + limit;
    std::string endDelimiter = fullDelimiter + "--";

    std::size_t startLDelimiterPos = _request.body.find(fullDelimiter);
    if (startLDelimiterPos == std::string::npos)
        return (body);

    while (startLDelimiterPos != std::string::npos) {
        startLDelimiterPos += (fullDelimiter.length() + 2);
        std::size_t endDelimiterPos =
            _request.body.find(fullDelimiter, startLDelimiterPos);

        if (_request.body.find(endDelimiter, startLDelimiterPos) ==
            startLDelimiterPos)
            break;

        if (endDelimiterPos == std::string::npos)
            return (body);

        std::string subStr = _request.body.substr(
            startLDelimiterPos, (endDelimiterPos - startLDelimiterPos));
        std::multimap<std::string, std::string> subMap = getFields(subStr);
        if (subMap.empty())
            break;
        body.push_back(subMap);
        startLDelimiterPos = endDelimiterPos;
    }
    return (body);
}

/**
 * @brief Parses individual fields from a multipart body part.
 * @param str The string containing the multipart body part.
 * @return A map of headers and their corresponding values.
 *
 * This function extracts headers and content from a given multipart body
 * part string, returning them as a map. It identifies headers by searching
 * for colon-separated key-value pairs and includes the body content under
 * the "_File Contents" key.
 */
const std::multimap<std::string, std::string>
PostResponse::getFields(const std::string &str) {
    std::multimap<std::string, std::string> subMap;

    std::size_t headerEnd = str.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return (subMap);

    std::string headers = str.substr(0, (headerEnd + 2));
    std::string body = str.substr(headerEnd + 4);

    std::size_t startSep = 0;
    std::size_t endSep = headers.find("\r\n");

    while (endSep != std::string::npos) {
        std::string field = headers.substr(startSep, (endSep - startSep));
        std::size_t colonPos = field.find(":");
        if (colonPos != std::string::npos) {
            std::string left = field.substr(0, colonPos);
            std::string right = field.substr(colonPos + 2);
            if (!left.empty() && !right.empty())
                subMap.insert(std::make_pair(left, right));
        }

        startSep = (endSep + 2);
        endSep = headers.find("\r\n", startSep);
    }

    subMap.insert(
        std::make_pair("_File Contents", body.substr(0, (body.length() - 2))));

    return (subMap);
}

/* ************************************************************************** */
/*                                 getFile()                                  */
/* ************************************************************************** */
/**
 * @brief Extracts file information from the HTTP request body.
 * @return A short representing the response status.
 *
 * This function retrieves the file information from the parsed HTTP request
 * body, including the file name, path, type, and content. It checks for the
 * presence of necessary headers and fields, returning INTERNAL_SERVER_ERROR
 * if any required information is missing.
 */
short PostResponse::getFile() {
    if (_body.empty())
        return (INTERNAL_SERVER_ERROR);

    std::multimap<std::string, std::string>::iterator contentDispIt =
        _body[0].find("Content-Disposition");
    if (contentDispIt == _body[0].end())
        return (INTERNAL_SERVER_ERROR);
    std::string contentDisp = contentDispIt->second;

    _file2upload.name = getFieldValue(contentDisp, "name");
    _file2upload.path = getFieldValue(contentDisp, "filename");
    std::multimap<std::string, std::string>::iterator contentTypeIt =
        _body[0].find("Content-Type");
    if (contentTypeIt == _body[0].end())
        return (INTERNAL_SERVER_ERROR);
    _file2upload.type = contentTypeIt->second;

    std::multimap<std::string, std::string>::iterator fileContentsIt =
        _body[0].find("_File Contents");
    if (fileContentsIt == _body[0].end())
        return (INTERNAL_SERVER_ERROR);
    _file2upload.content = fileContentsIt->second;

    return (OK);
}

/**
 * @brief Retrieves the value of a specific field from a header string.
 * @param header The header string to search within.
 * @param field The field name to retrieve the value for.
 * @return A string containing the value of the specified field.
 *
 * This function searches for a specified field within a header string and
 * returns its value. It handles both quoted and unquoted field values,
 * returning an empty string if the field is not found.
 */
std::string PostResponse::getFieldValue(const std::string &header,
                                        const std::string &field) {
    std::string key = (field + "=");
    std::size_t start = header.find(key);

    if (start == std::string::npos)
        return ("");

    start += key.length();

    bool quoted = false;
    if (header[start] == '"') {
        quoted = true;
        ++start;
    }

    std::size_t end = start;
    while (end < header.length()) {
        if (quoted) {
            if (header[end] == '"') {
                quoted = false;
                break;
            }
        } else if (header[end] == ';')
            break;
        ++end;
    }

    if (quoted && (end < header.length() && (header[end] == '"')))
        ++end;

    return (header.substr(start, (end - start)));
}

/* ************************************************************************** */
/*                                CheckForm()                                 */
/* ************************************************************************** */

/**
 * @brief Checks the form data in the HTTP request.
 * @return A short representing the response status.
 *
 * This function verifies the presence and type of the "content-type" header
 * in the HTTP request. It ensures that the content type is
 * "multipart/form-data", which is required for processing form data. If the
 * header is missing or the content type is not supported, it returns an
 * appropriate error status.
 */
short PostResponse::checkForm() {
    std::map<std::string, std::string>::iterator it =
        _request.headers.find("content-type");

    if (it == _request.headers.end())
        return (BAD_REQUEST);

    if (it->second.find("multipart/form-data") == std::string::npos)
        return (UNSUPPORTED_MEDIA_TYPE);

    return (OK);
}

/* ************************************************************************** */
/*                                uploadFile()                                */
/* ************************************************************************** */

static bool createDirectory(const std::string &path);
static long getFileSize(std::string &target);

/**
 * @brief Uploads a file from the HTTP request to the server.
 * @return A short representing the response status.
 *
 * This function retrieves the file information from the HTTP request and
 * attempts to upload it to the server. It checks for the presence of an
 * upload directory and creates it if necessary. Returns an appropriate
 * error status if the file cannot be uploaded.
 */
short PostResponse::uploadFile() {
    short status = getFile();
    if (status != OK)
        return (status);

    std::string dir = _server.getUploadStore(_locationRoute);
    if (dir.empty())
        dir = getPath(_server.getRoot(_locationRoute),
                      "default_upload_directory");
    if (dir.at(dir.length() - 1) != '/') // Check if dir ends with /
        dir += "/";
    if (!createDirectory(dir))
        return (INTERNAL_SERVER_ERROR);
    std::string path = (dir + _file2upload.path);
    long fileSize = getFileSize(path);

    int fd =
        open(path.c_str(), (O_WRONLY | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR));
    if (fd == -1)
        return (FORBIDDEN);

    std::size_t bytes2write = _file2upload.content.length();
    if ((storageSize += bytes2write) > MAX_STORAGE_SIZE) {
        close(fd);
        return (PAYLOAD_TOO_LARGE);
    }

    if (write(fd, _file2upload.content.c_str(), bytes2write) == -1) {
		close(fd);
		return (FORBIDDEN);
    }

	if (close(fd) == -1)
		return (INTERNAL_SERVER_ERROR);

	storageSize += (getFileSize(path) - fileSize);

    return (OK);
}

/**
 * @brief Creates a directory at the specified path.
 * @param path The path where the directory should be created.
 * @return True if the directory was created successfully or already exists,
 * false otherwise.
 *
 * This function attempts to create a directory at the given path with
 * permissions set to 777. If the directory already exists, it returns true.
 * If the directory cannot be created, it returns false.
 */
static bool createDirectory(const std::string &path) {
    if (mkdir(path.c_str(), 777) == 0)
        return (true);
    if (errno == EEXIST)
        return (true);
    return (false);
}

/**
 * @brief Retrieves the size of a file.
 * @param target The path to the file whose size is to be determined.
 * @return The size of the file in bytes, or 0 if the file does not exist or an
 * error occurs.
 *
 * This function uses the `stat` system call to obtain information about the
 * specified file. If successful, it returns the size of the file in bytes. If
 * the file does not exist or an error occurs during the `stat` call, it returns
 * 0.
 */
static long getFileSize(std::string &target) {
    struct stat fileInfo;

    if (stat(target.c_str(), &fileInfo) == 0)
        return (fileInfo.st_size);
    return (0);
}
