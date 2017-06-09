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
    std::string server;
    std::string path_to_file;
    struct sockaddr_in server_addr;
    
    URL() = default;
    URL(const std::string & url);
    std::string get_filename(void) const;
};

#endif /* url_hpp */
