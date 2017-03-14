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

SNDFILE *mySnd;
RtAudio audio;

chrono::seconds audioFileAgeLimit = chrono::seconds(3628800);
int soundFormat = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
string audioFileExtension = ".wav";
unsigned int inputAudioDeviceId = audio.getDefaultInputDevice();

bool silent_flag = 0;

int main(int argc, char *argv[])
{
    cout << SOFTWARE_NAME << " v" << VERSION << " Copyright (c) 2016-2017 Callum McLean" << endl
	 << endl;

    boost::filesystem::path output_directory;
    output_directory = boost::filesystem::current_path();

    string fileNameFormat = "%Y-%m-%d %H%M%S"; // Default strftime format for audio files. MinGW doesn't like %F...

    /* Parse cmd-line arguments */
    {
	cmdOpts opts = parse_options(argc, argv);

	/* Not recording, just querying - these options end with the program exiting */

	if (opts.licence == true)
	{
	    printLicence();
	    exit(0);
	}
	else if (opts.help == true)
	{
	    printHelp();
	    exit(0);
	}
	else if (opts.list_devices == true)
	{
	    /* List the input devices that are available and exit. */

	    unsigned int devices = audio.getDeviceCount();
	    RtAudio::DeviceInfo deviceInfo;

	    for (unsigned int i = 0; i < devices; i++)
	    {
		deviceInfo = audio.getDeviceInfo(i);
		if (deviceInfo.probed == true && deviceInfo.inputChannels != 0)
		{
		    cout << "#" << i << ": " << deviceInfo.name << endl;
		    cout << "    Channel count: " << deviceInfo.inputChannels << endl;
		}
	    }

	    deviceInfo = audio.getDeviceInfo(audio.getDefaultInputDevice());
	    cout << endl
		 << "Default device: " << deviceInfo.name << endl;

	    exit(0);
	}

	/* Check for conflicting options */
	if ((opts.max_age != 0) && (opts.no_delete == true))
	{
	    printf("Cannot supply --no-delete and --max-age together; they are incompatible");
	    exit(1);
	}

	/* Validate and set recording options */
	if (opts.directory != "")
	{
	    boost::filesystem::path proposedDir = opts.directory;

	    output_directory = proposedDir;
	}

	if (opts.format != "")
	{
	    fileNameFormat = opts.format;
		
		/* strftime seems to not produce anything when passed %F on MinGW-compiled versions,
		Fixes #26 */
		size_t found = fileNameFormat.find("%F");
		while (found != string::npos){
			fileNameFormat.replace(found,2,"%Y-%m-%d");
			found = fileNameFormat.find("%F");
		}

	}

	if (opts.no_delete == true)
	{
	    /* Effectively set the threshold for when files should be deleted
			to be in the future */
	    audioFileAgeLimit == chrono::seconds(-10);
	}

	if (opts.max_age != 0)
	{
	    if (opts.max_age < 1)
	    {
		cout << "The specified file age limit must be greater than 1 second:";
		cout << opts.max_age << endl;
		exit(1);
	    }

	    audioFileAgeLimit = chrono::seconds(opts.max_age);
	}

	if (opts.audio_format != "")
	{
	    if (opts.audio_format == "OGG" || opts.audio_format == "ogg")
	    {
		soundFormat = SF_FORMAT_OGG | SF_FORMAT_VORBIS;
		audioFileExtension = ".ogg";
	    }
	    else if (opts.audio_format == "WAV" || opts.audio_format == "wav")
	    {
		soundFormat = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		audioFileExtension = ".wav";
	    }
	    else
	    {
		cout << "Audio file format not supported:" << endl;
		cout << opts.audio_format << endl;
		cout << "Supported formats are: [ OGG | WAV ]" << endl;
		exit(1);
	    }
	}

	if (opts.input_device != -1)
	{
	    RtAudio::DeviceInfo proposedDeviceInfo;

	    /* Does a device exist with the provided ID? */
	    if (opts.input_device > (audio.getDeviceCount() - 1))
	    {
		cout << "No audio device found with ID " << opts.input_device << endl;
		cout << "Use:" << endl;
		cout << "    chronicle --list-devices" << endl;
		cout << "To find a list of available device IDs." << endl;
		exit(1);
	    }

	    /* Is the device an input device? */
	    cout << "Getting device info" << endl;
	    proposedDeviceInfo = audio.getDeviceInfo(opts.input_device);
	    if (proposedDeviceInfo.inputChannels == 0)
	    {
		cout << "The specified audio device is not an input device:" << endl;
		cout << "#" << opts.input_device << ": " << proposedDeviceInfo.name << endl;
		exit(1);
	    }

	    inputAudioDeviceId = opts.input_device;
	}
    }

    /* Make sure that audio devices exist before continuing... */
    if (audio.getDeviceCount() < 1)
    {
	cout << "No audio devices available! Exiting..." << endl;
	exit(0);
    }

    string windowTitle = "Chronicle v" + VERSION;
    initCurses(windowTitle);

    doRecord(output_directory, fileNameFormat);

    closeCurses();
    return 0;
}

