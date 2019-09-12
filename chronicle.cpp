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

enum AudioFormat
{
	WAV,
	OGG,
	MP3,
	FLAC
};

// Libsndfile stuff
SNDFILE *mySnd;
SF_INFO sfInfo;

// LAME stuff
lame_t lame_enc;
FILE *lameOutFile;

// The rest
AudioFormat destinationAudioFormat = AudioFormat::WAV;
string audioFileExtension = ".wav";
int sfSoundFormat = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
RtAudio audio;

/* The data is cast to a short. sizeof(short) = 2 (bytes) = 16 bits.
	Max value in a short is therefore 2^16 -1 = 65535.
	However, since the values are read from -1 to +1, the range is halved to 32763.
	It is appropriate to assume that a -40dB is sufficient for silence detection. Anything
	lower than this is negligible, especially for analogue broadcast.

	We can use this to figure out our silence detection.
	Assuming 65535 is max volume (0dB).

	I_db = 10 * log10(I / I_0)
	-> I_db / 10 = log10(I / I_0)
	-> 10^(I_db / 10) = I / I_0
	-> I = 10^(I_db / 10) * I_0
	-> I = 10^(-40 / 10) * 32763
	-> I = 10^(-4) * 32763
	-> I = 0.065535
	*/

const short maxAudioVal = (pow(2, (sizeof(short) * 8)) / 2) - 1;
const int silenceThresholdDB = -40;
const float thresholdVal = (pow(10, silenceThresholdDB / 10)) * maxAudioVal;

chrono::seconds audioFileAgeLimit = chrono::seconds(3628800);

unsigned int inputAudioDeviceId = audio.getDefaultInputDevice();
unsigned int inputAudioDeviceFirstChannel = 0;
unsigned int inputAudioDeviceChannels = 2;
unsigned int inputAudioDeviceSampleRate = 44100;

// cmdOpts opts;
boost::program_options::variables_map opts;
bool silent_flag = 0;

