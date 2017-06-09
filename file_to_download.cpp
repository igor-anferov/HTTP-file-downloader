//
//  file_to_download.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "file_to_download.hpp"

file_to_download::file_to_download(const URL & url, const std::string & path) {
    this -> url = url;
    this -> path = path;
};
