//
//  file_downloader.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef file_downloader_hpp
#define file_downloader_hpp

#include <iostream>
#include <string>
#include <mutex>
#include <atomic>
#include <functional>

#include "file_to_download.hpp"

#define BUF_SIZE (1 << 15)
#define ATTEMPT_COUNT 20

class file_downloader {
    file_to_download file;
    int part_count;
    long long file_size = -1;
    std::atomic<long long> already_downloaded{0};
    std::mutex cout_mutex;
    
public:
    file_downloader(file_to_download file, int part_count);
    
private:
    int create_socket_connected_to_server(void);
    void send_request_to_server(int socket_fd, const std::string & request);
    void update_downloaded_info(long long bytes);
    long long socket_to_stream(int socket_fd, std::ostream & os, long long limit = -1, bool chunked = false, std::function<void(long long)>callback_on_flush = nullptr);
    std::string search_in_socket(int socket_fd, std::string sequence);
    void download_part(long long first_byte, long long last_byte, int attempt_count, bool partial_downloading, bool chunked);
    long long bytes_per_thread(void);
};

#endif /* file_downloader_hpp */
