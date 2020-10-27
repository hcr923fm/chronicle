cmake_minimum_required(VERSION 3.11)
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

#target_link_directories(chronicle PRIVATE "C:/lib")

IF (UNIX AND NOT WIN32)
    include_directories(/usr/include/rtaudio)
endif()

if (WIN32)
    include_directories(/usr/$ENV{COMPILER_TRIPLET}/include/boost-1_68
    C:/include
    /usr/$ENV{COMPILER_TRIPLET}/include
    )
endif()

##########
# SPDLOG #
##########

#add_compile_options(-DSPDLOG_COMPILED_LIB -D_WIN32_WINNT=0x600 -D_ISOC99_SOURCE -lole32 -lwinmm -ldsound -Wl,--trace,--as-needed)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(chronicle PRIVATE spdlog::spdlog)

#find_package(fmt CONFIG REQUIRED)

##############
# LIBMP3LAME #
##############

#find_library(LAME_LIB
#NAMES libmp3lame libmp3lame.a libmp3lame-static.lib libmp3lame.dll libmp3lame.lib lame_enc lame_enc.dll lame_enc.lib) # For the Windows build - lib files are placed in the SOURCE/lib dir
if(UNIX)
    find_library(LAME_LIB NAMES libmp3lame libmp3lame.a libmp3lame-static.lib libmp3lame.dll libmp3lame.lib lame_enc lame_enc.dll lame_enc.lib)
    if (LAME_LIB OR lame_FOUND)
        message(STATUS "Found LAME: ${LAME_LIB}")
        target_link_libraries(chronicle PRIVATE ${LAME_LIB})
    else()
        message(FATAL_ERROR "Could not find lame! Exiting...")
    endif()

endif()

if(WIN32)
    find_package(mp3lame CONFIG REQUIRED)
    target_link_libraries(chronicle PRIVATE mp3lame::mp3lame)
endif()

##############
# LIBSNDFILE #
##############

if (UNIX)
    find_library(SNDFILE_LIB sndfile)
    if (SNDFILE_LIB OR sndfile_FOUND)
        message(STATUS "Found libsndfile")
        target_link_libraries(chronicle PRIVATE ${SNDFILE_LIB})
    else()
        message(FATAL_ERROR "Could not find libsndfile! Exiting...")
    endif()
endif()
if (WIN32)
    find_package(SndFile CONFIG REQUIRED)
    target_link_libraries(chronicle PRIVATE SndFile::sndfile)
endif()

####################
# ncurses/pdcurses #
####################

if (UNIX)
    find_library(CURSES_LIB ncurses)
endif()

if (WIN32)
    if(PORT STREQUAL "pdcurses")
        set(VCPKG_LIBRARY_LINKAGE static)
    elseif()
        set(VCPKG_LIBRARY_LINKAGE dynamic)
    endif()
    find_library(CURSES_LIB NAMES pdcurses.dll pdcurses.lib)
endif()

if (CURSES_LIB)
    message(STATUS "Found curses: ${CURSES_LIB}")
    target_link_libraries(chronicle PRIVATE ${CURSES_LIB})
else()
    message(FATAL_ERROR "Could not find curses! Exiting...")
endif()

###########
# RTAUDIO #
###########

find_library(RTAUDIO_LIB NAMES rtaudio
HINTS "C:/lib/")
if (RTAUDIO_LIB OR rtaudio_FOUND)
    message(STATUS "Found RtAudio: ${RTAUDIO_LIB}")
    target_link_libraries(chronicle PRIVATE ${RTAUDIO_LIB})

else()
    message(FATAL_ERROR "Could not find RtAudio! Exiting...")
endif()

#########
# BOOST #
#########

# set(Boost_DEBUG ON)
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

if(Boost_PROGRAM_OPTIONS_FOUND)
    message(STATUS "Found Boost program_options...")
    if (Boost_PROGRAM_OPTIONS_LIBRARY)
        message(STATUS "  Found Boost program_options libraries")
    else()
        message(FATAL_ERROR "  Could not found Boost program_options library! Exiting...")
    endif()
else()
    message(FATAL_ERROR "Could not find the Boost program_options component! Exiting...")
endif()

target_link_libraries(chronicle PRIVATE ${Boost_LIBRARIES})