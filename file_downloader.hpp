//
//  file_downloader.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef file_downloader_hpp
#define file_downloader_hpp

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <exception>
#include <iostream>

#include "file_to_download.hpp"
#include "http_request_creator.hpp"
#include "http_response_parser.hpp"

#define BUF_SIZE 8192

class file_downloader {
    file_to_download file;
    int part_count;
    size_t file_size;
    
public:
    file_downloader(file_to_download file, const int part_count) {
        this -> file = file;
        this -> part_count = part_count;
        
        std::cout << "Trying to download " + file.url.server + file.url.path_to_file << std::endl;
        
        while (true) { // In case of redirecting, till response is 2XX
            
            // Getting file size and checking acceptance of ranges
            
            int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd == -1) {
                throw std::runtime_error("Socket creation fail");
            }
            
            std::cout << "Connection... ";
            
            if ( connect(socket_fd,
                         (struct sockaddr *) &file.url.server_addr,
                         sizeof(file.url.server_addr))
                !=0 ) {
                throw std::runtime_error("Can't connect to server");
            }
            
            std::cout << "✅" << std::endl;
            
            http_request_creator request_creator("HEAD", file.url.path_to_file);
            request_creator.add_header("Host", file.url.server);
            request_creator.add_header("Accept", "*/*");
            request_creator.add_header("Referer", "http://" + file.url.server + "/");
            
            std::string request = request_creator.get_request();
            
            if (write(socket_fd, request.c_str(), request.length()) == -1) {
                throw std::runtime_error("Sending HEAD query to server failed");
            }
            
            shutdown(socket_fd, 1); // closing socket on write
            
            std::string response;
            char buf[BUF_SIZE];
            long count_of_read_bytes;
            while ( (count_of_read_bytes = read(socket_fd, buf, BUF_SIZE)) != 0 ) {
                if (count_of_read_bytes == -1) {
                    throw std::runtime_error("Reading request from server to HEAD query failed");
                }
                response += std::string(buf, count_of_read_bytes);
            }
            
            http_response_parser response_parser(response);
            
            switch (response_parser.code.front()) {
                case '3':
                    std::cout << "Redirection to " + response_parser.headers.at("Location") << std::endl;
                    file.url = URL(response_parser.headers.at("Location"));
                    break;
                    
                case '2':
//                    std::cout << response;
                    return;
                    
                default:
                    throw std::runtime_error("Unexpected server response: " +
                                             response_parser.code + " " +
                                             response_parser.code_description);
            }
        }
    }
};

#endif /* file_downloader_hpp */
