#ifdef __unix__
/* libsndfile uses __int64 type, which isn't available on Linux... */
#include <cinttypes>
typedef int64_t __int64;
#endif

#include "version.h"
#include "screen.h"
#include "parse_opts.h"
#include <RtAudio.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <lame/lame.h>

extern "C"
{
#include "sndfile.h"
}

#include <boost/version.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>
#include <csignal>
#include <cstring>

using namespace std;

// So, you've decided to make some logger software...

struct recordingParameters
{
	unsigned int channelCount;
	unsigned int firstChannel;
	unsigned int sampleRate;
	unsigned int bufferLength;
};

int main(int argc, char *argv[]);
void doRecord(boost::filesystem::path directory, string fileNameFormat);

float calculateHardDriveUsage(chrono::seconds duration, recordingParameters rp);

recordingParameters getRecordingParameters(RtAudio::DeviceInfo recordingDevice);

chrono::time_point<chrono::system_clock> calculateRecordEndTimeFromNow();

int cb_record(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double me, RtAudioStreamStatus status, void *userData);
void stopRecord();
void signalShutdownHandler(int sigNum);
void signalWinResizeHandler(int sigNum);
void onRtAudioError(RtAudioError::Type type, const string &errorText);

void removeOldAudioFiles(chrono::seconds age, boost::filesystem::path directory);

// void printHelp();