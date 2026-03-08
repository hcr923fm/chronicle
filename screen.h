#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <curses.h>

#include <string>
#include <cstring>
#include <algorithm>
#include <functional>

void initCurses(std::string windowTitle);
void closeCurses();
void calculateWindowPositions();

void setWindowTitle(std::string windowTitle);
void setBorderAndDividers();
void updateRecordingToPath(std::string filePath);
void updateAudioDevice(std::string audioDevice, int sampleRate, int channelCount);
void updateAudioMeter(int channelNum, float maxVal, float currentVal, std::string volumeLabel);
void updateHardDriveSpace(long spaceAvailBeforeGB, long fileSizeMB);
void onWindowResize();
void setSigIntCallback(std::function<void(int)>);

extern bool NC_UI_IS_ENABLED;

#endif //__SCREEN_H__