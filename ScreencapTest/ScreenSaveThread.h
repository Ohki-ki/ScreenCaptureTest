#ifndef _SCREENSAVETHREAD_H_
#define _SCREENSAVETHREAD_H_

#include <Windows.h>
#include <wingdi.h>
#include <stdio.h>

#include "ImageUtil.h"

typedef struct tagSAVEPARAMS
{
    int bpp;
    HANDLE hWriteEvent;
    HANDLE hSaveEvent;
    HANDLE hKillEvent;
    HANDLE hImageMutex;
    SCREENINFO* screenInfo;
} SAVEPARAMS;

HANDLE CreateScreenSaveThread(SAVEPARAMS* saveParams);
DWORD WINAPI ScreenSaveJob(LPVOID lpParam);



#endif // !_SCREENSAVETHREAD_H_
