//
//  url.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef url_hpp
#define url_hpp

#include <string>
#include <exception>

struct URL {
    std::string server;
    std::string path_to_file;
    
    URL(const std::string & url);
    std::string get_filename(void) const;
};

#endif /* url_hpp */
