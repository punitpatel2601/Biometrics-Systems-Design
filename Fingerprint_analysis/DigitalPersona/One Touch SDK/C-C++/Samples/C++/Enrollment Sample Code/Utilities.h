// Utilities.h
#ifndef REISTRATION_UTILITIES
#define REISTRATION_UTILITIES

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>

void ReportError(HWND hWnd, HRESULT hr, char* s="");

void FillBitmapInfoHeader(PBITMAPINFOHEADER pbi, LONG width, LONG height, DWORD dwImgSize, WORD bitCount, DWORD nColors);
void FillColorTable(PALETTEENTRY* pEntries, int nEntries);
HPALETTE CreatePalette(PALETTEENTRY* pEntries, int nEntries);

std::wstring ImageInfo(ULONG uImageQuality);

inline int WIDTHBYTES(int bits) { return (bits + 31) / 32 * 4; }

#endif //REISTRATION_UTILITIES
