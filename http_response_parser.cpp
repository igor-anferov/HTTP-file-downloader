//
//  http_response_parser.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "http_response_parser.hpp"

#include <sstream>
#include <exception>

http_response_parse_error::http_response_parse_error(): std::runtime_error("Can't parse HTTP responce") {}

http_response_parser::http_response_parser(const std::string & responce) {
    
    std::stringstream ss(responce);
    std::string line;
    std::getline(ss, line);
    std::stringstream line_ss(line);
    
    // Starting line handling
    
    std::string protocol;
    
    if (!(line_ss >> protocol)) {
        throw http_response_parse_error();
    }
    
    if (protocol.substr(0, 5) != "HTTP/") {
        throw http_response_parse_error();
    }
    
    version = protocol.substr(5);
    
    if (!(line_ss >> code >> code_description)) {
        throw http_response_parse_error();
    }
    
    // Headers handling
    
    while(true) {
        getline(ss, line);
        if (!ss) {
            throw http_response_parse_error();
        }
        
        line.pop_back();
        
        if (line.empty()) {
            break;
        }
        
        std::string name;
        std::string value;
        
        size_t semicolon_pos;
        
        if ( (semicolon_pos = line.find(": ")) == std::string::npos) {
            throw http_response_parse_error();
        }
        
        name = line.substr(0, semicolon_pos);
        value = line.substr(semicolon_pos+2);
        
        if (name.empty() || value.empty()) {
            throw http_response_parse_error();
        }
        
        headers[name] = value;
    }
}