int main(int argc, char *argv[])
{
	cout << SOFTWARE_NAME << " v" << SOFTWARE_VERSION_MAJOR << "." << SOFTWARE_VERSION_MINOR << "." << SOFTWARE_VERSION_PATCH << " Copyright (c) 2016-2019 Callum McLean" << endl
		 << endl;

	boost::filesystem::path output_directory;
	// output_directory = boost::filesystem::current_path();

	string fileNameFormat = "%Y-%m-%d %H%M%S"; // Default strftime format for audio files. MinGW doesn't like %F...

	/* Init logging */
	try
	{
		boost::filesystem::create_directory("logs");
		auto logger = spdlog::rotating_logger_mt("chronicle_log", "logs/chronicle.log", 1024 * 1024 * 5, 3);
		logger->set_pattern("[%H:%M:%S %z] %l\t- %v");

		logger->info("Chronicle started...");
	}
	catch (const spdlog::spdlog_ex &ex)
	{
		cout << "Cannot init logging: " << ex.what() << endl;
		exit(1);
	}

	auto logger = spdlog::get("chronicle_log");

	/* Parse cmd-line arguments */
	{
		// opts = parse_options(argc, argv);
		opts = doAThing(argc, argv);

		// See if we're running in debug mode

		// if (opts.is_debug)
		if (opts.count("debug"))
		{
			logger->set_level(spdlog::level::debug);
			logger->info("Log level: Debug");
			logger->flush_on(spdlog::level::debug);
		}
		else
		{
			logger->set_level(spdlog::level::info);
			logger->info("Log level: Info");
			logger->flush_on(spdlog::level::info);
		}

		if (opts.count("list-devices"))
		{
			/* List the input devices that are available and exit. */

			unsigned int devices = audio.getDeviceCount();
			RtAudio::DeviceInfo deviceInfo;

			if (devices < 1)
			{
				cout << "No devices found! Exiting..." << endl;
				exit(0);
			}

			for (unsigned int i = 0; i < devices; i++)
			{
				deviceInfo = audio.getDeviceInfo(i);
				if (deviceInfo.probed == true && deviceInfo.inputChannels != 0)
				{
					cout << "#" << i << ": " << deviceInfo.name;
					if (deviceInfo.isDefaultInput)
					{
						cout << " (default)";
					}
					cout << endl;
					cout << "    Channel count: " << deviceInfo.inputChannels << endl;
				}
			}

			deviceInfo = audio.getDeviceInfo(audio.getDefaultInputDevice());
			cout << endl
				 << "Default device: " << deviceInfo.name << endl;

			exit(0);
		}

		/* Check for conflicting options */
		if (opts.count("no-delete") & opts.count("max-age") & !opts["max-age"].defaulted())
		{
			printf("Cannot supply --no-delete and --max-age together; they are incompatible\n");
			exit(1);
		}

		/* Validate and set recording options */
		if (opts.count("directory"))
		{
			boost::filesystem::path proposedDir = opts["directory"].as<string>();

			output_directory = proposedDir;
		}

		if (opts.count("format"))
		{
			fileNameFormat = opts["format"].as<string>();

			/* strftime seems to not produce anything when passed %F on MinGW-compiled versions,
			Fixes #26 */
			size_t found = fileNameFormat.find("%F");
			while (found != string::npos)
			{
				fileNameFormat.replace(found, 2, "%Y-%m-%d");
				found = fileNameFormat.find("%F");
			}
		}

		if (opts.count("no-delete"))
		{
			/* Effectively set the threshold for when files should be deleted
				to be in the future */
			audioFileAgeLimit == chrono::hours(-10);
		}

		if (opts.count("max-age"))
		{
			string max_age_string = opts["max-age"].as<string>();
			int max_age_val;
			string max_age_unit;
			try
			{
				max_age_val = stoi(max_age_string);
			}
			catch (invalid_argument e)
			{
				printf("Cannot identify a valid duration for --max-age (supplied value: %s )\n. See chronicle --help for more details.\n", max_age_string.c_str());
				exit(1);
			}

			int unit_str_idx = max_age_string.find_first_of("smhdSMHD");
			if (unit_str_idx == string::npos)
			{
				printf("Cannot identify a valid duration for --max-age (supplied value: %s )\n. See chronicle --help for more details.\n", max_age_string.c_str());
				exit(1);
			}

			max_age_unit = max_age_string.substr(unit_str_idx, 0);

			/* Allow the user to specify age limit in units other than seconds,
			Fixes #12 */
			chrono::seconds age_seconds;
			// if (opts.max_age_unit == "s")
			if (max_age_unit == "s" || max_age_unit == "S")
			{
				age_seconds = chrono::seconds(max_age_val);
			}
			if (max_age_unit == "m" || max_age_unit == "M")
			{
				age_seconds = chrono::minutes(max_age_val);
			}
			if (max_age_unit == "h" || max_age_unit == "H")
			{
				age_seconds = chrono::hours(max_age_val);
			}
			if (max_age_unit == "d" || max_age_unit == "D")
			{
				age_seconds = chrono::hours(max_age_val * 24);
			}

			// if (opts.max_age_value < 1)
			if (max_age_val == 1 & (max_age_unit == "s" || max_age_unit == "S"))
			{
				cout << "The specified file age limit must be greater than 1 second:";
				cout << max_age_val << endl;
				exit(1);
			}

			audioFileAgeLimit = age_seconds;
		}

		// if (opts.audio_format != "")
		if (opts.count("audio-format"))
		{
			string audio_format_string = opts["audio-format"].as<string>();
			logger->debug("Found proposed audio format: {}", audio_format_string);
			// if (opts.audio_format == "OGG" || opts.audio_format == "ogg")
			if (audio_format_string == "OGG" || audio_format_string == "ogg")
			{
				sfSoundFormat = SF_FORMAT_OGG | SF_FORMAT_VORBIS;
				audioFileExtension = ".ogg";
				destinationAudioFormat = AudioFormat::OGG;
			}
			else if (audio_format_string == "FLAC" || audio_format_string == "flac")
			{
				sfSoundFormat = SF_FORMAT_FLAC | SF_FORMAT_PCM_16;
				audioFileExtension = ".flac";
				destinationAudioFormat = AudioFormat::FLAC;
			}
			else if (audio_format_string == "WAV" || audio_format_string == "wav")
			{
				sfSoundFormat = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
				audioFileExtension = ".wav";
				destinationAudioFormat = AudioFormat::WAV;
			}
			else if (audio_format_string == "MP3" || audio_format_string == "mp3")
			{
				audioFileExtension = ".mp3";
				destinationAudioFormat = AudioFormat::MP3;
			}
			else
			{
				printf("Supplied audio file format not supported: %s\n", audio_format_string.c_str());
				printf("Supported formats are: [ OGG | WAV | MP3 | FLAC ]\n");
				exit(1);
			}
			logger->info("\tUsing audio format: {}", audio_format_string);
		}

		// if (opts.input_device != -1)
		if (opts.count("input-device"))
		{
			RtAudio::DeviceInfo proposedDeviceInfo;
			int input_device_id = opts["input-device"].as<int>();
			logger->debug("Found proposed input device ID: {}", input_device_id);

			/* Does a device exist with the provided ID? */
			if (input_device_id > (audio.getDeviceCount() - 1))
			{
				printf("No audio device found with ID %ul\n", input_device_id);
				printf("Use chronicle --list-devices to find a list of available device IDs.\n");
				exit(1);
			}

			/* Is the device an input device? */
			logger->debug("Getting info for device ID %l", input_device_id);
			proposedDeviceInfo = audio.getDeviceInfo(input_device_id);
			if (proposedDeviceInfo.inputChannels == 0)
			{
				printf("The specified audio device is not an input device: %ul : %s\n", input_device_id, proposedDeviceInfo.name.c_str());
				exit(1);
			}

			inputAudioDeviceId = input_device_id;
		}

		if (opts.count("device-first-channel"))
		{
			int proposed_first_channel = opts["device-first-channel"].as<int>();
			logger->debug("Found proposed device first channel: {}", proposed_first_channel);

			RtAudio::DeviceInfo device_info = audio.getDeviceInfo(inputAudioDeviceId);
			logger->debug("\t{} input channels available, {} requested as first", device_info.inputChannels, proposed_first_channel);

			if (device_info.inputChannels - 1 < proposed_first_channel)
			{
				printf("Option device-first-channel has been specified as %lu, but device %lu (%s) only has %lu input channels\n",
					   proposed_first_channel, inputAudioDeviceId, device_info.name.c_str(), device_info.inputChannels);
				exit(1);
			}

			inputAudioDeviceFirstChannel = proposed_first_channel;
		}

		if (opts.count("device-channels"))
		{
			int proposed_device_channels = opts["device-channels"].as<int>();
			logger->debug("Found proposed device channel count: {}", proposed_device_channels);

			RtAudio::DeviceInfo device_info = audio.getDeviceInfo(inputAudioDeviceId);
			logger->debug("\t{} input channels available, {}-{} requested for use", device_info.inputChannels, inputAudioDeviceFirstChannel, proposed_device_channels + inputAudioDeviceFirstChannel);

			if (inputAudioDeviceFirstChannel + proposed_device_channels > device_info.inputChannels)
			{
				printf("Option device-channels has been specified as %lu, but device %lu (%s) only has %lu input channels (first channel: %lu)\n",
					   proposed_device_channels, inputAudioDeviceId, device_info.name.c_str(), device_info.inputChannels, inputAudioDeviceFirstChannel);
				exit(1);
			}

			inputAudioDeviceChannels = proposed_device_channels;
		}

		if (opts.count("sample-rate"))
		{
			int proposed_sample_rate = opts["sample-rate"].as<int>();
			logger->debug("Found proposed sample rate: {}", proposed_sample_rate);

			RtAudio::DeviceInfo device_info = audio.getDeviceInfo(inputAudioDeviceId);
			unsigned int matching_rate = 0;
			for (auto it : device_info.sampleRates)
			{
				logger->debug("\ttesting supported sample rate {}", it);
				if (it == proposed_sample_rate)
				{
					matching_rate = proposed_sample_rate;
					logger->debug("\tsuccess!");
					break;
				}
			}

			if (!matching_rate)
			{
				printf("Device %lu (%s) does not support specified sample rate: %lu\n", inputAudioDeviceId,
					   device_info.name.c_str(), proposed_sample_rate);
				exit(1);
			}

			inputAudioDeviceSampleRate = matching_rate;
		}
	}

	/* Make sure that audio devices exist before continuing... */
	if (audio.getDeviceCount() < 1)
	{
		logger->critical("No audio devices available! Exiting...");
		exit(0);
	}

	logger->debug("Output directory: " + output_directory.string());

	// if (!opts.no_term)
	if (!opts.count("no-term"))
	{
		string windowTitle = "Chronicle v" + SOFTWARE_VERSION_MAJOR + "." + SOFTWARE_VERSION_MINOR + "." + SOFTWARE_VERSION_PATCH;
		initCurses(windowTitle);
	}

	doRecord(output_directory, fileNameFormat);

	if (!opts.count("no-term"))
	{
		closeCurses();
	}
	return 0;
}

