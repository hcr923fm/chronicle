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
* RtAudio, (`librtaudio-dev`) @5.0.0
* libsndfile, which is in most package managers (`libsndfile1-dev`)
* Boost, which is in most package managers (`libboost-all-dev`) @1.65.0+
* ncurses, which is available in most package managers (`libncurses6-dev`) @6.0+
* LAME encoding library, which is in most package managers (`libmp3lame-dev`) @1.3.0+

#### Compiling from scratch
Or, you can compile chronicle from scratch.
You'll also need to download the build toolchain: `build-essential` and `pkg-config`

**Bleeding-edge source**

If you want the latest bleeding-edge version, clone this repository and build from source:

```
git clone http://hcr923fm/chronicle
cd chronicle
mkdir _build && cd _build && cmake .. && cmake --build .
sudo make install
```

**Stable builds**

For the latest stable build, [download the source tarball](https://github.com/hcr923fm/chronicle/releases/latest) and run

```
tar xvf chronicle-*
cd chronicle-*
mkdir _build && cd _build && cmake .. && cmake --build .
sudo make install
```

**NOTE:** This requires CMake version **3.15 or above**, which at the time of writing, is not available in many package managers.

`.deb`s will happen soon!

## Known Issues:
* MP3 audio is recorded at 320kbps CBR, and OGG is VBR. This is due to change in the future, but is currently the case.
* All audio is resampled to 16bit for conversion to PCM Wave. Although theoretically an issue, this is not likely to affect the vast majority of users.
* Due to the type sizes used in the underlying audio libraries, the audio monitoring graph on the 32-bit version is less reliable at very low levels. This does not affect the quality of the recorded output.

## Buy me a coffee?
If you find Chronicle to be useful, fancy buying me a coffee?

I'm on [Patreon](https://patreon.com/calmcl1), or you can find the link at [GitHub](https://github.com/hcr923fm/chronicle) - hit 'Sponsor' at the top!

## Copyright
Copyright 2016-2019 Callum McLean.
Distributed under the MIT Licence. See LICENCE for details.