void doRecord(boost::filesystem::path directory, string fileNameFormat)
{

    RtAudio::StreamParameters params;
    RtAudio::DeviceInfo deviceInfo = audio.getDeviceInfo(inputAudioDeviceId);
    //cout << "Using input device: " << deviceInfo.name << endl;

    recordingParameters rp = getRecordingParameters(deviceInfo);

    params.deviceId = inputAudioDeviceId;
    params.nChannels = rp.channelCount;
    params.firstChannel = 0;

    SF_INFO sfInfo;
    sfInfo.channels = rp.channelCount;
    sfInfo.format = soundFormat;
    sfInfo.samplerate = rp.sampleRate;

    updateAudioDevice(deviceInfo.name, rp.sampleRate, rp.channelCount);

    if (sf_format_check(&sfInfo) == 0)
    {
	//cout << "Destination format invalid; Exiting..." << endl;
	exit(0);
    }

    sf_command(mySnd, SFC_SET_SCALE_FLOAT_INT_READ, NULL, SF_TRUE);

    // Set up signal handling; fixes #1
    {
	signal(SIGINT, signalHandler);
	signal(SIGABRT, signalHandler);
	//signal(SIGBREAK, signalHandler);
	//signal(, handleWindowRedraw); // TODO: HANDLE TERMINAL RESIZE
    }

    /* The time to finish recording is at the end of the hour.
	We can't assume that the recording starting from the top of the current hour,
	as the first recording probably won't be.
	So, we'll add an hour to the current time, and then remove any minutes and
	seconds from that time.
	*/

    while (true)
    {
	removeOldAudioFiles(audioFileAgeLimit, directory);

	chrono::time_point<chrono::system_clock> endTime = calculateRecordEndTimeFromNow();

	char audioFileName[81];
	time_t now_tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
	struct tm now_tm;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	localtime_s(&now_tm, &now_tt); // Use localtime_s on windows
#else
	localtime_r(&now_tt, &now_tm); // Use localtime_r on POSIX
#endif
	strftime(audioFileName, 80, fileNameFormat.c_str(), &now_tm);

	boost::filesystem::path audioFileFullPath;
	audioFileFullPath = directory;

	audioFileFullPath /= audioFileName;
	audioFileFullPath.replace_extension(audioFileExtension);

	/* If the directory so far does not exist - create it.
		This is helpful if supplying directory arguments as part of --format;
		for example, -f "%Y/%M/D/%H-%m-%s.wav" would produce
		'2017/03/13/16-00-00.wav'.
		Fixes #23 */

	try
	{
		boost::filesystem::create_directories(audioFileFullPath.parent_path());
	}
	catch (boost::filesystem::filesystem_error& e)
	{
		printf(e.what());
		exit(1);
	}

	mySnd = sf_open(audioFileFullPath.generic_string().c_str(), SFM_WRITE, &sfInfo);

	try
	{
	    updateRecordingToPath(audioFileFullPath.generic_string());
	    audio.openStream(NULL, &params, RTAUDIO_SINT16, rp.sampleRate, &(rp.bufferLength), &cb_record, &(rp.channelCount));
	    audio.startStream();
	}
	catch (RtAudioError &e)
	{
	    //cout << "Could not open stream: " << e.getMessage() << endl;
	    exit(0);
	}

	this_thread::sleep_until(endTime);
	//cout << "Recording completed." << endl;
	//cout << endl;

	stopRecord();
    }
}

