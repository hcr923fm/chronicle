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

using namespace std;

// So, you've decided to make some logger software...

int main(int argc, char* argv[]);
void doRecord(char* directory);
int cb_record(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData);