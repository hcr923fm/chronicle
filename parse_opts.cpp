#include "parse_opts.h"
#include <cerrno>
#include <cstdio>

cmdOpts parse_options(int argc, char* argv[]){
    cmdOpts opts;
    for (int i=1; i<argc;i++){
        /* --licence */
        if (!strcmp(argv[i], "--licence"))
		{
			opts.licence = true;
		}

        /* --help */
        else if(!strcmp(argv[i], "-h") || !strcmp(argv[i],"--help"))
		{
			opts.help = true;
		}

        /* --list-devices */
        else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--list-devices")){
            opts.list_devices = true;
        }

        /* --directory */
        else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--directory")){
            opts.directory = argv[i+1];
            i++;
        }

        /* --format */
        else if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--format")){
            opts.format = argv[i+1];
            i++;
        }

        /* --no-delete */
        else if (!strcmp(argv[i], "--no-delete")){
            opts.no_delete = true;
        }

        /* --max-age */
        else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--max-age")) {
            char * endptr;
            unsigned int n = strtoul(argv[i+1], &endptr, 10);

            if (endptr != argv[i+1]){ // Conversion didn't happen
                opts.max_age = n;
                i++;
            } else {
                printf("Argument supplied for --max-age is invalid (must be a number)\n");
                printf(argv[i+1]);
                exit(0);
            }
            
        }

        /* --audio-format */
        else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--audio-format")) {
            opts.audio_format = argv[i+1];
            i++;
        }

        /* --input-device */
        else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input-device")){
            char * endptr;
            unsigned int n = strtoul(argv[i+1], &endptr, 10);

            if (endptr != argv[i+1]){ // Conversion didn't happen
                opts.input_device = n;
                i++;
            } else {
                printf("Argument supplied for --input-device is invalid (must be a number)\n");
                printf(argv[i+1]);
                exit(0);
            }
        }

        /* unrecognized arguments */
        else {
            printf("Unrecognized argument:\n");
            printf(argv[i]);
            exit(1);
        }

    }

    return opts;
}