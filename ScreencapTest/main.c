#include <windows.h>
#include <stdio.h>

#include "ScreenCapThread.h"
#include "ScreenSaveThread.h"
#include "ImageUtil.h"

#define WORKERTHREADS_COUNT	3

int main(int argc, char* argv[])
{
	// Create a mutex with no initial owner

	const char* aSaveEvtNames[] = {
	"SaveEvt1",
	"SaveEvt2",
	"SaveEvt3",
	};

	const char* aWriteEvtNames[] = {
	"WriteEvt1",
	"WriteEvt2",
	"WriteEvt3",
	};

	SCREENINFO screenInfo;
	HANDLE hKillEvent;

	HANDLE hMutex;
	HANDLE hImageMutex;

	HANDLE hScreenCapThread;

	HANDLE aThread[WORKERTHREADS_COUNT];
	HANDLE aSaveEvent[WORKERTHREADS_COUNT];
	HANDLE aWriteEvent[WORKERTHREADS_COUNT];
	SAVEPARAMS aSaveParams[WORKERTHREADS_COUNT];

	hMutex = CreateMutex(
		NULL,              
		TRUE,
		TEXT("Global\\Screencap"));	//global in the system to prevent launching other instances globally, but cause malware detections xD

	if (hMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	else if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		printf("Only one instance of the app can run at the time.");
	}
	else if (argc > 1)
	{		

		printf("Screencap starting...\n");

		hImageMutex = CreateMutex(
			NULL,              // default security attributes
			FALSE,
			TEXT("Image.mutex"));

		if (hMutex == NULL)
		{
			printf("CreateMutex error: %d\n", GetLastError());
			return 1;
		}

		hKillEvent = CreateEvent(
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			TEXT("KillEvent")  // object name
		);

		if (hKillEvent == NULL)
		{
			printf("CreateEvent failed (%d)\n", GetLastError());
			return;
		}

		for (int i = 0, y = BIT_COUNT_8; i < WORKERTHREADS_COUNT; i++, y *= 2)
		{
			aSaveEvent[i] = CreateEvent(
				NULL,               // default security attributes
				TRUE,               // manual-reset event
				FALSE,              // initial state is nonsignaled
				aSaveEvtNames[i]  // object name
			);

			aWriteEvent[i] = CreateEvent(
				NULL,               // default security attributes
				TRUE,               // manual-reset event
				FALSE,              // initial state is nonsignaled
				aWriteEvtNames[i]  // object name
			);
			
			aSaveParams[i].bpp = y;
			aSaveParams[i].hWriteEvent = aWriteEvent[i];
			aSaveParams[i].hKillEvent = hKillEvent;
			aSaveParams[i].hImageMutex = hImageMutex;
			aSaveParams[i].hSaveEvent = aSaveEvent[i];
			aSaveParams[i].screenInfo = &screenInfo;

			aThread[i] = CreateScreenSaveThread(&aSaveParams[i]);
		}



		CAPPARAMS capParams;
		capParams.count = atoi(argv[1]);
		capParams.saveThreadsCount = WORKERTHREADS_COUNT;
		capParams.aWriteEvent = aWriteEvent;
		capParams.aSaveEvent = aSaveEvent;
		capParams.screenInfo = &screenInfo;

		hScreenCapThread = CreateScreenCapThread(&capParams);

		WaitForSingleObject(hScreenCapThread, INFINITE);

		printf("Screencap loop finished, cleaning...\n");

		if (!SetEvent(hKillEvent))
		{
			printf("SetKillEvent failed (%d)\n", GetLastError());
			return 1;
		}

		WaitForMultipleObjects(WORKERTHREADS_COUNT, aThread, TRUE, INFINITE);

		for (int i = 0; i < WORKERTHREADS_COUNT; i++)
		{
			CloseHandle(aWriteEvent[i]);
			CloseHandle(aSaveEvent[i]);
		}

		CloseHandle(hKillEvent);

		ReleaseMutex(hImageMutex);
		CloseHandle(hImageMutex);

		// ugly way of pausing the instance for mutex check 
		// when launching another at the same time
		printf("Done, press [ENTER] key to continue...\n");
		getchar();
	}
	else
	{
		printf("Screencap program usage: \n"
			"screencaptest.exe [count] \n\n"
			"[count] - number of consecutive screen capture operations \n"
		);
	}

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return 0;
}