//
//  file_downloader.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "file_downloader.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <atomic>
#include <mutex>
#include <sstream>
#include <future>
#include <thread>
#include <chrono>
#include <fstream>
#include <list>
#include <functional>

#include "file_to_download.hpp"
#include "http_request_creator.hpp"
#include "http_response_parser.hpp"

file_downloader::file_downloader(file_to_download file, int part_count) {
    this -> file = file;
    this -> part_count = part_count;
    
    std::cout << "Trying to download " + file.url.server + file.url.file_path << std::endl;
    
    while (true) { // In case of redirecting, till response is 2XX
        
        // Getting file size and checking ability of partial download
        
        http_request_creator request_creator("HEAD", file.url.resource);
        request_creator.add_header("Host", file.url.server);
        request_creator.add_header("Accept", "*/*");
        request_creator.add_header("Referer", "http://" + file.url.server + "/");
        request_creator.add_header("Range", "bytes=0-");
        request_creator.add_header("Connection", "close");
        
        std::cout << "Connection... ";
        int socket_fd = create_socket_connected_to_server();
        std::cout << "✅" << std::endl;
        
        send_request_to_server(socket_fd, request_creator.get_request());
        
        shutdown(socket_fd, 1); // closing socket on write
        
        std::string response = search_in_socket(socket_fd, "\r\n\r\n");
        
        shutdown(socket_fd, 2);
        close(socket_fd);       // closing socket ignoring body

        http_response_parser response_parser(response);
        std::map<std::string, std::string>::iterator headers_it;
        
        bool chunked = false;
        
        switch (response_parser.code.front()) {
            case '3': // Redirection
                std::cout << "Redirection to " + response_parser.headers.at("Location") << std::endl;
                file.url = URL(response_parser.headers.at("Location"));
                break;
                
            case '2': // Good
                if ( (headers_it = response_parser.headers.find("Content-Type")) != response_parser.headers.end()) {
                    std::cout << "File type: " << headers_it->second << std::endl;
                }
                
                if ( (headers_it = response_parser.headers.find("Transfer-Encoding")) != response_parser.headers.end() && headers_it->second == "chunked") {
                    chunked = true;
                }
                
                std::cout << "File size: ";
                if ( (headers_it = response_parser.headers.find("Content-Length")) != response_parser.headers.end()) {
                    file_size = std::stoll(headers_it->second);
                    std::cout << file_size << std::endl;
                } else {
                    std::cout << "unknown" << std::endl;
                }
                
                try {
                    if (response_parser.code == "200" || chunked) {
                        if (part_count > 1) {
                            std::cout << "⚠️  Server don't support partial downloads. Using one thread" << std::endl;
                        }
                        if (file_size >= 0) {
                            download_part(0, file_size-1, ATTEMPT_COUNT, false, chunked);
                        } else {
                            download_part(0, -1, ATTEMPT_COUNT, false, chunked);
                        }
                    } else if (response_parser.code == "206") {  // Partial downloads are supported
                        if (file_size >= 0) {  // We can use partial downloads
                            std::list<std::future<void>> parts_downloaders;
                            
                            for (int i = 1; i < part_count; ++i) {
                                long long first_byte = i * bytes_per_thread();
                                long long last_byte = std::min((i+1) * bytes_per_thread(), file_size) - 1;
                                
                                parts_downloaders.push_back(std::async(std::launch::async, std::bind(&file_downloader::download_part, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5), first_byte, last_byte, ATTEMPT_COUNT, true, chunked));
                            }
                                                        
                            download_part(0, std::min(bytes_per_thread(), file_size) - 1, ATTEMPT_COUNT, true, chunked);
                            
                            for (auto & future: parts_downloaders) {
                                future.get();
                            }
                        } else {  // Can't use partial downloads because file size is unknown
                            download_part(0, -1, ATTEMPT_COUNT, true, chunked);
                        }
                    } else {  // !200 && !206
                        throw std::runtime_error("Unexpected server response: " +
                                                 response_parser.code + " " +
                                                 response_parser.code_description);
                    }
                    
                    std::cout << std::endl << "✅  File successfully downloaded" << std::endl;
                } catch (...) {
                    if (already_downloaded > 0) {
                        std::cout << std::endl;
                    }
                    throw;
                }
                
                return;
                
            default:
                throw std::runtime_error("Unexpected server response: " +
                                         response_parser.code + " " +
                                         response_parser.code_description);
        }
    }
}

int file_downloader::create_socket_connected_to_server(void) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socket_fd == -1) {
        throw std::runtime_error("Socket creation failed");
    }
    
    if ( connect(socket_fd,
                 (struct sockaddr *) &file.url.server_addr,
                 sizeof(file.url.server_addr))
        !=0 ) {
        throw std::runtime_error("Can't connect to server");
    }
    
    return socket_fd;
}

void file_downloader::send_request_to_server(int socket_fd, const std::string & request) {
    if (write(socket_fd, request.c_str(), request.length()) == -1) {
        throw std::runtime_error("Sending HEAD query to server failed");
    }
}

void file_downloader::update_downloaded_info(long long bytes) {
    long long downloaded = already_downloaded.fetch_add(bytes) + bytes;
    std::stringstream msg_ss;
    if (file_size > 0) {
        double downloaded_persent = (double) downloaded / file_size;
        int statusBar_length = 50;
        for (int i = 0; i < statusBar_length * downloaded_persent; ++i) {
            msg_ss << '#';
        }
        for (int i = statusBar_length * downloaded_persent; i < statusBar_length; ++i) {
            msg_ss << '.';
        }
        msg_ss << " " << int(downloaded_persent * 100) << "% ( " << downloaded << " / " << file_size << " )";
    } else {   // Can't show status bar because don't know file length
        msg_ss << "Downloaded " << downloaded << " bytes";
    }
    cout_mutex.lock();  // Threads write to terminal one by one
    std::cout << '\r' << msg_ss.str() << std::flush;  // '\r' to overwrite preveous status bar state
    cout_mutex.unlock();
}