void doRecord(boost::filesystem::path directory, string fileNameFormat)
{

	RtAudio::StreamParameters params;
	RtAudio::DeviceInfo deviceInfo = audio.getDeviceInfo(inputAudioDeviceId);
	auto logger = spdlog::get("chronicle_log");
	logger->info("Using input device: {}", deviceInfo.name);

	recordingParameters rp = getRecordingParameters(deviceInfo);

	params.deviceId = inputAudioDeviceId;
	params.nChannels = rp.channelCount;
	params.firstChannel = 0;

	// Generate libsndfile information if we're using it
	if (destinationAudioFormat == FLAC || destinationAudioFormat == OGG || destinationAudioFormat == WAV)
	{
		sfInfo.channels = rp.channelCount;
		sfInfo.format = sfSoundFormat;
		sfInfo.samplerate = rp.sampleRate;

		if (sf_format_check(&sfInfo) == 0)
		{
			logger->critical("Destination format invalid, exiting...");
			exit(0);
		}

		sf_command(mySnd, SFC_SET_SCALE_FLOAT_INT_READ, NULL, SF_TRUE);
	}

	// Or setup the MP3 encoder
	else if (destinationAudioFormat == MP3)
	{
		lame_enc = lame_init();
		lame_set_in_samplerate(lame_enc, rp.sampleRate);
		lame_set_out_samplerate(lame_enc, rp.sampleRate);
		lame_set_brate(lame_enc, 320);
		lame_set_VBR(lame_enc, vbr_off);
		//lame_set_VBR_quality(lame_enc, 3);
		lame_set_num_channels(lame_enc, rp.channelCount);
		int ret = lame_init_params(lame_enc);
		if (ret < 0)
		{
			logger->critical("Error occurred when initializing LAME parameters. Code: {}", ret);
		}
	}

	if (!opts.count("no-term"))
	{
		updateAudioDevice(deviceInfo.name, rp.sampleRate, rp.channelCount);
	}

	// Set up signal handling; fixes #1
	{
		signal(SIGINT, signalHandler);
		signal(SIGABRT, signalHandler);
		//signal(SIGBREAK, signalHandler);
		//signal(, handleWindowRedraw); // TODO: HANDLE TERMINAL RESIZE
	}

	// Main record loop begins

	while (true)
	{

		/* The time to finish recording is at the end of the hour.
	We can't assume that the recording starting from the top of the current hour,
	as the first recording probably won't be.
	So, we'll add an hour to the current time, and then remove any minutes and
	seconds from that time.
	*/
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
		catch (boost::filesystem::filesystem_error &e)
		{
			printf(e.what());
			logger->critical("Could not create directory: {}", e.what());
			exit(1);
		}
		catch (exception &e)
		{
			logger->critical("Could not create directory: {}", e.what());
		}

		/* Calculate the file size - fixes #22 */
		chrono::time_point<chrono::system_clock> tpNow = chrono::system_clock::now();
		chrono::seconds recordDuration = chrono::duration_cast<chrono::seconds>(endTime - tpNow);
		long fileSizeMB = calculateHardDriveUsage(recordDuration, rp);

		boost::filesystem::space_info diskSpace = boost::filesystem::space(directory);
		long diskSpaceAvailableGB = diskSpace.available / 1073741824; // bytes to GB
		if (!opts.count("no-term"))
		{
			updateHardDriveSpace(diskSpaceAvailableGB, fileSizeMB);
		}

		// Maintenance
		removeOldAudioFiles(audioFileAgeLimit, directory);

		// Open audio file for writing
		if (destinationAudioFormat != MP3)
		{
			mySnd = sf_open(audioFileFullPath.generic_string().c_str(), SFM_WRITE, &sfInfo);

			/* Check if the file can be opened. Fixes #6. */
			if (mySnd == NULL)
			{
				// Can't open the file. Exit.
				logger->critical("Could not start recording: {}", sf_strerror(mySnd));
				exit(1);
			}
		}
		else
		{
			lameOutFile = fopen(audioFileFullPath.generic_string().c_str(), "wb+");
			if (lameOutFile == NULL)
			{
				// Can't open the file. Exit.
				logger->critical("Couldn't open the destination file. Error: {}", strerror(errno));
				exit(1);
			}
		}

		try
		{
			if (!opts.count("no-term"))
			{
				updateRecordingToPath(audioFileFullPath.generic_string());
			}
			logger->debug("Updated recording output path: {}", audioFileFullPath.generic_string());
			audio.openStream(NULL, &params, RTAUDIO_SINT16, rp.sampleRate, &(rp.bufferLength), &cb_record, &(rp.channelCount), NULL, &onRtAudioError);
			logger->debug("Opened audio stream");
			audio.startStream();
			logger->info("Started new recording");
		}
		catch (RtAudioError &e)
		{
			logger->critical("Could not open stream: {}", e.getMessage());
			exit(0);
		}

		catch (exception &e)
		{
			logger->critical("Could not begin recording: {}", e.what());
		}

		this_thread::sleep_until(endTime);
		logger->info("Recording completed");

		stopRecord();
	}
}