int cb_record(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData)
{
    /* userData is the channel count. */

    int *pChannelCount = (int *)userData;
    int channelCount = *pChannelCount;

    short *data = (short *)inputBuffer;
    sf_writef_short(mySnd, data, nFrames);

    /* To figure out if a buffer is silent, we need to check the content of the buffer.
	The buffer is just the (number of frames) * (the number of channels); in our case,
	nFrames*2. Each buffer is then just a number, representing the amplitude of the audio.
	Therefore, if the buffer is empty, the audio is silent.

	The data is cast to a short. sizeof(short) = 2 (bytes) = 16 bits.
	Max value in a short is therefore 2^16 -1 = 65535.
	However, since the values are read from -1 to +1, the range is halved to 32763.
	It is appropriate to assume that a -60dB is sufficient for silence detection. Anything
	lower than this is negligible, especially for analogue broadcast.

	We can use this to figure out our silence detection.
	Assuming 65535 is max volume (0dB).

	I_db = 10 * log10(I / I_0)
	-> I_db / 10 = log10(I / I_0)
	-> 10^(I_db / 10) = I / I_0
	-> I = 10^(I_db / 10) * I_0
	-> I = 10^(-60 / 10) * 32763
	-> I = 10^(-6) * 32763
	-> I = 0.065535
	*/
    //cout << streamTime << endl;
    //if ((long)round(streamTime) % 2 == 0) {
    //cout << (long)round(streamTime) << endl;

    short maxAudioVal = (pow(2, (sizeof(short) * 8)) / 2) - 1;
    int thresholdDB = -60;
    float thresholdVal = (pow(10, thresholdDB / 10)) * maxAudioVal;

    short framesPeak = 0;

    for (int i = 0; i < nFrames * channelCount; i++)
    {
	short val = abs(*(data + i));
	framesPeak = max(val, framesPeak);
    };

    float level = log10(float(framesPeak) / float(maxAudioVal)) * 10;
    //float level = float(framesPeak)/float(maxAudioVal);
    string label = to_string(level) + " dB";

    updateAudioMeter(0, 30, 30 - abs(level), label);
    //updateAudioMeter(0,maxAudioVal,framesPeak,to_string(level));

    return 0;
}

void stopRecord()
{

    //cout << "\r" << endl;

    try
    {
	audio.stopStream();
    }
    catch (RtAudioError &e)
    {
	//cout << "Could not stop stream: " << e.getMessage() << endl;
    }

    if (audio.isStreamOpen())
    {
	audio.closeStream();
    }
    sf_write_sync(mySnd);
    sf_close(mySnd);
}

recordingParameters getRecordingParameters(RtAudio::DeviceInfo recordingDevice)
{
    recordingParameters rp;

    /* Set channel count */
    if (recordingDevice.inputChannels == 1)
    {
	rp.channelCount = 1;
    }
    else if (recordingDevice.inputChannels >= 2)
    {
	rp.channelCount = 2;
    }
    //cout << "Recording channels count: " << rp.channelCount << endl;

    /* Set sample rate - prefer 44100 */
    if (recordingDevice.preferredSampleRate == 44100)
    {
	rp.sampleRate = 44100;
    }
    else
    {
	for (std::vector<unsigned int>::iterator i = recordingDevice.sampleRates.begin(); i != recordingDevice.sampleRates.end(); i++)
	{
	    if (*i == 44100)
	    {
		rp.sampleRate = 44100;
	    }
	}

	if (!rp.sampleRate)
	{
	    rp.sampleRate = recordingDevice.preferredSampleRate;
	    //cout << "Could not set sample rate at 44.1 kHz, using preferred sample rate: " << rp.sampleRate << endl;
	}
    }
    //cout << "Sample rate: " << rp.sampleRate << endl;

    rp.bufferLength = 1024;

    //cout << endl;
    return rp;
}

