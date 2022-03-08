#ifndef _SCREENCAPTHREAD_H_
#define _SCREENCAPTHREAD_H_

#include <Windows.h>
#include <wingdi.h>
#include <stdio.h>

#include "ImageUtil.h"

typedef struct tagCAPPARAMS
{
    int count;
    int saveThreadsCount;
    HANDLE* aSaveEvent;
    HANDLE* aWriteEvent;
    SCREENINFO* screenInfo;
} CAPPARAMS;

HANDLE CreateScreenCapThread(CAPPARAMS* params);
DWORD WINAPI ScreenCapJob(LPVOID lpParam);


#endif // !_SCREENCAPTHREAD_H_

