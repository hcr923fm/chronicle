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

#endif //__SCREEN_H__