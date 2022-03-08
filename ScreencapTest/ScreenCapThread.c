#include "ScreenCapThread.h"       

HANDLE CreateScreenCapThread(CAPPARAMS* params)
{    
    DWORD ThreadID;

    HANDLE aThread = CreateThread(
        NULL,       // default security attributes
        0,          // default stack size
        (LPTHREAD_START_ROUTINE)ScreenCapJob,
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

DWORD WINAPI ScreenCapJob(LPVOID lpParam)
{
    CAPPARAMS* params = (CAPPARAMS*)lpParam;
    
    for (int i = 0; i < params->count; i++)
    {
        printf("screen %d capturing\n", i);
        ScreenCapture(params->screenInfo);
        printf("screen captured\n");

        SetEvent(params->aWriteEvent[0]);
        SetEvent(params->aWriteEvent[1]);
        SetEvent(params->aWriteEvent[2]);

        WaitForMultipleObjects(params->saveThreadsCount, params->aSaveEvent, TRUE, INFINITE);

        ResetEvent(params->aSaveEvent[0]);
        ResetEvent(params->aSaveEvent[1]);
        ResetEvent(params->aSaveEvent[2]);

        ImageCleanup(params->screenInfo);

        printf("screen saved by all threads\n");
    }
    

    return 0;
}