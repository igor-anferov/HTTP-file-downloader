//
//  main.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 08.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include <iostream>
#include <exception>
#include "command_line_args.hpp"
#include "list_to_download.hpp"

int main(int argc, char ** argv) {
    command_line_args args;
    
    try {
        args.add_argument("-help",
                          {},
                          "Prints this user guide. If exists, all other arguments will be ignored");
        args.add_argument("-i",
                          {"file"},
                          "Use file with URLs to download and mabye paths specifies where to dowload the file. File format:\r\n\t\t{<URL> [<path>]\\n}\r\n\t\tIf exists, -o argument and URL will be ignored");
        args.add_argument("-o",
                          {"path (with filename)"},
                          "Specifies where to dowload the file");
        args.add_argument("-p",
                          {"part_count"},
                          "Specifies number of simultaneously downloading parts. Will be ignored if server not supported parallel downloads");
        args.set_program_options({"URL"});
        
        args.parse_args(argc, argv);
        
        list_to_download downloading_files;
        if (args.is_argument_presented("-i")) {
            downloading_files = list_to_download(args.get_argument_options("-i")["file"]);
        } else {
            if (!args.is_program_option_presented("URL")) {
                throw std::logic_error("You should specify eihter URL program option or \"-i\" attribute");
            }
            std::string url = args.get_program_option_value("URL");
            std::string path = "";
            if (args.is_argument_presented("-o")) {
                path = args.get_argument_options("-o")["path (with filename)"];
            }
            
            downloading_files = list_to_download(url, path);
        }
        
        for (const file_to_download & f: downloading_files) {
            std::cout << f.url.server << " " << f.url.path_to_file << " " << f.url.get_filename() << " " << f.path << std::endl;
        }
        
    } catch (parse_error e) {
        std::cerr << "ERROR!!! " << e.what() << std::endl;
        std::cerr << args.get_help();
        return -1;
    } catch (std::exception e) {
        std::cerr << "ERROR!!! " << e.what() << std::endl;
        return -2;
    }
    
    return 0;
}
