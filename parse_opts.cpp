#include "parse_opts.h"
#include <cerrno>
#include <cstdio>
#include <chrono>
#include <iostream>

namespace po = boost::program_options;

po::variables_map parse_cmd_opts(int argc, char *argv[])
{
    po::options_description options_info("Informational Options");
    options_info.add_options()
        /* Informational switches - print info then quit */
        ("help,h", "Prints this help message")                                                     //
        ("licence", "Prints the licence information for this software and libraries that it uses") //
        ("list-devices,l", "Lists the available input devices with their IDs")                     //
        ("version", "Prints the version number of Chronicle");

    po::options_description options_filesystem("Filesystem Options");
    options_filesystem.add_options()
        /* Filesystem switches - to do with the directory name or filename */
        ("directory,d", po::value<string>()->default_value(boost::filesystem::current_path().string()), "Sets the directory to save the logged audio to. A trailing slash is not required, but may be added. On Windows, if using a trailing slash, use a trailing double-slash.\n"
                                                                                                        "Defaults to current directory") //
        ("format,f", po::value<string>()->default_value("\%F \%H\%M\%S"), "strftime-compatible format to use when naming the audio files.\n"
                                                                          "Defaults to %F %H%M%S") //
        ("no-delete", "If passed, Chronicle will not delete old audio files, so they can be manually managed.\n"
                      "Incompatible with --max-age") //
        ("max-age,a", po::value<string>()->default_value("1000h"), "Sets the maximum age before audio files will be automatically deleted.\n"
                                                                   "Use the format <length><unit>, where unit is < s| m | h | d > for seconds, minutes, hours and days, respectively.\n"
                                                                   "Defaults to 1000 hours (OfCom minimum required storage for radio stations");

    po::options_description options_audio("Audio Options");
    options_audio.add_options()
        /* Audio switches - to do with the input device or audio format */
        ("audio-format,s", po::value<string>()->default_value("WAV"), "Sets the audio format to use for the recorded audio files. Acceptable parameters are:\n"
                                                                      "  OGG \tOgg Vorbis (.ogg)\n"
                                                                      "WAV \t16-bit PCM WAV (.wav) (default)\n"
                                                                      "MP3 \t 320kbps MP3 (.mp3)\n"
                                                                      "FLAC \t Free Lossless Audio Codec (.flac)")                                                                          //
        ("input-device,i", po::value<unsigned int>(), "The ID number of the input device to record from. A list of input devices and their ID numbers can be obtained with `chronicle -l`") //
        ("device-first-channel,t", po::value<unsigned int>()->default_value(0), "On multi-channel audio devices, select the first input channel to record from.\n"
                                                                                "Defaults to 0") //
        ("device-channels,c", po::value<unsigned int>()->default_value(2), "On multi-channel audio devices, select how many channels to record.\n"
                                                                           "Use with --device-first-channel to use effectively on devices with more audio channels available than you wish to record from") //
        ("sample-rate,r", po::value<unsigned int>()->default_value(44100), "The sample rate (in Hz) that you wish to use with the audio device specified, if supported");

    po::options_description options_extra("Extra Options");
    options_extra.add_options()
        /* Extra/maintenance switches */
        ("no-term", "If supplied, will not initialize a UI. Use when running chronicle in the background") //
        ("debug", "Enables verbose logging");

    po::variables_map vm;
    po::options_description options_all("All Options");
    options_all.add(options_info).add(options_audio).add(options_filesystem).add(options_extra);
    po::store(po::parse_command_line(argc, argv, options_all), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        cout << options_info << endl
             << options_audio << endl
             << options_filesystem << endl
             << options_extra << endl;
        exit(0);
    }

    if (vm.count("licence"))
    {
        printLicence();
        exit(0);
    }

    if (vm.count("version"))
    {
        printVersion();
        exit(0);
    }

    return vm;
}

bool is_integer(char *value)
{
    char *endptr = NULL;
    unsigned int n = strtoul(value, &endptr, 10);

    return endptr != value;
}

void printVersion()
{
    printf("%s.%s.%s", SOFTWARE_VERSION_MAJOR, SOFTWARE_VERSION_MINOR, SOFTWARE_VERSION_PATCH);
}

void printLicence()
{
    const char LICENCE[] =
        R"(
Chronicle is distributed under the MIT Licence.
See LICENCE for details of the licences used.
Chronicle uses the following libraries internally:

    libsndfile
        Copyright (C) 1999-2016 Erik de Castro Lopo <erikd@mega-nerd.com>
        Licenced under the LGPL as a dynamically linked library.
        The version of libsndfile that is distributed with this software has not
        been modified from the version available at
        http://www.mega-nerd.com/libsndfile/
        
    RtAudio
        Copyright (c) Gary P. Scavone, McGill University
        Licenced under the the RtAudio licence.
        https://www.music.mcgill.ca/~gary/rtaudio/

	Boost
		Licenced under the Boost Software Licence.

	LAME
		Licenced under the LGPL v2.1. Courtesy of www.mp3dev.org
)";

    cout << LICENCE << endl;
}