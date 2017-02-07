#ifdef __unix__
/* libsndfile uses __int64 type, which isn't available on Linux... */
#include <cinttypes>
typedef int64_t __int64;
#endif


#include "RtAudio.h"

extern "C"{
	#include "sndfile.h"
}

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>
#include <csignal>
#include <cstring>


std::string const SOFTWARE_NAME = "chronicle";
std::string const VERSION = "0.1.1";

using namespace std;

// So, you've decided to make some logger software...

int main(int argc, char* argv[]);
void doRecord(string directory, string fileNameFormat);
int cb_record(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double    me, RtAudioStreamStatus status, void *userData);
void stopRecord();
void signalHandler(int sigNum);

void printLicence();
void printHelp();