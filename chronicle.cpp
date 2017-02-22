/*
MIT License

Chronicle: an audio logger.
Copyright (c) 2016-2017 Callum McLean

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "chronicle.h"

using namespace std;

SNDFILE* mySnd;
RtAudio audio;

bool silent_flag = 0;

int main(int argc, char* argv[])
{
	cout << SOFTWARE_NAME << " v" << VERSION << " Copyright (c) 2016-2017 Callum McLean" << endl;
	
	string output_directory = "."; // Default directory to save files to
	string fileNameFormat = "%Y-%m-%d %H%M%S.wav"; // Default strftime format for audio files. MinGW doesn't like %F...
	
	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "--licence"))
		{
			printLicence();
			exit(0);
		}
		else if(!strcmp(argv[i], "-h") || !strcmp(argv[i],"--help"))
		{
			printHelp();
			exit(0);
		}
		else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--directory"))
		{
			output_directory = argv[i + 1];
			i++; // Skip parsing the next argument
		}
		else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--filename"))
		{
			fileNameFormat = argv[i + 1];
			i++;
		}
	}
	
	doRecord(output_directory, fileNameFormat);
	return 0;
}

void doRecord(string directory, string fileNameFormat) {

	// How many devices are available?
	unsigned int devices = audio.getDeviceCount();
	if (devices < 1) {
		cout << "No audio devices available! Exiting..." << endl;
		exit(0);
	}

	// TODO: Check for *input* devices specifically

	/*
	// Tell us about the devices available

	cout << devices << " devices available" << endl;
	for (unsigned int i=0; i<devices; i+=1){
		RtAudio::DeviceInfo deviceInfo = audio.getDeviceInfo(i);
		cout << "Device #" << i << ": " << deviceInfo.name << endl;
		if (deviceInfo.probed != true){
			cout << "    Could not probe device! May not be usable." << endl;
			continue;
		}
		cout << "    Input channels: " << deviceInfo.inputChannels << endl;
		cout << "    Output channels: " << deviceInfo.outputChannels << endl;
		cout << "    Max duplex channels: " << deviceInfo.duplexChannels << endl;
		cout << "    Sample rates: ";

		for (unsigned int j = 0; j < deviceInfo.sampleRates.size(); j++) {
			cout << deviceInfo.sampleRates[j] << "; ";
		}
		cout << endl;

		cout << "    Preferred sample rate: " << deviceInfo.preferredSampleRate << endl;
		cout << "    Data formats: ";

		RtAudioFormat deviceAudioFormats = deviceInfo.nativeFormats;
		if (deviceAudioFormats & RTAUDIO_SINT8){
			cout << "Signed 8-bit int; ";
		}
		if (deviceAudioFormats & RTAUDIO_SINT16){
			cout << "Signed 16-bit int; ";
		}
		if (deviceAudioFormats & RTAUDIO_SINT24){
			cout << "Signed 24-bit int; ";
		}
		if (deviceAudioFormats & RTAUDIO_SINT32){
			cout << "Signed 32-bit int; ";
		}
		if (deviceAudioFormats & RTAUDIO_FLOAT32){
			cout << "32-bit float; ";
		}
		if (deviceAudioFormats & RTAUDIO_FLOAT64){
			cout << "64-bit double; ";
		}

		cout << endl;
	}
	*/

	RtAudio::StreamParameters params;
	params.deviceId = audio.getDefaultInputDevice();
	//params.deviceId = 2;
	RtAudio::DeviceInfo info = audio.getDeviceInfo(params.deviceId);
	cout << "Using input device: " << info.name << endl;
	params.nChannels = 2;
	params.firstChannel = 0;
	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 1024;

	SF_INFO sfInfo;
	sfInfo.channels = 2;
	sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	sfInfo.samplerate = sampleRate;

	if (sf_format_check(&sfInfo) == 0) {
		cout << "Destination format invalid; Exiting..." << endl;
		exit(0);
	}

	// Set up signal handling; fixes #1
	{
		signal(SIGINT, signalHandler);
		signal(SIGABRT, signalHandler);
	}

	/* The time to finish recording is at the end of the hour.
	We can't assume that the recording starting from the top of the current hour,
	as the first recording probably won't be.
	So, we'll add an hour to the current time, and then remove any minutes and
	seconds from that time.
	*/

	while (true) {

		chrono::time_point<chrono::system_clock> nowChrono, endChronoInaccurate, endChronoAccurate;
		nowChrono = chrono::system_clock::now();

		endChronoInaccurate = nowChrono + chrono::seconds(3600);
		// Convert the end-time into a form than we can manipulate
		time_t end_tt = chrono::system_clock::to_time_t(endChronoInaccurate);
		struct tm* end_tm = localtime(&end_tt);
		// And remove any extraneous hours/minutes so the time is at the top of hour
		end_tm->tm_min = 0;
		end_tm->tm_sec = 0;

		// Now convert back to a chrono
		end_tt = mktime(end_tm);
		endChronoAccurate = chrono::system_clock::from_time_t(end_tt);
		// Now we can pass that to sleepUntil to finish the recording at the correct time!

		
#ifdef _WIN32
		if (directory.back() != '\\') {
			directory += '\\';
		}
#endif
#ifdef __unix__
		if (directory.back() != '/') {
			directory += '/';
		}
#endif 

		char audioFileName[81];
		time_t now_tt = chrono::system_clock::to_time_t(nowChrono);
		tm now_tm;
		localtime_s(&now_tm, &now_tt);

		strftime(audioFileName, 80, fileNameFormat.c_str(), &now_tm);

		string audioFileFullPath = directory;
		audioFileFullPath += audioFileName;

		mySnd = sf_open(audioFileFullPath.c_str(), SFM_WRITE, &sfInfo);

		try {
			cout << "Recording to " << audioFileFullPath.c_str() << endl;
			audio.openStream(NULL, &params, RTAUDIO_SINT16, sampleRate, &bufferFrames, &cb_record);
			audio.startStream();
		}
		catch (RtAudioError &e) {
			cout << "Could not open stream: " << e.getMessage() << endl;
			exit(0);
		}

		this_thread::sleep_until(endChronoAccurate);
		cout << "Recording completed." << endl;
		cout << endl;

		stopRecord();
	}
}

