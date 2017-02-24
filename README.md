# Chronicle

Chronicle is an audio logger, designed for radio station use.

At the moment, it records audio from the default input to an audio file, on the hour, every hour. Once the recordings have become outdated, they will be deleted. As such, it is a full rotating logger.

**To use:**
```
chronicle [ -h | --help ] [ --licence ] [ -d | --directory OUTPUT_DIRECTORY]  [ -f | --filename FORMAT ]
        [ -a | --max-age MAX_FILE_AGE ] [ -s | --audio-format [ WAV | OGG ] ]
```

Where:
* OUTPUT_DIRECTORY is the directory to save audio files to
* FORMAT is the `strftime`-compatible string to use when saving the audio files
* MAX_FILE_AGE is the amount of time in seconds before an old audio file should be deleted
* Audio format can be specified as WAV for 16-bit PCM Wave files, or OGG for Ogg Vorbis.




Copyright 2016-2017 Callum McLean.
Distributed under the MIT Licence.