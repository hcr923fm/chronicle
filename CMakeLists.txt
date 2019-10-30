cmake_minimum_required(VERSION 3.7)
project (chronicle CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("$PROJECT_SOURCE_DIR"
lame
)

IF (UNIX AND NOT WIN32)
    include_directories(/usr/include/rtaudio)
endif()

if (WIN32)
    include_directories(/usr/$ENV{COMPILER_TRIPLET}/include/boost-1_68
    /usr/$ENV{COMPILER_TRIPLET}/include
    )
endif()

add_compile_options(-DSPDLOG_COMPILED_LIB -D_WIN32_WINNT=0x600 -D_ISOC99_SOURCE -lole32 -lwinmm -ldsound -Wl,--trace,--as-needed)
#add_subdirectory(rtaudio)
#add_subdirectory(spdlog)
find_package(spdlog REQUIRED)

IF (UNIX AND NOT WIN32)
    find_library(LAME_LIB libmp3lame.a)
endif()

IF (WIN32)
    find_library(LAME_LIB libmp3lame.dll)
endif()
if (LAME_LIB OR lame_FOUND)
    message(STATUS "Found LAME")
else()
    message(FATAL_ERROR "Could not find lame! Exiting...")
endif()

find_library(SNDFILE_LIB sndfile)
if (SNDFILE_LIB OR sndfile_FOUND)
    message(STATUS "Found libsndfile")
else()
    message(FATAL_ERROR "Could not find libsndfile! Exiting...")
endif()

#find_package(PkgConfig REQUIRED)
find_library(RTAUDIO_LIB rtaudio)
#pkg_check_modules(RtAudio REQUIRED IMPORTED_TARGET rtaudio)
if (RTAUDIO_LIB OR rtaudio_FOUND)
    message(STATUS "Found RtAudio: ${RTAUDIO_LIB}")
else()
    message(FATAL_ERROR "Could not find RtAudio! Exiting...")
endif()

set(Boost_DEBUG ON)
find_package(Boost REQUIRED filesystem program_options)
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


add_executable(chronicle 
parse_opts.cpp
screen.cpp
chronicle.cpp
)

target_link_libraries(chronicle PRIVATE
spdlog::spdlog
${LAME_LIB}
${SNDFILE_LIB}
${RTAUDIO_LIB}
${Boost_LIBRARIES}
ncurses)