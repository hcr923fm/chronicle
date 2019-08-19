cmake_minimum_required(VERSION 3.7)
project (chronicle)

include_directories("$PROJECT_BINARY_DIR"
lame
spdlog)
add_subdirectory(rtaudio)

add_executable(chronicle chronicle.cpp
parse_opts.cpp
screen.cpp
)

find_package(spdlog REQUIRED)
spdlog_enable_warnings(chronicle)

target_link_libraries(chronicle PRIVATE
spdlog::spdlog_header_only
rtaudio)