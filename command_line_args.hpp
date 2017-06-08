//
//  command_line_args.hpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 08.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#ifndef command_line_args_hpp
#define command_line_args_hpp

#include <string>
#include <vector>
#include <map>

class parse_error: public std::logic_error {
public:
    parse_error();
};

struct argument_properties {
    std::vector<std::string> options;
    std::string description;
    
    argument_properties() = default;
    
    argument_properties(const std::vector<std::string> & options,
                        const std::string & description);
};

class command_line_args {
    std::map<std::string, argument_properties> avaivable_arguments;
    std::vector<std::string> avaivable_program_options;
    
    std::string program_name;
    std::map<std::string, std::map<std::string, std::string>> presented_arguments;
    std::map<std::string, std::string> presented_program_options;
    
public:
    void add_argument(const std::string & arg_name,
                      const std::vector<std::string> & options,
                      const std::string & description);
    void remove_argument(const std::string & arg_name);
    void set_program_options(const std::vector<std::string> & options);
    void parse_args(int argc, char ** argv);
    bool is_argument_presented(std::string arg);
    bool is_program_option_presented(std::string arg);
    std::map<std::string, std::string> & get_argument_options (std::string arg);
    std::string & get_program_option_value (std::string arg);
    std::string get_help();
};

#endif /* command_line_args_hpp */
