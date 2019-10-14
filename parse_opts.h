#ifndef __PARSE_OPTS__H
#define __PARSE_OPTS__H
#include <string>
#include <cstring>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace std;

struct cmdOpts
{
    bool licence = false;
    bool help = false;
    bool list_devices = false;
    string directory = "";
    string format = "";
    bool no_delete = false;
    unsigned int max_age_value = 0;
    string max_age_unit = "";
    string audio_format = "";
    int input_device = -1;
    bool no_term = false;
    bool is_debug = false;
    unsigned int device_first_channel = 0;
    unsigned int device_channel_count = 2;
};

boost::program_options::variables_map parse_cmd_opts(int argc, char *argv[]);
void printLicence();

#endif //__PARSE_OPTS__H
