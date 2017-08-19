#include "screen.h"

using namespace std;

WINDOW *mainWindow;

int POS_FNAME_X = 2;
int POS_FNAME_Y = 8;

int POS_AUDIO_DEVICE_X;
int POS_SAMPLERATE_X;
int POS_CHANNELCOUNT_X;
int POS_STATUSBAR_Y;

int POS_AUDIOMETER_X;
int POS_AUDIOMETER_Y;
int POS_AUDIOMETER_MAXWIDTH;
int POS_AUDIOMETER_LABEL_X;
int POS_AUDIOMETER_LABEL_Y;

int POS_HDSPACE_X;

void initCurses(string windowTitle)
{
    initscr();
    cbreak(); // To disable the buffering of typed characters by the TTY driver and get a character-at-a-time input
    noecho(); // To suppress the automatic echoing of typed characters
    mainWindow = newwin(LINES, COLS, 0, 0);
    calculateWindowPositions();

    /* Add the border and window title */
    wborder(mainWindow, 0, 0, 0, 0, 0, 0, 0, 0);

    string paddedWindowTitle = " " + windowTitle + " ";
    wmove(mainWindow, 0, (COLS - paddedWindowTitle.length()) / 2);
    wprintw(mainWindow, paddedWindowTitle.c_str());

    /* Draw the horizontal line above the 'status bar' */
    wmove(mainWindow, LINES - 3, 1);
    whline(mainWindow, ACS_HLINE, COLS - 2);

    start_color();
    wrefresh(mainWindow);
}

void closeCurses()
{
    endwin();
}

void calculateWindowPositions()
{
    /* Stuff that goes on the status bar */
    POS_STATUSBAR_Y = LINES - 2;

    POS_AUDIO_DEVICE_X = 1;
    POS_CHANNELCOUNT_X = COLS - 14;
    POS_SAMPLERATE_X = POS_CHANNELCOUNT_X - 14;

    /* Recording audio meter */
    POS_AUDIOMETER_X = POS_FNAME_X;
    POS_AUDIOMETER_Y = POS_FNAME_Y + 2;
    POS_AUDIOMETER_MAXWIDTH = COLS - POS_AUDIOMETER_X - 15;

    POS_AUDIOMETER_LABEL_X = POS_AUDIOMETER_X + 60;
    POS_AUDIOMETER_LABEL_Y = POS_AUDIOMETER_Y;

	POS_HDSPACE_X = POS_AUDIO_DEVICE_X + 30;
}

void updateRecordingToPath(string filePath)
{
    string recordingTo = "Recording to: " + filePath;
    wmove(mainWindow, POS_FNAME_Y, POS_FNAME_X);
    wprintw(mainWindow, recordingTo.c_str());
    wmove(mainWindow, COLS, LINES);
    wrefresh(mainWindow);
}

void updateAudioDevice(string audioDevice, int sampleRate, int channelCount)
{
    /* Draw audio device name */
    wmove(mainWindow, POS_STATUSBAR_Y, POS_AUDIO_DEVICE_X);
    wprintw(mainWindow, audioDevice.c_str());
    wrefresh(mainWindow);

    /* Draw sample rate */
    wmove(mainWindow, POS_STATUSBAR_Y, POS_SAMPLERATE_X);
    string sampleRateStr = to_string(sampleRate) + " Hz";
    wprintw(mainWindow, sampleRateStr.c_str());

    /* Draw channel count */
    wmove(mainWindow, POS_STATUSBAR_Y, POS_CHANNELCOUNT_X);
    string channelCountStr = to_string(channelCount) + " Channels";
    wprintw(mainWindow, channelCountStr.c_str());

    wrefresh(mainWindow);
}

void updateAudioMeter(int minVal, int maxVal, int currentVal, string volumeLabel)
{
    wmove(mainWindow, POS_AUDIOMETER_Y, POS_AUDIOMETER_X);
    wprintw(mainWindow, "| ");

    float range = (maxVal-minVal);
	float proportion = currentVal / range;
	int barWidth = (POS_AUDIOMETER_MAXWIDTH - 3) * proportion;

    whline(mainWindow, '=', barWidth);
    wmove(mainWindow, POS_AUDIOMETER_Y, barWidth);
    whline(mainWindow, ' ', POS_AUDIOMETER_MAXWIDTH - barWidth - 3);

    wmove(mainWindow, POS_AUDIOMETER_Y, POS_AUDIOMETER_MAXWIDTH - 1);
    wprintw(mainWindow, "| ");
    wprintw(mainWindow, volumeLabel.c_str());
	//wprintw(mainWindow, currentVal);
	//wprintw(mainWindow, to_string(proportion).c_str());
	wprintw(mainWindow,"   ");

    wrefresh(mainWindow);
}

void updateHardDriveSpace(long spaceAvailBeforeGB, long fileSizeMB) {
	wmove(mainWindow, POS_STATUSBAR_Y, POS_HDSPACE_X);

	string spaceStr = "Space available: " + to_string(spaceAvailBeforeGB) + " GB; Audio file size: " + to_string(fileSizeMB) + " MB";
	wprintw(mainWindow, spaceStr.c_str());
	wrefresh(mainWindow);
}