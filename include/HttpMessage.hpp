/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtimsina <rtimsina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 23:03:11 by mcutura           #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2024/03/24 11:27:12 by rtimsina         ###   ########.fr       */
=======
/*   Updated: 2024/03/21 20:49:25 by tvasilev         ###   ########.fr       */
>>>>>>> fe720d4 (directory default file)
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HTTPMESSAGE_HPP
# define WEBSERV_HTTPMESSAGE_HPP

#include <map>
#include <string>
#include <sstream> 
#include <iostream>
#include <unordered_map>
#include "CGI.hpp"
#include "Server_class.hpp"
#include "ServerConfig.hpp"

class CGI;

#define MAX_CHUNK_SIZE 1024

class HttpRequest {
    public:
        HttpRequest(char *raw_request, size_t len);
    	void parse();
    
    public:
        const std::string& getRawRequest() const;
        const std::string& getMethod() const;
        const std::string& getPath() const;
        void               setPath(std::string str);
        const std::string& getHttpVersion() const;
        const std::unordered_map<std::string, std::string>& getHeaders() const;
        const std::string& getBody() const;
        std::string get_query();

    private:
        std::string raw_request;
        std::string method;
        std::string path;
        std::string http_version;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
};

class HTTPResponse {
public:
    HTTPResponse() {};
    HTTPResponse(int status_code);

    void setHeader(const std::string& key, const std::string& value);

    void setBody(const std::string& body_content);

    std::string getRawResponse() const;

    void handle_cgi_get_response(HTTPResponse &resp, std::string& cgi_ret, Server& server);
    void handle_cgi_post_response(HTTPResponse& resp, std::string& cgi_ret, HttpRequest& request, Server& server);
    std::string send_cgi_response(CGI& cgi_handler, HttpRequest& request, Server& server);

private:
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;
};

std::string const get_status_message(int status);
std::string const generate_error_page(int status);
std::string const get_status_message_detail(int status);

////////////////////////////////////////////////////////////////////////////////
// --- METHODS ---

// Source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
std::string const get_status_message(int status);

////////////////////////////////////////////////////////////////////////////////
// --- INTERNALS ---

std::string const generate_error_page(int status);

#endif  // WEBSERV_HTTPMESSAGE_HPP
