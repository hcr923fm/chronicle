#ifndef __SCREEN_H__
#define __SCREEN_H__

#ifdef _WIN32
#include <curses.h>
#else
#include <ncurses.h>
#endif

#include <string>
#include <cstring>
#include <algorithm>

using namespace std;

void initCurses(string windowTitle);
void closeCurses();
void calculateWindowPositions();

void setWindowTitle(string windowTitle);
void setBorderAndDividers();
void updateRecordingToPath(string filePath);
void updateAudioDevice(string audioDevice, int sampleRate, int channelCount);
void updateAudioMeter(int channelNum, float maxVal, float currentVal, string volumeLabel);
void updateHardDriveSpace(long spaceAvailBeforeGB, long fileSizeMB);
void onWindowResize();

extern bool NC_UI_IS_ENABLED;

#endif //__SCREEN_H__