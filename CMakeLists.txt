cmake_minimum_required(VERSION 3.7)
project (chronicle CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("$PROJECT_SOURCE_DIR"
lame
)

add_executable(chronicle 
parse_opts.cpp
screen.cpp
chronicle.cpp
)

target_link_directories(chronicle PRIVATE C:/lib/)

IF (UNIX AND NOT WIN32)
    include_directories(/usr/include/rtaudio)
endif()

if (WIN32)
    include_directories(/usr/$ENV{COMPILER_TRIPLET}/include/boost-1_68
    /usr/$ENV{COMPILER_TRIPLET}/include
    )
endif()

##########
# SPDLOG #
##########

add_compile_options(-DSPDLOG_COMPILED_LIB -D_WIN32_WINNT=0x600 -D_ISOC99_SOURCE -lole32 -lwinmm -ldsound -Wl,--trace,--as-needed)
find_package(spdlog REQUIRED)

##############
# LIBMP3LAME #
##############

find_library(LAME_LIB
NAMES libmp3lame libmp3lame.a libmp3lame-static.lib libmp3lame.dll libmp3lame.lib lame_enc lame_enc.dll lame_enc.lib
HINTS "C:/lib/") # For the Windows build - lib files are placed in the SOURCE/lib dir

if (LAME_LIB OR lame_FOUND)
    message(STATUS "Found LAME: ${LAME_LIB}")
else()
    message(FATAL_ERROR "Could not find lame! Exiting...")
endif()

##############
# LIBSNDFILE #
##############

if (UNIX)
    find_library(SNDFILE_LIB sndfile)
    if (SNDFILE_LIB OR sndfile_FOUND)
        message(STATUS "Found libsndfile")
    else()
        message(FATAL_ERROR "Could not find libsndfile! Exiting...")
    endif()
endif()
if (WIN32)
    find_package(LibSndFile CONFIG REQUIRED)
endif()

###########
# RTAUDIO #
###########

find_library(RTAUDIO_LIB NAMES rtaudio rtaudio.lib rtaudio.dll
HINTS "C:/lib/)
if (RTAUDIO_LIB OR rtaudio_FOUND)
    message(STATUS "Found RtAudio: ${RTAUDIO_LIB}")
else()
    message(FATAL_ERROR "Could not find RtAudio! Exiting...")
endif()

#########
# BOOST #
#########

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

#############
# CHRONICLE #
#############

if (WIN32)
    target_link_libraries(chronicle PRIVATE sndfile-shared)
endif()

if(UNIX)
    target_link_libraries(chronicle PRIVATE ${SNDFILE_LIB})
endif()

target_link_libraries(chronicle PRIVATE
spdlog::spdlog
${LAME_LIB}
${RTAUDIO_LIB}
${Boost_LIBRARIES}
ncurses)