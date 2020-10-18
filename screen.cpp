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

string windowTitleCache;
string audioDeviceCache;
string recordingPathCache;
long HDSpaceAvailableCache;
long fileSizeCache;
int sampleRateCache;
int channelCountCache;

bool NC_UI_IS_ENABLED;

void initCurses(string windowTitle)
{
    if (NC_UI_IS_ENABLED)
    {
        initscr();
        cbreak();    // To disable the buffering of typed characters by the TTY driver and get a character-at-a-time input
        noecho();    // To suppress the automatic echoing of typed characters
        curs_set(0); // Hide the cursor
        mainWindow = newwin(LINES, COLS, 0, 0);
        calculateWindowPositions();

        /* Add the border and window title */
        setBorderAndDividers();
        setWindowTitle(windowTitle);

        /* Draw the horizontal line above the 'status bar' */

        start_color();
        wrefresh(mainWindow);
    }
}

void closeCurses()
{
    if (NC_UI_IS_ENABLED)
    {
        endwin();
    }
}

void calculateWindowPositions()
{
    if (NC_UI_IS_ENABLED)
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

        POS_AUDIOMETER_LABEL_X = POS_AUDIOMETER_X + POS_AUDIOMETER_MAXWIDTH + 3;
        POS_AUDIOMETER_LABEL_Y = POS_AUDIOMETER_Y;

        POS_HDSPACE_X = POS_AUDIO_DEVICE_X + 30;
    }
}

void setWindowTitle(string windowTitle)
{
    if (NC_UI_IS_ENABLED)
    {
        windowTitleCache = windowTitle;

        string paddedWindowTitle = " " + windowTitleCache + " ";
        wmove(mainWindow, 0, (COLS - paddedWindowTitle.length()) / 2);
        wprintw(mainWindow, paddedWindowTitle.c_str());
    }
}

void setBorderAndDividers()
{
    if (NC_UI_IS_ENABLED)
    {
        wborder(mainWindow, 0, 0, 0, 0, 0, 0, 0, 0);
        wmove(mainWindow, LINES - 3, 1);
        whline(mainWindow, ACS_HLINE, COLS - 2);
        wrefresh(mainWindow);
    }
}

void updateRecordingToPath(string filePath)
{
    if (NC_UI_IS_ENABLED)
    {
        recordingPathCache = filePath;
        string recordingTo = "Recording to: " + filePath;
        wmove(mainWindow, POS_FNAME_Y, POS_FNAME_X);
        wprintw(mainWindow, recordingTo.c_str());
        wmove(mainWindow, COLS, LINES);
        wrefresh(mainWindow);
    }
}

void updateAudioDevice(string audioDevice, int sampleRate, int channelCount)
{
    if (NC_UI_IS_ENABLED)
    {
        /* Draw audio device name */
        wmove(mainWindow, POS_STATUSBAR_Y, POS_AUDIO_DEVICE_X);
        audioDeviceCache = audioDevice;
        wprintw(mainWindow, audioDeviceCache.c_str());
        //wrefresh(mainWindow);

        /* Draw sample rate */
        wmove(mainWindow, POS_STATUSBAR_Y, POS_SAMPLERATE_X);
        sampleRateCache = sampleRate;
        string sampleRateStr = sampleRateCache + " Hz";

        wprintw(mainWindow, sampleRateStr.c_str());

        /* Draw channel count */
        wmove(mainWindow, POS_STATUSBAR_Y, POS_CHANNELCOUNT_X);
        channelCountCache = channelCount;
        string channelCountStr = to_string(channelCountCache) + " Channels";
        wprintw(mainWindow, channelCountStr.c_str());

        /* Draw channel meter start and end bars */
        for (int channelNum = 0; channelNum < channelCount; channelNum++)
        {
            wmove(mainWindow, POS_AUDIOMETER_Y + channelNum, POS_AUDIOMETER_X);
            wprintw(mainWindow, "| ");
            wmove(mainWindow, POS_AUDIOMETER_Y + channelNum, POS_AUDIOMETER_MAXWIDTH - 2);
            wprintw(mainWindow, "| ");
        }

        wrefresh(mainWindow);
    }
}

void updateAudioMeter(int channelNum, float maxVal, float currentVal, string volumeLabel)
{
    if (NC_UI_IS_ENABLED)
    {
        int potentialBarWidth = (POS_AUDIOMETER_MAXWIDTH - 3) * (currentVal / maxVal);
        int clippedBarWidth = min(POS_AUDIOMETER_MAXWIDTH - 3, max(2, potentialBarWidth));

        mvwhline(mainWindow, POS_AUDIOMETER_Y + channelNum, POS_AUDIOMETER_X + 1, '=', clippedBarWidth);
        mvwhline(mainWindow, POS_AUDIOMETER_Y + channelNum, POS_AUDIO_DEVICE_X + clippedBarWidth, ' ', POS_AUDIOMETER_MAXWIDTH - clippedBarWidth - 3);

        wmove(mainWindow, POS_AUDIOMETER_Y + channelNum, POS_AUDIOMETER_MAXWIDTH);
        wprintw(mainWindow, volumeLabel.c_str());

        wrefresh(mainWindow);
    }
}

void updateHardDriveSpace(long spaceAvailBeforeGB, long fileSizeMB)
{
    if (NC_UI_IS_ENABLED)
    {
        wmove(mainWindow, POS_STATUSBAR_Y, POS_HDSPACE_X);

        HDSpaceAvailableCache = spaceAvailBeforeGB;
        fileSizeCache = fileSizeMB;
        string spaceStr = "Space available: " + to_string(HDSpaceAvailableCache) + " GB; Audio file size: " + to_string(fileSizeCache) + " MB";
        wprintw(mainWindow, spaceStr.c_str());
        wrefresh(mainWindow);
    }
}

void onWindowResize()
{
    if (NC_UI_IS_ENABLED)
    {
        int x;
        int y;

        getmaxyx(mainWindow, y, x);
#ifndef _WIN32
        resizeterm(x, y);
#else
        resize_term(x, y);
#endif
        endwin();
        wrefresh(mainWindow);

        initscr();
        cbreak();    // To disable the buffering of typed characters by the TTY driver and get a character-at-a-time input
        noecho();    // To suppress the automatic echoing of typed characters
        curs_set(0); // Hide the cursor

        calculateWindowPositions();
        setWindowTitle(windowTitleCache);
        setBorderAndDividers();
        updateAudioDevice(audioDeviceCache, sampleRateCache, channelCountCache);
        updateHardDriveSpace(HDSpaceAvailableCache, fileSizeCache);
        updateRecordingToPath(recordingPathCache);
    }
}