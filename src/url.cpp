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

URL::URL(const std::string & url) {
    
    int port = 80;
    
    size_t server_pos;
    
    if ((server_pos = url.find("://")) == std::string::npos) {
        server_pos = 0;
    } else {
        if (url.substr(0, server_pos) != "http") {
            throw std::logic_error("Unsupported protocol");
        }
        server_pos += 3;
    }
    
    size_t path_to_file_pos;
    
    if ((path_to_file_pos = url.find('/', server_pos)) == std::string::npos) {
        throw std::logic_error("Path to file wasn't found in URL " + url);
    }
    
    server = url.substr(server_pos, path_to_file_pos - server_pos);
    path_to_file = url.substr(path_to_file_pos);
    
    if (path_to_file.empty()) {
        throw std::logic_error("URL \"" + url + "\" has empty path to file");
    }
    
    // Getting port
    
    size_t server_port;
    
    if ((server_port = server.find(":")) != std::string::npos) {
        port = std::stoi(server.substr(server_port + 1));
        server = server.substr(0, server_port);
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

std::string URL::get_filename(void) const {
    return delete_query(path_to_file.substr(path_to_file.find_last_of('/')+1));
}

std::string URL::get_path_to_file_without_query(void) const {
    return delete_query(path_to_file);
}

std::string URL::delete_query(const std::string & str) const {
    size_t query_pos;
    if ( (query_pos = str.find_first_of('?')) != std::string::npos) {
        return str.substr(0, query_pos);
    }
    return str;
}

