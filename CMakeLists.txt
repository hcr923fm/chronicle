cmake_minimum_required(VERSION 3.7)
project (chronicle CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT ${COMPILER_TRIPLET} OR ${COMPILER_TRIPLET} STREQUAL "")
    message(WARNING "COMPILER_TRIPLET not found; Setting manually to x86_64-w64-mingw32")
    set(COMPILER_TRIPLET "x86_64-w64-mingw32")
endif()

include_directories("$PROJECT_SOURCE_DIR"
lame)

if (WIN32)
    include_directories(/usr/${COMPILER_TRIPLET}/include/boost-1_68
    /usr/${COMPILER_TRIPLET}/include
    )
endif()

add_compile_options(-DSPDLOG_COMPILED_LIB -D_WIN32_WINNT=0x600 -D_ISOC99_SOURCE -lole32 -lwinmm -ldsound -Wl,--trace)
#add_subdirectory(rtaudio)
add_subdirectory(spdlog)

find_library(LAME_LIB libmp3lame.a)
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

if (WIN32)
    target_link_libraries(chronicle PRIVATE
    winmm
    ole32
    dsound)
endif()

target_link_libraries(chronicle PRIVATE
#spdlog::spdlog_header_only
spdlog::spdlog
${LAME_LIB}
${SNDFILE_LIB}
#PkgConfig::RtAudio
#RtAudio::rtaudio
${RTAUDIO_LIB}
${Boost_LIBRARIES}
ncurses)