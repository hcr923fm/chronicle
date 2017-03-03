
# Changelog - Chronicle

## Unreleased - Will become v0.1.2
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

# Fixed
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