//
//  file_to_download.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef file_to_download_hpp
#define file_to_download_hpp

#include <string>
#include "url.hpp"

struct file_to_download {
    URL url;
    std::string path;
    
    file_to_download() = default;
    file_to_download(const URL & url, const std::string & path);
};

#endif /* file_to_download_hpp */
