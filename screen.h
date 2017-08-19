#ifndef __SCREEN_H__
#define __SCREEN_H__

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
//#include <ncurses/ncurses.h>
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#include <string>
#include <cstring>

using namespace std;

void initCurses(string windowTitle);
void closeCurses();
void calculateWindowPositions();

void updateRecordingToPath(string filePath);
void updateAudioDevice(string audioDevice, int sampleRate, int channelCount);
//void updateAudioMeter(float minVal, float maxVal, float currentVal, string volumeLabel);
//void updateAudioMeter(short minVal, short maxVal, short currentVal, string volumeLabel);
void updateAudioMeter(int minVal, int maxVal, int currentVal, string volumeLabel);
void updateHardDriveSpace(long spaceAvailBeforeGB, long fileSizeMB);

#endif //__SCREEN_H__