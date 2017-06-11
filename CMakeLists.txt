cmake_minimum_required(VERSION 3.5)
project(HTTP_file_downloader)

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_FLAGS -pthread)

include_directories(include)

set(SRC_DIR
	src
)

set(SOURCES
	${SRC_DIR}/command_line_args.cpp
	${SRC_DIR}/file_downloader.cpp
	${SRC_DIR}/file_to_download.cpp
	${SRC_DIR}/http_request_creator.cpp
	${SRC_DIR}/http_response_parser.cpp
	${SRC_DIR}/list_to_download.cpp
	${SRC_DIR}/url.cpp
)

set(MAIN_SOURCES
	${SRC_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES} ${SOURCES} ${HEADERS})