int cb_record(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData)
{
	short* data = (short*)inputBuffer;
	sf_writef_short(mySnd, data, nFrames);

	/* To figure out if a buffer is silent, we need to check the content of the buffer.
	The buffer is just the (number of frames) * (the number of channels); in our case,
	nFrames*2. Each buffer is then just a number, representing the amplitude of the audio.
	Therefore, if the buffer is empty, the audio is silent.

	The data is cast to a short. sizeof(short) = 2 (bytes) = 16 bits.
	Max value in a short is therefore 2^16 -1 = 65535.
	It is appropriate to assume that a -60dB is sufficient for silence detection. Anything
	lower than this is negligible, especially for analogue broadcast.

	We can use this to figure out our silence detection.
	Assuming 65535 is max volume (0dB).

	I_db = 10 * log10(I / I_0)
	-> I_db / 10 = log10(I / I_0)
	-> 10^(I_db / 10) = I / I_0
	-> I = 10^(I_db / 10) * I_0
	-> I = 10^(-60 / 10) * 65535
	-> I = 10^(-6) * 65535
	-> I = 0.065535
	*/
	//cout << streamTime << endl;
	//if ((long)round(streamTime) % 2 == 0) {
		//cout << (long)round(streamTime) << endl;
		float threshold = 0.065535;

		unsigned int framesSum;
		float framesAvg;
		for (int i = 0; i < nFrames * 2; i++) {
			framesSum += abs((*(data + i)));
		};

		framesAvg = framesSum / nFrames;
		float level = log10(framesAvg / 65535) * 10;
		/*cout << "\rLevel: " << level << "dB";
		if (framesAvg < threshold) { cout << "silence at "<<streamTime << " (" << framesAvg << ")"; }
		cout << flush;*/

		float numberOfEquals = (framesAvg / 65535) * 76;
		cout << "\r| ";
		cout << string(numberOfEquals, '=');
		cout << string(76 - numberOfEquals, ' ');
		cout << "]  " << level << " dB" << flush;
	//}
	return 0;
}

void stopRecord() {

	cout << "\r" << endl;

	try {
		audio.stopStream();
	}
	catch (RtAudioError &e) {
		cout << "Could not stop stream: " << e.getMessage() << endl;
	}

	if (audio.isStreamOpen()) { audio.closeStream(); }
	sf_write_sync(mySnd);
	sf_close(mySnd);
}

void removeOldAudioFiles(chrono::seconds age, string directory) {
	/* Iterate over the files in a directory (presumably the output directory) and delete
	audio files older than a certain age
	*/

	chrono::time_point<chrono::system_clock> nowChrono, oldestTimeChrono;
	nowChrono = chrono::system_clock::now();
	oldestTimeChrono = nowChrono - age;
}

void signalHandler(int sigNum) {
	cout << "Received signal " << sigNum << "; shutting down...";
	stopRecord();
	exit(sigNum);
}

void printLicence() {
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
        Licenced under the the RtAudio licence.
        Copyright (c) Gary P. Scavone, McGill University
        https://www.music.mcgill.ca/~gary/rtaudio/
)";
    
    cout << LICENCE << endl;
}

void printHelp() {

	const char USAGE[] =
		R"(
Usage:
    chronicle [-h | --help] [--licence] [-d | --directory OUTPUT_DIRECTORY] [-f | --filename FORMAT]

    Where:
        -h | --help         Prints this help message.
        --licence           Prints the licence information for this software and libraries that it uses.
        -d | --directory    Sets the directory to save the logged audio to. A trailing slash is not required, but may
                            be added. On Windows, if using a trailing slash, use a trailing double-slash. Defaults to
                            current directory.
        -f | --format       strftime-compatible format to use when naming the audio files. Defaults to %F %H%M%S.wav .
)";

	cout << USAGE << endl;

}