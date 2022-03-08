#ifndef _IMAGEUTIL_H_
#define _IMAGEUTIL_H_

#include <Windows.h>
#include <wingdi.h>
#include <stdio.h>
#include <malloc.h>

#define	BIT_COUNT_8		8
#define	BIT_COUNT_16	16
#define	BIT_COUNT_32	32

typedef struct tagSCREENINFO
{
	HDC hdcScreen;
	HDC hdcMemDC;
	HBITMAP hbmBitmap;
} SCREENINFO;

//int ScreenCapture(HDC* hdcScreen, HDC*hdcMemDC, HBITMAP* hbmBitmap);
//int ScreenSave(HBITMAP hbmBitmap, HDC hdcScreen, int bitCount, LPCWSTR fileName);
//void ImageCleanup(HBITMAP hbmBitmap, HDC hdcScreen, HDC hdcMemDC);
int ScreenCapture(SCREENINFO* screeninfo);
int ScreenSave(SCREENINFO* screeninfo, int bitCount, const char* fileName);
void ImageCleanup(SCREENINFO* screeninfo);

#endif // !_IMAGEUTIL_H_
