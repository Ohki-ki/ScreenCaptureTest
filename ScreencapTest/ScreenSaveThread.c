#include "ScreenSaveThread.h"


HANDLE CreateScreenSaveThread(SAVEPARAMS* params)
{
    DWORD ThreadID;

    HANDLE aThread = CreateThread(
        NULL,       // default security attributes
        0,          // default stack size
        (LPTHREAD_START_ROUTINE)ScreenSaveJob,
        params,       // no thread function arguments
        0,          // default creation flags
        &ThreadID); // receive thread identifier

    if (aThread == NULL)
    {
        printf("CreateThread error: %d\n", GetLastError());
        return 1;
    }

    return aThread;
}

DWORD WINAPI ScreenSaveJob(LPVOID lpParam)
{
    SAVEPARAMS* params = (SAVEPARAMS*)lpParam;

    int counter = 0;

    while (WaitForSingleObject(params->hKillEvent, 0) != WAIT_OBJECT_0)
    {
        if (WaitForSingleObject(params->hWriteEvent, 0) == WAIT_OBJECT_0
            && WaitForSingleObject(params->hImageMutex, INFINITE) == WAIT_OBJECT_0)
        {
            char filename[20];
            snprintf(filename, 20, ".\\bpp%d_%d.bmp", params->bpp, ++counter);

            printf("Saving file %s\n", filename);

            //todo save
            ScreenSave(params->screenInfo, params->bpp, filename);

            printf("Saved file %s\n", filename);

            ResetEvent(params->hWriteEvent);
            ReleaseMutex(params->hImageMutex);

            if (!SetEvent(params->hSaveEvent))
            {
                printf("SetSaveEvent%d failed (%d)\n", params->bpp, GetLastError());
                return 1;
            }
        }
    }

    printf("killing thread %dbpp\n", params->bpp);
    // Return the exit code to the system. 

    return 0;
}