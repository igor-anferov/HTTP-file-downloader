//
//  list_to_download.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 08.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "list_to_download.hpp"

file_to_download::file_to_download(const std::string & url, const std::string & path) {
    this -> url = url;
    this -> path = path;
};

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
            if (!(ss >> path)) {
                path = "";
            }
            list.push_back(file_to_download(url, path));
        }
    }
}

list_to_download::list_to_download(const std::string & url, const std::string & path) {
    list.push_back(file_to_download(url, path));
}

std::vector<file_to_download>::iterator list_to_download::begin() {
    return list.begin();
}

std::vector<file_to_download>::iterator list_to_download::end() {
    return list.end();
}
