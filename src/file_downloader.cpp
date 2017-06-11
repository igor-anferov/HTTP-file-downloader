//
//  file_downloader.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 09.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "file_downloader.hpp"

file_downloader::file_downloader(file_to_download file, int part_count) {
    this -> file = file;
    this -> part_count = part_count;
    
    std::cout << "Trying to download " + file.url.server + file.url.get_path_to_file_without_query() << std::endl;
    
    while (true) { // In case of redirecting, till response is 2XX
        
        // Getting file size and checking acceptance of ranges
        
        
        http_request_creator request_creator("HEAD", file.url.path_to_file);
        request_creator.add_header("Host", file.url.server);
        request_creator.add_header("Accept", "*/*");
        request_creator.add_header("Referer", "http://" + file.url.server + "/");
        request_creator.add_header("Range", "bytes=0-");
        
        std::cout << "Connection... ";
        
        int socket_fd = create_socket_connected_to_server();
        
        std::cout << "✅" << std::endl;
        
        send_request_to_server(socket_fd, request_creator.get_request());
        
        shutdown(socket_fd, 1); // closing socket on write
        
        std::stringstream response;
        socket_to_stream(socket_fd, response);
        
        shutdown(socket_fd, 2);
        close(socket_fd);       // closing socket
        
        http_response_parser response_parser(response.str());
        std::map<std::string, std::string>::iterator headers_it;
        
        switch (response_parser.code.front()) {
            case '3':
                std::cout << "Redirection to " + response_parser.headers.at("Location") << std::endl;
                file.url = URL(response_parser.headers.at("Location"));
                break;
                
            case '2':
                if ( (headers_it = response_parser.headers.find("Content-Type")) != response_parser.headers.end()) {
                    std::cout << "File type: " << (*headers_it).second << std::endl;
                }
                
                std::cout << "File size: ";
                if ( (headers_it = response_parser.headers.find("Content-Length")) != response_parser.headers.end()) {
                    file_size = std::stoll((*headers_it).second);
                    std::cout << file_size << std::endl;
                } else {
                    std::cout << "unknown" << std::endl;
                }
                
                try {
                    if (response_parser.code == "200") {
                        if (part_count > 1) {
                            std::cout << "⚠️  Server don't support partitional downloads. Using one thread" << std::endl;
                        }
                        if (file_size >= 0) {
                            download_part(0, file_size-1, ATTEMPT_COUNT, false);
                        } else {
                            download_part(0, -1, ATTEMPT_COUNT, false);
                        }
                    } else if (response_parser.code == "206") {
                        if (file_size >= 0) {
                            std::list<std::future<void>> parts_downloaders;
                            
                            for (int i = 1; i < part_count; ++i) {
                                long long first_byte = i * bytes_per_thread();
                                long long last_byte = std::min((i+1) * bytes_per_thread(), file_size) - 1;
                                
                                parts_downloaders.push_back(std::async(std::launch::async, &file_downloader::download_part, this, first_byte, last_byte, ATTEMPT_COUNT, true));
                            }
                            
                            std::cout << 0 << ": " << 0 << "-" << std::min(bytes_per_thread(), file_size) - 1 << std::endl;
                            
                            download_part(0, std::min(bytes_per_thread(), file_size) - 1, ATTEMPT_COUNT, true);
                            
                            for (auto & future: parts_downloaders) {
                                future.get();
                            }
                        } else {
                            download_part(0, -1, ATTEMPT_COUNT, true);
                        }
                    } else {
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

int file_downloader::create_socket_connected_to_server() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socket_fd == -1) {
        throw std::runtime_error("Socket creation fail");
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
    } else {
        msg_ss << "Downloaded " << downloaded << " bytes";
    }
    cout_mutex.lock();
    std::cout << '\r' << msg_ss.str() << std::flush;
    cout_mutex.unlock();
}

long long file_downloader::socket_to_stream(int socket_fd, std::ostream & os, std::function<void(long long)>callback_on_flush) {
    char buf[BUF_SIZE];
    long long read_totaly = 0;
    long long count_of_read_bytes;
    while ( (count_of_read_bytes = read(socket_fd, buf, BUF_SIZE)) != 0 ) {
        if (count_of_read_bytes == -1) {
            throw std::runtime_error("Reading request from socket failed");
        }
        read_totaly += count_of_read_bytes;
        os.flush();
        os.write(buf, count_of_read_bytes);
        os.flush();
        if (callback_on_flush != nullptr) {
            callback_on_flush(count_of_read_bytes);
        }
    }
    return read_totaly;
}

std::string file_downloader::get_header(int socket_fd) {
    std::string buf = "NOT";
    std::string header;
    
    char current;
    
    size_t count_of_read_bytes;
    
    while ( (count_of_read_bytes = read(socket_fd, &current, 1)) != 0 ) {
        if (count_of_read_bytes == -1) {
            throw std::runtime_error("Reading request from socket failed");
        }
        header.push_back(current);
        buf.push_back(current);
        if (buf == "\r\n\r\n") {
            break;
        }
        buf = buf.substr(1);
    }
    
    return header;
}

void file_downloader::download_part(long long first_byte, long long last_byte, int attempt_count, bool partitional_downloading) {
    
    if (file_size >=0 && first_byte >= file_size) {
        return;
    }
    
    http_request_creator request_creator("GET", file.url.path_to_file);
    request_creator.add_header("Host", file.url.server);
    request_creator.add_header("Accept", "*/*");
    request_creator.add_header("Referer", "http://" + file.url.server + "/");
    request_creator.add_header("Connection", "close");
    
    std::fstream f(file.path, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    f.close();
    f.open(file.path, std::ios_base::out | std::ios_base::in | std::ios_base::binary);
    f.seekp(first_byte);
    
    if (f.fail()) {
        throw std::runtime_error("File opening failed");
    }
    
    long long to_download = 0;
    
    if (partitional_downloading) {
        
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
    
    shutdown(socket_fd, 1);    // closing socket on write
    
    get_header(socket_fd);     // Move to body
    
    long long got = socket_to_stream(socket_fd, f,
                                     std::bind(&file_downloader::update_downloaded_info, this, std::placeholders::_1));
    
    shutdown(socket_fd, 2);
    close(socket_fd);          // closing socket
    
    f.close();
    
    if (to_download && got < to_download) {
        if (got > 0) {
            attempt_count = ATTEMPT_COUNT;
        }
        if (attempt_count == 1 || !partitional_downloading) {
            throw std::runtime_error("Server closed connection before sent all data");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200*(ATTEMPT_COUNT - attempt_count)));   // wait for Internet connection resumption
        download_part(first_byte + got, last_byte, attempt_count - 1, partitional_downloading);
    }
}

long long file_downloader::bytes_per_thread(void) {
    if (file_size < 0) {
        throw std::logic_error("Using multithread downloading while file-size is unknown");
    }
    return (file_size - 1) / part_count + 1;
}
