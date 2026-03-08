cmake_minimum_required(VERSION 3.11)
project (chronicle
    VERSION 1.1.0
    HOMEPAGE_URL https://github.com/hcr923fm/chronicle
    DESCRIPTION "An hourly broadcast radio logger"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(chronicle 
parse_opts.cpp
screen.cpp
chronicle.cpp
)


##########
# SPDLOG #
##########

#add_compile_options(-DSPDLOG_COMPILED_LIB -D_WIN32_WINNT=0x600 -D_ISOC99_SOURCE -lole32 -lwinmm -ldsound -Wl,--trace,--as-needed)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(chronicle PRIVATE spdlog::spdlog)

##############
# LIBMP3LAME #
##############
find_package(libmp3lame CONFIG REQUIRED)
target_link_libraries(chronicle PRIVATE libmp3lame::libmp3lame)

##############
# LIBSNDFILE #
##############

find_package(SndFile CONFIG REQUIRED)
target_link_libraries(chronicle PRIVATE SndFile::sndfile)

####################
# ncurses/pdcurses #
####################

find_package(pdcurses CONFIG REQUIRED)
if (pdcurses_INCLUDE_DIR)
    message(STATUS "Found pdcurses: ${pdcurses_INCLUDE_DIR}")
    target_include_directories(chronicle PRIVATE ${pdcurses_INCLUDE_DIR})
    target_link_libraries(chronicle PRIVATE ${pdcurses_LIBRARIES})
else()
    message(FATAL_ERROR "Could not find pdcurses! Exiting...")
endif()
target_link_libraries(chronicle PRIVATE pdcurses::pdcurses)


###########
# RTAUDIO #
###########
#target_link_libraries(chronicle PRIVATE rtaudio)
if(WIN32)
    add_definitions(-DAUDIO_WINDOWS_WASAPI=ON)
    add_definitions(-DAUDIO_WINDOWS_ASIO=ON)
elseif(UNIX)
    add_definitions(-DAUDIO_LINUX_ALSA=ON)
    add_definitions(-DAUDIO_LINUX_PULSE=ON)
endif()

add_subdirectory(rtaudio)
target_link_libraries(chronicle PRIVATE rtaudio)
target_include_directories(chronicle PRIVATE ${RtAudio_SOURCE_DIR})
add_custom_command(TARGET chronicle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:rtaudio>" "$<TARGET_FILE_DIR:chronicle>"
    VERBATIM
)

#########
# BOOST #
#########
find_package(Boost REQUIRED filesystem program_options)
target_link_libraries(chronicle PRIVATE Boost::program_options Boost::filesystem)
target_include_directories(chronicle PRIVATE ${Boost_INCLUDE_DIR})


############################
# INSTALLATION / PACKAGING #
############################

install(TARGETS chronicle)
install(FILES
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/LICENCE.md
    ${CMAKE_SOURCE_DIR}/CHANGELOG.md
    DESTINATION .
)

if(WIN32)
    install(CODE [[
        #message(STATUS "Finding deps for: $<TARGET_FILE:chronicle>")
        file(GET_RUNTIME_DEPENDENCIES
            RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
            UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
            EXECUTABLES "${CMAKE_BINARY_DIR}/Release/chronicle.exe"
            DIRECTORIES C:/lib
            POST_EXCLUDE_REGEXES ".*system32.*"
        )

        #message(STATUS "Got resolved deps: ${RESOLVED_DEPS}")
        #message(STATUS "Got unresolved deps: ${UNRESOLVED_DEPS}")
        foreach(_file ${RESOLVED_DEPS})
            file(INSTALL "${_file}"
                DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
                TYPE STATIC_LIBRARY 
                FOLLOW_SYMLINK_CHAIN
            )
        endforeach()
    ]])
endif()

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An hourly broadcast radio logger")
set(CPACK_PACKAGE_DESCRIPTION "Chronicle is an audio logger, designed for radio station compliance use.")
set(CPACK_PACKAGE_VENDOR "Cal McLean")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENCE.md")

if(UNIX AND NOT WIN32)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cal McLean <calmcl1@aol.com>") # required
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "librtaudio6 (>=5.2), libsndfile1, libboost-program-options1.71.0, libboost-filesystem1.71.0, libncurses6 (>=6.0), libmp3lame-dev (>= 1.3.0)")
    set(CPACK_DEBIAN_PACKAGE_SECTION "Sound")
    set(CPACK_DEBIAN_PACKAGE_VERSION "1.1.0")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

    set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
    if($ENV:{CIRRUS_BRANCH} STREQUAL "master")
        set(CPACK_SOURCE_PACKAGE_FILE_NAME "chronicle-${CPACK_DEBIAN_PACKAGE_VERSION}")
    else()
        string(TIMESTAMP DATETIME "%Y%m%d%H%M%S")
        set(CPACK_SOURCE_PACKAGE_FILE_NAME "chronicle-${CPACK_DEBIAN_PACKAGE_VERSION}+SNAPSHOT${DATETIME}")
    endif() 
    set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "CPackSourceConfig.cmake")
    set(CPACK_SOURCE_IGNORE_FILES "\.ci;.git.*;\.vscode;.*cirrus.*;.*build.*")

else()
    set(CPACK_GENERATOR "ZIP;TGZ")
endif()

set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
