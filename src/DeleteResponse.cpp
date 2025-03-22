/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: passunca <passunca@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 18:20:33 by passunca          #+#    #+#             */
/*   Updated: 2025/03/11 18:20:34 by passunca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/DeleteResponse.hpp"
#include "../inc/AResponse.hpp"

/**
 * @class DeleteResponse
 * @brief Handles HTTP DELETE requests and generates appropriate responses.
 */
 
/* ************************************************************************** */
/*                                Constructors                                */
/* ************************************************************************** */

/**
 * @brief Constructs a DeleteResponse object.
 * @param server Reference to the server configuration.
 * @param request Reference to the HTTP request.
 */
DeleteResponse::DeleteResponse(const Server &server, const HttpRequest &request)
    : AResponse(server, request, OK) {}

/**
 * @brief Copy constructor for DeleteResponse.
 * @param other Another DeleteResponse object to copy from.
 */
DeleteResponse::DeleteResponse(const DeleteResponse &other)
    : AResponse(other) {}

/**
 * @brief Destructor for DeleteResponse.
 */
DeleteResponse::~DeleteResponse() {}

/* ************************************************************************** */
/*                                  Getters                                   */
/* ************************************************************************** */

/**
 * @brief Generates the HTTP response for a DELETE request.
 * @return A string representing the HTTP response.
 */
std::string DeleteResponse::generateResponse() {
    setLocationRoute();

    short status = checkMethod();
    if (_status != OK)
        return getErrorPage(status);
    std::string path = getPath();

    _status = checkFile(path);
    if (_status != OK)
        return getErrorPage(status);

    if (!isDir(path)) { // Delete File
        _status = deleteFile(path);
        if (_status != OK)
            return getErrorPage(status);
    } else { // Is Directory
        if (isDirEmpty(path)) {
            _status = deleteDir(path);
            if (_status != OK)
                return getErrorPage(status);
        } else
		{
			_status = CONFLICT;
            return getErrorPage(status); // non-empty directory
		}
    }
    _response.status = NO_CONTENT; // Nginx status upon successfull deletion
    return (getResponseStr());
}

/**
 * @brief Checks if a directory is empty.
 * @param path The path to the directory.
 * @return True if the directory is empty, false otherwise.
 */
bool DeleteResponse::isDirEmpty(const std::string &path) const {
    DIR *dir = opendir(path.c_str());
    if (dir == NULL)
        return (false);

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL)
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            ++count; // Skip ".." and "."

    closedir(dir);
    return (count == 0);
}

/**
 * @brief Deletes a file at the specified path.
 * @param path The path to the file.
 * @return A status code indicating the result of the operation.
 */
short DeleteResponse::deleteFile(const std::string &path) {
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) != 0)
        return (NOT_FOUND);
    if (!(fileInfo.st_mode & S_IWUSR)) // No write permission
        return (FORBIDDEN);

    std::size_t fileSize = fileInfo.st_size;
    if (std::remove(path.c_str()) != 0) {
        if (errno == EACCES)
            return (FORBIDDEN);
        return (INTERNAL_SERVER_ERROR);
    } else {
        storageSize -= fileSize;
        return (OK);
    }
}

/**
 * @brief Deletes a directory at the specified path.
 * @param path The path to the directory.
 * @return A status code indicating the result of the operation.
 */
short DeleteResponse::deleteDir(const std::string &path) {
    if (rmdir(path.c_str()) != 0)
        return (INTERNAL_SERVER_ERROR);
    return (OK);
}
