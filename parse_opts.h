#ifndef __PARSE_OPTS__H
#define __PARSE_OPTS__H
#include <string>
#include <cstring>

using namespace std;

struct cmdOpts{
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
};

cmdOpts parse_options(int argc, char* argv[]);

#endif //__PARSE_OPTS__H
