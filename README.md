# Chronicle

Chronicle is an audio logger, designed for radio station use.

At the moment, it records audio from the default input to a WAV file, on the hour, every hour.

**To use:**
`chronicle [ -d DIRECTORY ] [ -f FILENAME_FORMAT ]`

Where:
* DIRECTORY is the directory to save audio files to
* FILENAME_FORMAT is the `strftime`-compatible string to use when saving the audio files.


Copyright 2016-2017 Callum McLean.
Distributed under the MIT Licence.