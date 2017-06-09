//
//  url.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "url.hpp"

URL::URL(const std::string & url) {
    
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
}

std::string URL::get_filename(void) const {
    return path_to_file.substr(path_to_file.find_last_of('/')+1);
}
