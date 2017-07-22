cmake_minimum_required(VERSION 3.5)
project(HTTP_file_downloader)

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_FLAGS -pthread)

set(SRC_DIR
	src
)

set(INCLUDE_DIR
	include
)

include_directories(${INCLUDE_DIR})

set(SOURCES
	${SRC_DIR}/command_line_args.cpp
	${SRC_DIR}/file_downloader.cpp
	${SRC_DIR}/file_to_download.cpp
	${SRC_DIR}/http_request_creator.cpp
	${SRC_DIR}/http_response_parser.cpp
	${SRC_DIR}/list_to_download.cpp
	${SRC_DIR}/url.cpp
)

set(HEADERS
	${INCLUDE_DIR}/command_line_args.hpp
	${INCLUDE_DIR}/file_downloader.hpp
	${INCLUDE_DIR}/file_to_download.hpp
	${INCLUDE_DIR}/http_request_creator.hpp
	${INCLUDE_DIR}/http_response_parser.hpp
	${INCLUDE_DIR}/list_to_download.hpp
	${INCLUDE_DIR}/url.hpp
)

set(MAIN_SOURCES
	${SRC_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES} ${SOURCES} ${HEADERS})
