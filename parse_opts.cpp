#include "parse_opts.h"
#include <cerrno>
#include <cstdio>
#include <chrono>
#include <iostream>

namespace po = boost::program_options;

po::variables_map doAThing(int argc, char *argv[])
{
    po::options_description options_info("Informational Options");
    options_info.add_options()
        /* Informational switches - print info then quit */
        ("help,h", "Prints this help message")                                                     //
        ("licence", "Prints the licence information for this software and libraries that it uses") //
        ("list-devices,l", "Lists the available input devices with their IDs");

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
                                                                      "FLAC \t Free Lossless Audio Codec (.flac)")                                                                 //
        ("input-device,i", po::value<int>(), "The ID number of the input device to record from. A list of input devices and their ID numbers can be obtained with `chronicle -l`") //
        ("device-first-channel,t", po::value<int>()->default_value(0), "On multi-channel audio devices, select the first input channel to record from.\n"
                                                                       "Defaults to 0") //
        ("device-channels,c", po::value<int>()->default_value(2), "On multi-channel audio devices, select how many channels to record.\n"
                                                 "Use with --device-first-channel to use effectively on devices with more audio channels available than you wish to record from") //
        ("sample-rate,r", po::value<int>()->default_value(44100), "The sample rate (in Hz) that you wish to use with the audio device specified, if supported");

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

    return vm;
}

// cmdOpts parse_options(int argc, char *argv[])
// {
//     cmdOpts opts;
//     for (int i = 1; i < argc; i++)
//     {

//         /* Informational switches - print info then quit */
//         /* --licence */
//         if (!strcmp(argv[i], "--licence"))
//         {
//             opts.licence = true;
//         }

//         /* --help */
//         else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
//         {
//             opts.help = true;
//         }

//         /* --list-devices */
//         else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--list-devices"))
//         {
//             opts.list_devices = true;
//         }

//         /* Filesystem switches - to do with the directory name or filename */
//         /* --directory */
//         else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--directory"))
//         {
//             opts.directory = argv[i + 1];
//             i++;
//         }

//         /* --format */
//         else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--format"))
//         {
//             opts.format = argv[i + 1];
//             i++;
//         }

//         /* --no-delete */
//         else if (!strcmp(argv[i], "--no-delete"))
//         {
//             opts.no_delete = true;
//         }

//         /* --max-age */
//         else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--max-age"))
//         {

//             string arg = argv[i + 1];
//             string arg_unit = arg.substr(arg.length() - 1, 1); // s/m/d/h
//             string arg_value = arg.substr(0, arg.length() - 1);

//             char *endptr;
//             unsigned int n = strtoul(arg_value.c_str(), &endptr, 10);

//             if (endptr == arg_value.c_str())
//             { // Conversion didn't happen if endptr == start of string
//                 printf("Value supplied for --max-age is invalid (must be a number)\n");
//                 printf(arg.c_str());
//                 exit(0);
//             }

//             if (arg_unit != "s" && arg_unit != "m" && arg_unit != "h" && arg_unit != "d")
//             {
//                 printf("Unit supplied for --max-age is invalid (must be one of 's', 'm', 'h', or 'd')\n");
//                 printf(arg.c_str());
//             }

//             opts.max_age_value = n;
//             opts.max_age_unit = arg_unit;
//             i++;
//         }

//         /* Audio switches - to do with the input device or audio format */
//         /* --audio-format */
//         else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--audio-format"))
//         {
//             opts.audio_format = argv[i + 1];
//             i++;
//         }

//         /* --input-device */
//         else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input-device"))
//         {
//             char *endptr;
//             unsigned int n = strtoul(argv[i + 1], &endptr, 10);

//             if (endptr != argv[i + 1])
//             { // Conversion didn't happen
//                 opts.input_device = n;
//                 i++;
//             }
//             else
//             {
//                 printf("Argument supplied for --input-device is invalid (must be a number)\n");
//                 printf(argv[i + 1]);
//                 exit(1);
//             }
//         }

//         /* --device-first-channel */
//         else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--device-first-channel"))
//         {
//             if (is_integer(argv[i + 1]))
//             {
//                 opts.device_first_channel = atol(argv[i + 1]);
//                 i++;
//             }
//             else
//             {
//                 printf("Argument supplied for --device-first-channel is invalid (must be a number): %s", argv[i + 1]);
//                 exit(1);
//             }
//         }

//         /* --device-channels */
//         else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--device-channels"))
//         {
//             if (is_integer(argv[i + 1]))
//             {
//                 opts.device_first_channel = atol(argv[i + 1]);
//                 i++;
//             }
//             else
//             {
//                 printf("Argument supplied for --device-channels is invalid (must be a number): %s", argv[i + 1]);
//                 exit(1);
//             }
//         }

//         /* --sample-rate */
//         else if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--sample-rate"))
//         {
//             if (is_integer(argv[i + 1]))
//             {
//                 opts.device_first_channel = atol(argv[i + 1]);
//                 i++;
//             }
//             else
//             {
//                 printf("Argument supplied for --device-channels is invalid (must be a number): %s", argv[i + 1]);
//                 exit(1);
//             }
//         }

//         /* Extra/maintenance switches */
//         /* --no-term */

//         else if (!strcmp(argv[i], "--no-term"))
//         {
//             opts.no_term = true;
//             i++;
//         }

//         /* --debug */
//         else if (!strcmp(argv[i], "--debug"))
//         {
//             opts.is_debug = true;
//             i++;
//         }

//         /* unrecognized arguments */
//         else
//         {
//             printf("Unrecognized argument:\n");
//             printf(argv[i]);
//             exit(1);
//         }
//     }

//     return opts;
// }

bool is_integer(char *value)
{
    char *endptr = NULL;
    unsigned int n = strtoul(value, &endptr, 10);

    return endptr != value;
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