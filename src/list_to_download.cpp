//
//  list_to_download.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 08.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "list_to_download.hpp"

#include <fstream>
#include <exception>
#include <stdexcept>
#include <sstream>

list_to_download::list_to_download(const std::string & filename) {
    std::fstream f(filename);
    
    if (!f.good()) {
        throw std::logic_error("File with list to download opening failed");
    }
    
    while (!f.eof()) {
        std::string file;
        std::getline(f, file);
        std::stringstream ss(file);
        std::string url;
        std::string path;
        
        if (ss >> url) {
            URL struct_url(url);
            if (!(ss >> path)) {
                path = struct_url.file_name;
            }
            list.push_back(file_to_download(struct_url, path));
        }
    }
}

list_to_download::list_to_download(const std::string & url, const std::string & path) {
    URL struct_url(url);
    if (path == "") {
        list.push_back(file_to_download(struct_url, struct_url.file_name));
    } else {
        list.push_back(file_to_download(struct_url, path));
    }
}

std::vector<file_to_download>::iterator list_to_download::begin() {
    return list.begin();
}

std::vector<file_to_download>::iterator list_to_download::end() {
    return list.end();
}
