//
//  url.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef url_hpp
#define url_hpp

#include <netdb.h>

#include <string>

struct URL {
    struct sockaddr_in server_addr;
    std::string server;
    std::string resource;
    std::string file_path;
    std::string file_name;
    
    URL() = default;
    URL(const std::string & url);
};

#endif /* url_hpp */
