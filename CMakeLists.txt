cmake_minimum_required(VERSION 3.7)
project (chronicle CXX)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("$PROJECT_SOURCE_DIR"
lame
)

#add_subdirectory(rtaudio)
add_subdirectory(spdlog)

if (UNIX)
    find_library(LAME_LIB libmp3lame.a)
    if(LAME_LIB OR lame_FOUND)
            message(STATUS "Found libmp3lame")
    else()
            message(FATAL_ERROR "Could not find libmp3lame! Exiting...")
    endif()
endif()

find_library(SNDFILE_LIB sndfile)
if (SNDFILE_LIB OR sndfile_FOUND)
    message(STATUS "Found libsndfile")
else()
    message(FATAL_ERROR "Could not find libsndfile! Exiting...")
endif()

#find_package(PkgConfig REQUIRED)
find_package(RtAudio REQUIRED)
#pkg_check_modules(RtAudio REQUIRED IMPORTED_TARGET rtaudio)
if (RtAudio_FOUND)
    message(STATUS "Found RtAudio: ${RtAudio_VERSION}")
else()
    message(FATAL_ERROR "Could not find RtAudio! Exiting...")
    endif()

find_package(Boost REQUIRED filesystem)
if(Boost_FILESYSTEM_FOUND)
    message(STATUS "Found Boost filesystem...")
    if (Boost_FILESYSTEM_LIBRARY)
        message(STATUS "  Found Boost filesystem libraries")
    else()
        message(FATAL_ERROR "  Could not found Boost filesystem library! Exiting...")
    endif()
else()
    message(FATAL_ERROR "Could not find the Boost filesystem component! Exiting...")
endif()


add_executable(chronicle chronicle.cpp
parse_opts.cpp
screen.cpp
)

spdlog_enable_warnings(chronicle)

target_link_libraries(chronicle PRIVATE
spdlog::spdlog_header_only
#PkgConfig::RtAudio
RtAudio::rtaudio
${SNDFILE_LIB}
#${LAME_LIB}
${Boost_LIBRARIES}
ncurses)
