#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <ncurses/ncurses.h>
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

#endif //__SCREEN_H__