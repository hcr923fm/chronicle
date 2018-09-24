# Chronicle
[![Build Status](https://api.cirrus-ci.com/github/hcr923fm/chronicle.svg)](https://cirrus-ci.com/github/hcr923fm/chronicle)

## What is Chronicle?

Chronicle is an audio logger, designed for radio station use.

At the moment, it records audio from specified input to an audio file, on the hour, every hour. Once the recordings have become outdated, they will be deleted. As such, it is a full rotating logger.

## Getting Chronicle
### Windows x32 and x64
Windows users (both 32 and 64 bit) can simply download the latest compiled binaries from the [Releases](https://github.com/hcr923fm/chronicle/releases/latest) page. Simply extract and run. Hooray!

### Linux
#### With binaries
Linux users can also [download the binaries](https://github.com/hcr923fm/chronicle/releases/latest). However, you will need to manually install the pre-requisites:
* RtAudio, (`librtaudio-dev`)
* libsndfile, which is in most package managers. (`libsndfile1-dev`)
* Boost, which is in most package managers (`libboost-all-dev`).
* ncurses, which is available in most package managers (`libncurses5-dev`).
* LAME encoding library, which is in most package managers (`libmp3lame-dev`).

#### Compiling from scratch
Or, you can compile chronicle from scratch.
You'll also need to download the build toolchain: `build-essential` and `pkg-config`

**Bleeding-edge source**

If you want the latest bleeding-edge version, clone this repository and build from source:

```
git clone http://hcr923fm/chronicle
cd chronicle
./autogen.sh && ./configure && make && make install
```

**Stable builds**
For the latest stable build, [download the source tarball](https://github.com/hcr923fm/chronicle/releases/latest) and run

```
tar xvf chronicle-*
ch chronicle
./configure && make && make install
```

`.deb`s will happen soon!

## Usage:

```
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
    -a | --max-age       Sets the maximum age before audio files will be automatically deleted.
							 Use the format <length><unit>, where unit is < s| m | h | d > for
							 seconds, minutes, hours and days, respectively.
							 So, to specify 25 hours, pass '-a 25h' .
				             Defaults to 42 days, in accordance with OFCOM rules.
    --no-delete          If passed, Chronicle will not delete old audio files, so they can be manually managed.
		                     Incompatible with --max-age.
    -s | --audio-format Sets the audio format to use for the recorded audio files.
                             Acceptable parameters are:
                                 OGG | Ogg Vorbis (.ogg)
                                 WAV | 16-bit PCM WAV (.wav)
                                 MP3 | MP3 VBR, quality 3 (.mp3)
                             Defaults to WAV.
```

## Known Issues:
* Audio devices with more than 2 channels will default to the first two channels. This is perfectly suitable for standard line-in/mic-in audio devices and sound cards where the channels are split up into pairs (which most do), but any sound card that presents multiple channels in a single device will automatically record the first two.
* All audio is resampled to 16bit for conversion to PCM Wave. Although theoretically an issue, this is not likely to affect the vast majority of users.
* Due to the type sizes used in the underlying audio libraries, the audio monitoring graph on the 32-bit version is less reliable at very low levels. This does not affect the quality of the recorded output.

## Buy me a coffee?
If you find Chronicle to be useful, fancy buying me a coffee?

I'm on [Flattr](https://flattr.com/submit/auto?fid=kzr39z&url=http%3A%2F%2Fgithub.com%2Fcalmcl1%2Fchronicle) and [PayPal](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=FXDR44PHGFEDN)!

## Copyright
Copyright 2016-2018 Callum McLean.
Distributed under the MIT Licence. See LICENCE for details.
