# Chronicle

## What is Chronicle?

Chronicle is an audio logger, designed for radio station use.

At the moment, it records audio from specified input to an audio file, on the hour, every hour. Once the recordings have become outdated, they will be deleted. As such, it is a full rotating logger.


## Usage:

```
chronicle [-h | --help]
chronicle [--licence]
chronicle [-l | --list-devices ]
chronicle [-d | --directory OUTPUT_DIRECTORY] [-f | --filename FORMAT] [-i | --input-device DEVICE_ID] 
          [-a | --max-age MAX_FILE_AGE] [-s | --audio-format [WAV | OGG]]

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
                             Defaults to 3600000 (1000 hours, in accordance with OFCOM rules).
    -s | --audio-format Sets the audio format to use for the recorded audio files.
                             Acceptable parameters are:
                                 OGG | Ogg Vorbis (.ogg)
                                 WAV | 16-bit PCM WAV (.wav)
                             Defaults to WAV.
```

## Buy me a coffee?
If you find chronicle to be useful, fancy buying me a coffee?

I'm on [Flattr](https://flattr.com/submit/auto?fid=kzr39z&url=http%3A%2F%2Fgithub.com%2Fcalmcl1%2Fchronicle)!

## Copyright
Copyright 2016-2017 Callum McLean.
Distributed under the MIT Licence. See LICENCE for details.