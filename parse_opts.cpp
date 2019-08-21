#include "parse_opts.h"
#include <cerrno>
#include <cstdio>
#include <chrono>

cmdOpts parse_options(int argc, char *argv[])
{
    cmdOpts opts;
    for (int i = 1; i < argc; i++)
    {
        /* --licence */
        if (!strcmp(argv[i], "--licence"))
        {
            opts.licence = true;
        }

        /* --help */
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            opts.help = true;
        }

        /* --list-devices */
        else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--list-devices"))
        {
            opts.list_devices = true;
        }

        /* --directory */
        else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--directory"))
        {
            opts.directory = argv[i + 1];
            i++;
        }

        /* --format */
        else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--format"))
        {
            opts.format = argv[i + 1];
            i++;
        }

        /* --no-delete */
        else if (!strcmp(argv[i], "--no-delete"))
        {
            opts.no_delete = true;
        }

        /* --max-age */
        else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--max-age"))
        {

            string arg = argv[i + 1];
            string arg_unit = arg.substr(arg.length() - 1, 1); // s/m/d/h
            string arg_value = arg.substr(0, arg.length() - 1);

            char *endptr;
            unsigned int n = strtoul(arg_value.c_str(), &endptr, 10);

            if (endptr == arg_value.c_str())
            { // Conversion didn't happen if endptr == start of string
                printf("Value supplied for --max-age is invalid (must be a number)\n");
                printf(arg.c_str());
                exit(0);
            }

            if (arg_unit != "s" && arg_unit != "m" && arg_unit != "h" && arg_unit != "d")
            {
                printf("Unit supplied for --max-age is invalid (must be one of 's', 'm', 'h', or 'd')\n");
                printf(arg.c_str());
            }

            opts.max_age_value = n;
            opts.max_age_unit = arg_unit;
            i++;
        }

        /* --audio-format */
        else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--audio-format"))
        {
            opts.audio_format = argv[i + 1];
            i++;
        }

        /* --input-device */
        else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input-device"))
        {
            char *endptr;
            unsigned int n = strtoul(argv[i + 1], &endptr, 10);

            if (endptr != argv[i + 1])
            { // Conversion didn't happen
                opts.input_device = n;
                i++;
            }
            else
            {
                printf("Argument supplied for --input-device is invalid (must be a number)\n");
                printf(argv[i + 1]);
                exit(0);
            }
        }

        /* --no-term */

        else if (!strcmp(argv[i], "--no-term"))
        {
            opts.no_term = true;
            i++;
        }

        /* --debug */
        else if (!strcmp(argv[i], "--debug"))
        {
            opts.is_debug = true;
            i++;
        }

        /* unrecognized arguments */
        else
        {
            printf("Unrecognized argument:\n");
            printf(argv[i]);
            exit(1);
        }
    }

    return opts;
}