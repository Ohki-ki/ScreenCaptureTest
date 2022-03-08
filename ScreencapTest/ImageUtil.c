#include "ImageUtil.h"


int ScreenCapture(SCREENINFO* screeninfo)
{
    // Retrieve the handle to a display device context for the client 
    // area of the window. 
    screeninfo->hdcScreen = GetDC(NULL);

    int width = GetDeviceCaps(screeninfo->hdcScreen, HORZRES);
    int height = GetDeviceCaps(screeninfo->hdcScreen, VERTRES);

    // Create a compatible DC, which is used in a BitBlt from the window DC.
    screeninfo->hdcMemDC = CreateCompatibleDC(screeninfo->hdcScreen);

    if (!screeninfo->hdcMemDC)
    {
        printf("DC creation error \n");
        return -1;
    }

    // Create a compatible bitmap from the Window DC.
    screeninfo->hbmBitmap = CreateCompatibleBitmap(screeninfo->hdcScreen, width, height);

    if (!screeninfo->hbmBitmap)
    {
        printf("HBM creation error \n");
        return -1;
    }

    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(screeninfo->hdcMemDC, screeninfo->hbmBitmap);

    // Bit block transfer into our compatible memory DC.
    if (!BitBlt(screeninfo->hdcMemDC,
        0, 0,
        width, height,
        screeninfo->hdcScreen,
        0, 0,
        SRCCOPY))
    {
        printf("Bit block transfer error \n");
        return -1;
    }

    return 0;
}


int ScreenSave(SCREENINFO* screeninfo, int bitCount, const char* fileName)
{
    BITMAP bmpScreen;
    DWORD dwBytesWritten = 0;
    DWORD dwSizeofDIB = 0;
    HANDLE hFile = NULL;
    char* lpbitmap = NULL;
    HANDLE hDIB = NULL;
    DWORD dwBmpSize = 0;   

    DWORD dwBmiSize = 0;

    // Get the BITMAP from the HBITMAP.
    GetObject(screeninfo->hbmBitmap, sizeof(BITMAP), &bmpScreen);

    BITMAPFILEHEADER   bmfHeader;

    // if we want to use 8bpp we have to include palette info
    if (bitCount == BIT_COUNT_8)
        dwBmiSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
    else
        dwBmiSize = sizeof(BITMAPINFOHEADER);

    BITMAPINFO* pBI = malloc(dwBmiSize);

    pBI->bmiHeader.biSize = sizeof(pBI->bmiHeader);
    pBI->bmiHeader.biWidth = bmpScreen.bmWidth;
    pBI->bmiHeader.biHeight = bmpScreen.bmHeight;
    pBI->bmiHeader.biPlanes = 1;
    pBI->bmiHeader.biBitCount = bitCount;
    pBI->bmiHeader.biCompression = BI_RGB;
    pBI->bmiHeader.biSizeImage = 0;
    pBI->bmiHeader.biXPelsPerMeter = 0;
    pBI->bmiHeader.biYPelsPerMeter = 0;
    pBI->bmiHeader.biClrUsed = 0;
    pBI->bmiHeader.biClrImportant = 0;

    //if (bitCount != BIT_COUNT_8) pBI->bmiColors == NULL;

    //filling palette info for 8bpp only
    for (int i = 0; bitCount == BIT_COUNT_8 && i < 256; i++)
    {
        pBI->bmiColors[i].rgbRed = i;
        pBI->bmiColors[i].rgbGreen = i;
        pBI->bmiColors[i].rgbBlue = i;
        pBI->bmiColors[i].rgbReserved = 0;
    }



    dwBmpSize = ((bmpScreen.bmWidth * pBI->bmiHeader.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char*)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap, and copies them into a buffer 
    // that's pointed to by lpbitmap.
    GetDIBits(screeninfo->hdcScreen, screeninfo->hbmBitmap, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        pBI, DIB_RGB_COLORS);
    
    // A file is created, this is where we will save the screen capture.
    hFile = CreateFileA(fileName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size.
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + dwBmiSize;

    // Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + dwBmiSize;

    // Size of the file.
    bmfHeader.bfSize = dwSizeofDIB;

    // bfType must always be BM for Bitmaps.
    bmfHeader.bfType = 0x4D42; // BM.

    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)pBI, dwBmiSize, &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    // Unlock and Free the DIB from the heap.
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    free(pBI);

    // Close the handle for the file that was created.
    CloseHandle(hFile);

    return 0;
}


void ImageCleanup(SCREENINFO* screeninfo)
{
    DeleteObject(screeninfo->hbmBitmap);
    DeleteObject(screeninfo->hdcMemDC);
    ReleaseDC(NULL, screeninfo->hdcScreen);
}