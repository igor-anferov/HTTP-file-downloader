//
//  http_request_creator.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "http_request_creator.hpp"

http_request_creator::http_request_creator(const std::string & method, const std::string & url, const std::string & version) {
    request += method + " " + url + " HTTP/" + version + "\r\n";
}

void http_request_creator::add_header(const std::string & name, const std::string & value) {
    request += name + ": " + value + "\r\n";
}

std::string http_request_creator::get_request(void) const {
    return request + "\r\n";
}
