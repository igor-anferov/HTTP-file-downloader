//
//  list_to_download.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 08.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef list_to_download_hpp
#define list_to_download_hpp

#include <vector>
#include <fstream>
#include <exception>
#include <string>
#include <sstream>
#include "url.hpp"

struct file_to_download {
    URL url;
    std::string path;
    
    file_to_download() = default;
    file_to_download(const URL & url, const std::string & path);
};

class list_to_download {
    std::vector<file_to_download> list;
    
public:
    list_to_download() = default;
    list_to_download(const std::string & filename);
    list_to_download(const std::string & url, const std::string & path);
    std::vector<file_to_download>::iterator begin();
    std::vector<file_to_download>::iterator end();
};

#endif /* list_to_download_hpp */
