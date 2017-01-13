#include "chronicle.h"

using namespace std;

SNDFILE* mySnd;

int main(int argc, char* argv[])
{
	doRecord(argv[1]);
	return 0;
}

void doRecord(char* directory) {
	RtAudio audio;

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
	unsigned int bufferFrames = 512;

	SF_INFO sfInfo;
	sfInfo.channels = 2;
	sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	sfInfo.samplerate = sampleRate;

	if (sf_format_check(&sfInfo) == 0) {
		cout << "Destination format invalid; Exiting..." << endl;
		exit(0);
	}

	/* The time to finish recording is at the end of the hour.
	We can't assume that the recording starting from the top of the current hour,
	as the first recording probably won't be.
	So, we'll add an hour to the current time, and then remove any minutes and
	seconds from that time.
	*/

	while (true){

		chrono::time_point<chrono::system_clock> nowChrono, endChronoInaccurate, endChronoAccurate;
		nowChrono = chrono::system_clock::now();

		endChronoInaccurate = nowChrono + chrono::seconds(3600);
		// Convert the end-time into a form than we can manipulate
		time_t end_tt = chrono::system_clock::to_time_t(endChronoInaccurate);
		struct tm* end_tm = localtime(&end_tt);
		// And remove any extraneous hours/minutes so the time is at the top of hour
		end_tm->tm_min = 0;
		end_tm->tm_sec = 0;
		cout << "endtime adj " << put_time(end_tm, "%F %H%M%S") << endl;

		// Now convert back to a chrono

		end_tt = mktime(end_tm);
		endChronoAccurate = chrono::system_clock::from_time_t(end_tt);
		// Now we can pass that to sleepUntil to finish the recording at the correct time!

		string audioFileFullPath;
		audioFileFullPath = directory;
#ifdef _WIN32
		if (audioFileFullPath.back() != '\\') {
			audioFileFullPath += '\\';
		}
#endif
#ifdef unix
		if (audioFileFullPath.back() != '/') {
			audioFileFullPath += '/';
	}
#endif
		
		char* audioFileName;
		time_t now_tt = chrono::system_clock::to_time_t(nowChrono);
		strftime(audioFileName, 80, "%F %H%M%S.wav", localtime(&now_tt));
		
		audioFileFullPath += audioFileName;

		mySnd = sf_open(audioFileFullPath.c_str(), SFM_WRITE, &sfInfo);

		try {
			cout << "Recording to " << audioFileName <<endl;
			audio.openStream(NULL, &params, RTAUDIO_SINT16, sampleRate, &bufferFrames, &cb_record);
			audio.startStream();
		}
		catch (RtAudioError &e) {
			cout << "Could not open stream: " << e.getMessage() << endl;
			exit(0);
		}

		this_thread::sleep_until(endChronoAccurate);
		cout << "Recording completed." << endl;

		try {
			audio.stopStream();
		}
		catch (RtAudioError &e) {
			cout << "Could not stop stream: " << e.getMessage() << endl;
		}

		sf_write_sync(mySnd);
		if (audio.isStreamOpen()) { audio.closeStream(); }
		sf_close(mySnd);
	}
}

int cb_record(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData)
{
	short* data = (short *)inputBuffer;
	sf_writef_short(mySnd, data, nFrames);

	return 0;
}