//
//  http_request_creator.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef http_request_creator_hpp
#define http_request_creator_hpp

#include <string>

class http_request_creator {
    std::string request;
    
public:
    http_request_creator(const std::string & method, const std::string & url, const std::string & version = "1.1");
    void add_header(const std::string & name, const std::string & value);
    std::string get_request(void) const;
};

#endif /* http_request_creator_hpp */
