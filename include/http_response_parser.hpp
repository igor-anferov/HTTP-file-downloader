//
//  http_response_parser.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef http_response_parser_hpp
#define http_response_parser_hpp

#include <string>
#include <map>

class http_response_parse_error: public std::runtime_error {
public:
    http_response_parse_error();
};

struct http_response_parser {
    std::string version;
    std::string code;
    std::string code_description;
    std::map<std::string, std::string> headers;
    
    http_response_parser(const std::string & responce);
};

#endif /* http_response_parser_hpp */
