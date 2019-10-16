
# Changelog - Chronicle

## Will be 1.0.3
### Added
* Added support for using chronicle without a terminal
* Added support for FLAC (currently VBR only)
* Added support to specify samplerate
* Added support to select channels on multi-channel devices
* Added support to resize the window reasonably well

### Fixed
* Fixed issue where using `-l` with no available audio devices would throw an error

### Changed
* Switched to CMake-based build system, removed most platform-dependent CI scripting

## v1.0.2
### Fixed
* Fixed issue where help string would print incorrectly
* Fixed issue where MP3 encoding was not listed in the help string
* Fixed issue where VBR was being used instead of CBR for MP3 recording, leading to duration inaccuracies

## v1.0.1
### Added
* Added per-channel level monitoring (#38)
* In debug mode, list audio device caps on init

### Fixed
* Fixed inaccurate file size calculation when using OGG output
* Fixed inaccurate file size calculation when using MP3 output
* Fixed inaccurate audio level monitoring at low levels

## v1.0.0
### Added
* Hard drive usage monitor (#22)
* MP3 recording (#20)
* Proper autotools installation

## v0.1.2
### Added
* Added Changelog - you should always [Keep a Changelog](http:////keepachangelog.com)
* Added ability to list the available audio devices to use with Chronicle (`-l | --list-devices`) (#9)
* Added proper command-line switches
* Added licence details
* Added ability to supply custom `strftime`-style output filenames
* Added README.md
* Added ability to automatically remove audio files older than a given age (#5, #7)
* Added incoming audio level meter
* Added support for OGG files as well as WAV (#2)
* Chronicle now validates and sanitizes cmd-line arguments before trying to record (#13)
* Added full `ncurses` interface (#16)
* Added `--no-delete` option (#22)
* Added ability to specify `--max-age` in seconds, minutes, hours or days (#12)

### Fixed
* Fixed issue where SIGKILL/SIGINT caused unreadable audio files (#1)
* Fixed issue where no directory supplied would cause Chronicle to crash
* Fixed issue where Chronicle would not compile on Unix due to libsndfile requiring `cinttypes`
* Fixed issue where Chronicle would not compile on Unix due to using `localtime_s`
* Fixed issue where Chronicle would not compile on Unix due to using SIGBEAK
* Fixed filepath handling on Windows and Unix
* Fixed issue where RtAudio would try to record into a directory that doesn't exist (#8)
* Fixed release bundling - all .dlls are now bundled with the .exe (#10)
* Prettified help and licence strings (#3)
* Fixed unsafe `atoi` use, now using `strtoul` (#14)
* Fixed issue where audio metering differed between 32-bit and 64-bit devices
* Simplified install process on Linux
* Fixed issue where chronicle would continue trying to record to unopenable files
* Added loggin (fixes #31)