int cb_record(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData)
{
	/* userData is the channel count. */

	int *pChannelCount = (int *)userData;
	int channelCount = *pChannelCount;
	short *data = (short *)inputBuffer;
	unsigned char MP3Buffer[8192];

	if (destinationAudioFormat == WAV || destinationAudioFormat == OGG || destinationAudioFormat == FLAC)
	{
		sf_writef_short(mySnd, data, nFrames);
	}
	else if (destinationAudioFormat == MP3)
	{
		int enc_data_size = lame_encode_buffer_interleaved(lame_enc, data, nFrames, MP3Buffer, 8192);
		fwrite(MP3Buffer, enc_data_size, 1, lameOutFile);
	}

	/* To figure out if a buffer is silent, we need to check the content of the buffer.
	The buffer is just the (number of frames) * (the number of channels); in our case,
	nFrames*numChannels. Each buffer is then just a number, representing the amplitude of the audio.
	Therefore, if the buffer is empty, the audio is silent. */

	// Do this for each channel
	for (int ch = 0; ch < channelCount; ch++)
	{
		short framesPeak = 0;

		// Out of all of the frames we have available, iterate over the ones for the given channel and find the loudest
		// It's interleaved audio (i.e. LRLRLRLRLR), so we skip every channelCount'th frame
		for (int i = ch; i < nFrames * channelCount; i += channelCount)
		{
			short val = abs(*(data + i));
			framesPeak = max(val, framesPeak);
		};

		// I_db = 10*log10(I/I_0)
		// Substitute 1 for framesPeak if it's actually 0, or the maths doesn't work. We can live we the inaccuracy at this low level.
		float level = 10 * (log10(float((framesPeak > 0) ? framesPeak : 1) / float(maxAudioVal)));

		char label[10];
		// If framesPeak < 0, print the level as being "-INF db"
		(framesPeak > 1) ? sprintf(label, "%02.2f dB", level) : sprintf(label, "  -INF dB");

		// TODO: Rather than calling this for every channel individually it would be nicer to just pass an array of values representing all of the channels...
		if (!opts.count("no-term"))
		{
			updateAudioMeter(ch, abs(silenceThresholdDB), abs(silenceThresholdDB) - abs(level), label);
		}
	}
	return 0;
}

