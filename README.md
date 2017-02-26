# Chronicle

## What is Chronicle?

Chronicle is an audio logger, designed for radio station use.

At the moment, it records audio from specified input to an audio file, on the hour, every hour. Once the recordings have become outdated, they will be deleted. As such, it is a full rotating logger.


## Usage:

```
chronicle [ -h | --help ] [ --licence ] [ -d | --directory OUTPUT_DIRECTORY]  [ -f | --filename FORMAT ]
        [ -a | --max-age MAX_FILE_AGE ] [ -s | --audio-format [ WAV | OGG ] ]
```

Where:
* OUTPUT_DIRECTORY is the directory to save audio files to
* FORMAT is the `strftime`-compatible string to use when saving the audio files
* MAX_FILE_AGE is the amount of time in seconds before an old audio file should be deleted
* Audio format can be specified as WAV for 16-bit PCM Wave files, or OGG for Ogg Vorbis.


## Buy me a coffee?
If you find chronicle to be useful, fancy buying me a coffee?

I'm on [Flattr](https://flattr.com/submit/auto?fid=kzr39z&url=http%3A%2F%2Fgithub.com%2Fcalmcl1%2Fchronicle)!

## Copyright
Copyright 2016-2017 Callum McLean.
Distributed under the MIT Licence. See LICENCE for details.