long long file_downloader::socket_to_stream(int socket_fd, std::ostream & os, long long limit, bool chunked, std::function<void(long long)>callback_on_flush) { // callback_on_flush = nullptr by default, uses to update status bar
    if (limit == 0) {
        return 0;
    }
    long long read_totaly = 0;
    long long count_of_read_bytes;
    if (chunked) {  // Chunked transfer encoding
        do {
            std::stringstream hex_to_int;
            hex_to_int << std::hex << search_in_socket(socket_fd, "\r\n");
            hex_to_int >> count_of_read_bytes;
            read_totaly += socket_to_stream(socket_fd, os, count_of_read_bytes, false, callback_on_flush);
            if (search_in_socket(socket_fd, "\r\n") != "\r\n") {
                throw std::runtime_error("Bad chunk");
            }
        } while (count_of_read_bytes > 0);
    } else {
        char buf[BUF_SIZE];
        while ( (count_of_read_bytes = read(socket_fd, buf, limit>=0?std::min((long long)(BUF_SIZE), limit-read_totaly):BUF_SIZE)) != 0 ) {
            if (count_of_read_bytes == -1) {
                throw std::runtime_error("Reading request from socket failed");
            }
            read_totaly += count_of_read_bytes;
            os.write(buf, count_of_read_bytes);
            if (callback_on_flush != nullptr) {
                callback_on_flush(count_of_read_bytes);
            }
            if (limit > -1 && read_totaly >= limit) {
                break;
            }
        }
    }
    return read_totaly;
}

std::string file_downloader::search_in_socket(int socket_fd, std::string sequence) {
    std::stringstream ss;
    socket_to_stream(socket_fd, ss, sequence.size());
    std::string buf = ss.str();
    std::string res = buf;
    
    char current;  // This method uses for small data, so we can read it one charachter by another without losing performance
    
    size_t count_of_read_bytes;
    
    while ( buf != sequence && (count_of_read_bytes = read(socket_fd, &current, 1)) != 0 ) {
        if (count_of_read_bytes == -1) {
            throw std::runtime_error("Reading request from socket failed");
        }
        res.push_back(current);
        buf.push_back(current);
        buf = buf.substr(1); // buf.pop_front();
    }
    
    return res;
}

void file_downloader::download_part(long long first_byte, long long last_byte, int attempt_count, bool partial_downloading, bool chunked) {
    
    if (file_size >=0 && first_byte >= file_size) {
        return;
    }
    
    http_request_creator request_creator("GET", file.url.resource);
    request_creator.add_header("Host", file.url.server);
    request_creator.add_header("Accept", "*/*");
    request_creator.add_header("Referer", "http://" + file.url.server + "/");
    request_creator.add_header("Connection", "close");
    
    std::fstream f(file.path, std::ios_base::out | std::ios_base::app | std::ios_base::binary); // Create file if it is not exists
    f.close();
    f.open(file.path, std::ios_base::out | std::ios_base::in | std::ios_base::binary); // The only possible mode; failes if file is not exists
    f.seekp(first_byte);
    
    if (f.fail()) {
        throw std::runtime_error("File opening failed");
    }
    
    long long to_download = 0;
    if (last_byte >= 0) {
        to_download = last_byte - first_byte + 1;
    }
    
    if (partial_downloading) {
        std::stringstream range;
        range << "bytes=" << first_byte << "-";
        
        if (last_byte >= 0) {
            to_download = last_byte - first_byte + 1;
            range << last_byte;
        }
        
        request_creator.add_header("Range", range.str());
    }
    
    int socket_fd = create_socket_connected_to_server();
    
    send_request_to_server(socket_fd, request_creator.get_request());
    
    shutdown(socket_fd, 1);    // Closing socket on write
    
    http_response_parser response(search_in_socket(socket_fd, "\r\n\r\n"));
    if (response.headers["Transfer-Encoding"] == "chunked") {
        chunked = true;
        if (partial_downloading && first_byte > 0) {
            return;
        }
    }
    
    long long got = socket_to_stream(socket_fd, f, -1, chunked,
                                     std::bind(&file_downloader::update_downloaded_info, this, std::placeholders::_1));
    
    shutdown(socket_fd, 2);
    close(socket_fd);          // Closing socket
    
    f.close();                 // Closing file
    
    if (to_download && got < to_download) {  // Server closed connection before sent all data
        if (got > 0) {  // This attempt wasn't completely unsuccessful
            attempt_count = ATTEMPT_COUNT;  // Reseting counter of unsuccessful attempts
        }
        if (attempt_count == 1 || !partial_downloading || chunked) { // No more attempts or can't request needed bytes
            throw std::runtime_error("Server closed connection before sent all data");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200*(ATTEMPT_COUNT - attempt_count)));   // Waiting for Internet connection resumption
        download_part(first_byte + got, last_byte, attempt_count - 1, partial_downloading, chunked);        // Making one more attempt to download lacking data
    }
}

long long file_downloader::bytes_per_thread(void) {
    if (file_size < 0) {
        throw std::logic_error("Using multithread downloading while file-size is unknown");
    }
    return (file_size - 1) / part_count + 1;
}
