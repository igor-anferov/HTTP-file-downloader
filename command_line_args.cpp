//
//  command_line_args.cpp
//  HTTP file downloader
//
//  Created by Игорь Анфёров on 08.06.2017.
//  Copyright © 2017 Игорь Анфёров. All rights reserved.
//

#include "command_line_args.hpp"

#include <exception>
#include <sstream>

parse_error::parse_error(): std::logic_error("Command line arguments parsing failed") {}

argument_properties::argument_properties(const std::vector<std::string> & options,
                    const std::string & description) {
    this -> options = options;
    this -> description = description;
}

void command_line_args::add_argument(const std::string & arg_name,
                  const std::vector<std::string> & options,
                  const std::string & description) {
    
    argument_properties arg_prop(options, description);
    avaivable_arguments[arg_name] = arg_prop;
}

void command_line_args::remove_argument(const std::string & arg_name) {
    avaivable_arguments.erase(arg_name);
}

void command_line_args::set_program_options(const std::vector<std::string> & options) {
    avaivable_program_options = options;
}

void command_line_args::parse_args(int argc, char ** argv) {
    int cur_param_num = 0;
    
    // Program name handling
    program_name = argv[cur_param_num++];
    size_t slash_pos;
    if ( (slash_pos = program_name.find_last_of('/')) != std::string::npos) {
        program_name = program_name.substr(slash_pos+1);
    }
    
    // Arguments handling
    for (; cur_param_num < argc;) {
        std::map<std::string, argument_properties>::const_iterator argument_iterator;
        if ((argument_iterator = avaivable_arguments.find(argv[cur_param_num++])) != avaivable_arguments.end()) {
            // Argument
            presented_arguments[argument_iterator -> first];
            // Argument options handling
            for (const std::string & option_name: argument_iterator -> second.options) {
                presented_arguments[argument_iterator -> first][option_name] = argv[cur_param_num++];
            }
        } else {
            // Arguments ended, read program options in other cycle
            break;
        }
    }
    
    // Program options handling
    std::vector<std::string>::const_iterator prorgam_option_iterator = avaivable_program_options.begin();
    for (; cur_param_num < argc && prorgam_option_iterator != avaivable_program_options.end();) {
        presented_program_options[*(prorgam_option_iterator++)] = argv[cur_param_num++];
    }
    
    if (cur_param_num != argc) {
        throw parse_error();
    }
}

bool command_line_args::is_argument_presented(std::string arg) {
    return presented_arguments.find(arg) != presented_arguments.end();
}

bool command_line_args::is_program_option_presented(std::string opt) {
    return presented_program_options.find(opt) != presented_program_options.end();
}

std::map<std::string, std::string> & command_line_args::get_argument_options (std::string arg) {
    return presented_arguments[arg];
}

std::string & command_line_args::get_program_option_value (std::string arg) {
    return presented_program_options[arg];
}

std::string command_line_args::get_help() {
    std::stringstream ss;
    ss << "Usage:" << std::endl;
    ss << "\t" << program_name << " [ { <argument> {<argument option>} } ] [ {program option} ]" << std::endl;
    ss << std::endl;
    ss << "Arguments (you can use them in any order):" << std::endl;
    for (const std::pair<std::string, argument_properties> & curr_argument: avaivable_arguments) {
        ss << "\t" << curr_argument.first;
        for (const std::string & curr_option: curr_argument.second.options) {
            ss << " <" << curr_option << ">";
        }
        ss << std::endl;
        ss << "\t\t" << curr_argument.second.description << std::endl;
    }
    ss << std::endl;
    ss << "Program options (you should use them in order as they presented here):" << std::endl;
    for (const std::string & curr_option: avaivable_program_options) {
        ss << "\t" << curr_option << std::endl;
    }
    return ss.str();
}
