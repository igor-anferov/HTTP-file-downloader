cmake_minimum_required(VERSION 3.5)
project(HTTP file downloader)

set(CMAKE_CXX_STANDARD 11)

set(HEADERS
	command_line_args.hpp
	file_downloader.hpp
	file_to_download.hpp
	http_request_creator.hpp
	http_response_parser.hpp
	list_to_download.hpp
	url.hpp
)

set(SOURCES
	command_line_args.cpp
	file_downloader.cpp
	file_to_download.cpp
	http_request_creator.cpp
	http_response_parser.cpp
	list_to_download.cpp
	url.cpp
)

set(MAIN_SOURCE
	main.cpp
)

add_executable(${PROJECT_NAME} ${MAIN_SOURCE} ${SOURCES} ${HEADERS})