void stopRecord()
{
	auto logger = spdlog::get("chronicle_log");

	try
	{
		audio.stopStream();
	}
	catch (RtAudioError &e)
	{
		logger->warn("Could not stop stream: {}", e.getMessage());
	}

	if (audio.isStreamOpen())
	{
		audio.closeStream();
	}

	if (destinationAudioFormat != MP3)
	{
		sf_write_sync(mySnd);
		sf_close(mySnd);
	}
	else
	{
		unsigned char discarded_buffer[8192];
		int remaining_frames = lame_encode_flush(lame_enc, discarded_buffer, 8192);
		logger->debug("Flushed LAME");
		lame_mp3_tags_fid(lame_enc, lameOutFile);
		fclose(lameOutFile);
		logger->debug("Closed destination file");
		// logger->debug("Cleared LAME encoder");
	}
}

float calculateHardDriveUsage(chrono::seconds duration, recordingParameters rp)
{
	/* Let's assume WAV, for now, because that's what we're recording... 
	OGG/MP3/etc. will change this.
	*/

	/* WAV file size:
	sz_in_mb = (bit_depth * sample_rate * channels * dur_in_secs) / 8 / 1000000
	*/

	if (destinationAudioFormat == WAV || destinationAudioFormat == FLAC)
	{
		return (rp.sampleRate * 16.00 * rp.channelCount * duration.count()) / 8 / 1024 / 1024;
	}
	else if (destinationAudioFormat == OGG)
	{
		// The OGG encoding uses VBR with a mean bit rate of 128 kbps
		return (128 * duration.count() / 8 / 1024);
	}
	else if (destinationAudioFormat == MP3)
	{
		// Our MP3 encoding uses VBR, but ~128kbps seems to give an accurate-ish number
		return (128 * duration.count() / 8 / 1024);
	}
	else
	{
		// Something's gone wrong...
		return 0;
	}
}