chrono::time_point<chrono::system_clock> calculateRecordEndTimeFromNow()
{

    chrono::time_point<chrono::system_clock> nowChrono, endChronoInaccurate, endChronoAccurate;
    nowChrono = chrono::system_clock::now();

    endChronoInaccurate = nowChrono + chrono::seconds(3600);
    // Convert the end-time into a form than we can manipulate
    time_t end_tt = chrono::system_clock::to_time_t(endChronoInaccurate);
    struct tm *end_tm = localtime(&end_tt);
    // And remove any extraneous hours/minutes so the time is at the top of hour
    end_tm->tm_min = 0;
    end_tm->tm_sec = 0;

    // Now convert back to a chrono
    end_tt = mktime(end_tm);
    endChronoAccurate = chrono::system_clock::from_time_t(end_tt);

    return endChronoAccurate;
}

void removeOldAudioFiles(chrono::seconds age, boost::filesystem::path directory)
{
    /* Iterate over the files in a directory (presumably the output directory) and delete
	audio files older than a certain age
	*/

    chrono::system_clock::time_point nowChrono, oldestTimeChrono, fileMTime;
    nowChrono = chrono::system_clock::now();
    oldestTimeChrono = nowChrono - age;

    boost::filesystem::directory_iterator dirIterEnd = boost::filesystem::directory_iterator();
    boost::filesystem::directory_iterator dirIter = boost::filesystem::directory_iterator(directory);

    while (dirIter != dirIterEnd)
    {
	boost::filesystem::directory_entry dirEntry;
	dirEntry = *dirIter;

	chrono::system_clock::time_point fileMTime = chrono::system_clock::from_time_t(boost::filesystem::last_write_time(dirEntry.path()));

	if ((fileMTime < oldestTimeChrono) & (dirEntry.path().extension() == audioFileExtension))
	{
	    //cout << "Removing old audio file: " << dirEntry.path().filename() << endl;
	    boost::filesystem::remove(dirEntry.path());
	}

	dirIter++;
    }
}

void signalHandler(int sigNum)
{
    //cout << "Received signal " << sigNum << "; shutting down...";
    stopRecord();
    closeCurses();
    exit(sigNum);
}

void printLicence()
{
    /* TODO: ADD BOOST WEBSITE + COPYRIGHT */
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
)";

    cout << LICENCE << endl;
}

void printHelp()
{

    const char USAGE[] =
	R"(
Usage:
    chronicle [-h | --help]
    chronicle [--licence]
    chronicle [-l | --list-devices ]
    chronicle [-d | --directory OUTPUT_DIRECTORY] [-f | --filename FORMAT] [-i | --input-device DEVICE_ID] 
              [[-a | --max-age MAX_FILE_AGE] | --no-delete] [-s | --audio-format [WAV | OGG]]

    Where:
        -h | --help          Prints this help message.
        --licence            Prints the licence information for this software and libraries that it uses.
        -l | --list-devices  Lists the available input devices with their IDs.
        -d | --directory     Sets the directory to save the logged audio to. A trailing slash is not required, but may
                                 be added. On Windows, if using a trailing slash, use a trailing double-slash.
                                 Defaults to current directory.
        -f | --format        strftime-compatible format to use when naming the audio files.
                                 Defaults to %F %H%M%S .
        -i | --input-device  The ID number of the input device to record from. A list of input devices and their ID
                                 numbers can be obtained with `chronicle -l`.
                                 If unspecified, the system default audio recording device will be used.
        -a | --max-age       Sets the maximum age (in seconds) before audio files will be automatically deleted.
                                 Defaults to 3628800 (42 days, in accordance with OFCOM rules).
		--no-delete          If passed, Chronicle will not delete old audio files, so they can be manually managed.
		                         Incompatible with --max-age.
        -s | --audio-format Sets the audio format to use for the recorded audio files.
                                 Acceptable parameters are:
                                     OGG | Ogg Vorbis (.ogg)
                                     WAV | 16-bit PCM WAV (.wav)
                                 Defaults to WAV.
)";

    cout << USAGE << endl;
}