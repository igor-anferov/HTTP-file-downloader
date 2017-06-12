//
//  url.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "url.hpp"

#include <exception>
#include <stdexcept>
#include <cstring>
#include <regex>

URL::URL(const std::string & url) {
    int port = 80;
    
    // Parsing url
    
    std::regex url_pattern("^(?:([[:alnum:]]+)://)?([^/:]+)(?::([[:digit:]]+))?(((?:/(?:[^\?]+/)*)([^\?]+))?(?:.*))$");
    std::match_results<const char *> matches;
    std::regex_search(url.c_str(), matches, url_pattern);

    if (matches[0].str().empty()) {
        throw std::logic_error("Can't parse URL " + url);
    }
    if (!matches[1].str().empty() && matches[1].str() != "http") {
        throw std::logic_error("Unsupported protocol: " + matches[1].str());
    }
    server = matches[2].str();
    if (!matches[3].str().empty()) {
        port = std::stoi(matches[3].str());
    }
    resource = matches[4].str();
    if (resource.empty()) {
        resource = "/";
    }
    file_path = matches[5].str();
    if (file_path.empty()) {
        file_path = server;
    }
    file_name = matches[6].str();
    if (file_name.empty()) {
        file_name = server;
    }

    // Getting server address
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    struct hostent * serv = gethostbyname(server.c_str());
    if (serv == NULL) {
        throw std::runtime_error("Can't translate server's domain name \"" + server + "\" to IP address");
    }
    
    memcpy(&server_addr.sin_addr.s_addr, serv -> h_addr_list[0], 4);
}