recordingParameters getRecordingParameters(RtAudio::DeviceInfo recordingDevice)
{
	auto logger = spdlog::get("chronicle_log");
	recordingParameters rp;

	(recordingDevice.inputChannels == 1) ? rp.channelCount = 1 : rp.channelCount = 2;
	logger->debug("Input device has {} channels, using {}", recordingDevice.inputChannels, rp.channelCount);

	for (std::vector<unsigned int>::const_iterator i = recordingDevice.sampleRates.begin(); i != recordingDevice.sampleRates.end() + 1; i++)
	{

		logger->debug("Device supports sample rate {}", *i);
		if (*i == 44100)
		{
			logger->debug("Device can accept sample rate of 44.1kHz, using this");
			rp.sampleRate = 44100;
			break;
		}
	}

	if (rp.sampleRate != 44100)
	{
		rp.sampleRate = recordingDevice.preferredSampleRate;
		logger->warn("Could not set sample rate at 44.1 kHz, using preferred sample rate: {}", rp.sampleRate);
	}

	logger->debug("Using sample rate: {}", rp.sampleRate);

	rp.bufferLength = 1024;
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
			boost::filesystem::remove(dirEntry.path());
		}

		dirIter++;
	}
}

void signalHandler(int sigNum)
{
	auto logger = spdlog::get("chronicle_log");
	logger->info("Received signal {}; shutting down...", sigNum);
	stopRecord();
	if (!opts.count("no-term"))
	{
		closeCurses();
	}
	exit(sigNum);
}

void onRtAudioError(RtAudioError::Type type, const string &errorText)
{
	auto logger = spdlog::get("chronicle_log");
	logger->error("Got RtAudio error of type {}: {}", type, errorText);
	signalHandler(1);
}

// void printHelp()
// {
// 	cout << "Usage: " << endl
// 		 << "chronicle [-h | --help]" << endl
// 		 << "chronicle [--licence]" << endl
// 		 << "chronicle [-l | --list-devices ]" << endl
// 		 << "chronicle [-d | --directory OUTPUT_DIRECTORY] [-f | --filename FORMAT] [-i | --input-device DEVICE_ID] " << endl
// 		 << "          [[-a | --max-age MAX_FILE_AGE] | --no-delete] [-s | --audio-format [WAV | OGG]]" << endl
// 		 << endl
// 		 << "Where:" << endl
// 		 << "-h | --help          Prints this help message." << endl
// 		 << "--licence            Prints the licence information for this software and libraries that it uses." << endl
// 		 << "-l | --list-devices  Lists the available input devices with their IDs." << endl
// 		 << "-d | --directory     Sets the directory to save the logged audio to. A trailing slash is not required, but may" << endl
// 		 << "                     be added. On Windows, if using a trailing slash, use a trailing double-slash." << endl
// 		 << "                     Defaults to current directory." << endl
// 		 << "-f | --format        strftime-compatible format to use when naming the audio files." << endl
// 		 << "                     Defaults to %F %H%M%S ." << endl
// 		 << "-i | --input-device  The ID number of the input device to record from. A list of input devices and their ID" << endl
// 		 << "                     numbers can be obtained with `chronicle -l`." << endl
// 		 << "                     If unspecified, the system default audio recording device will be used." << endl
// 		 << "-a | --max-age       Sets the maximum age before audio files will be automatically deleted." << endl
// 		 << "                     Use the format <length><unit>, where unit is < s| m | h | d > for" << endl
// 		 << "                     seconds, minutes, hours and days, respectively." << endl
// 		 << "                     So, to specify 25 hours, pass '-a 25h' ." << endl
// 		 << "                     Defaults to 42 days, in accordance with OFCOM rules." << endl
// 		 << "--no-delete          If passed, Chronicle will not delete old audio files, so they can be manually managed." << endl
// 		 << "                     Incompatible with --max-age." << endl
// 		 << "-s | --audio-format  Sets the audio format to use for the recorded audio files." << endl
// 		 << "                     Acceptable parameters are:" << endl
// 		 << "                     OGG | Ogg Vorbis (.ogg)" << endl
// 		 << "                     WAV | 16-bit PCM WAV (.wav) (default)" << endl
// 		 << "                     MP3 | 320kbps MP3 (.mp3)" << endl
// 		 << "                     FLAC| Free Lossless Audio Codec (.flac)" << endl
// 		 << "--debug              Enables debug logging" << endl
// 		 << endl;
// }