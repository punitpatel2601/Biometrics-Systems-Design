// Utilities.cpp

#include "stdafx.h"
#include "Utilities.h"
#include "DPDevClt.h"
#include "dpDefs.h"
#include "dpRCodes.h"
#include "dpFtrEx.h"

///////////////////////////////////////////////////////////////////////////////

void ReportError(HWND hWnd, HRESULT hr, char* s) {
	if (FAILED(hr)) {
		WCHAR buffer[MAX_PATH+1] = {0};
		_com_error E(hr);
		_snwprintf(buffer, MAX_PATH, L"ERROR: %s(0x%X)\n%s", E.ErrorMessage(), E.Error(), s);
		MessageBox(hWnd, buffer, L"ERROR", MB_OK);
	}
}

/////////////////////////////////////////////////////////////////////////////
// dib stuff

void FillBitmapInfoHeader(PBITMAPINFOHEADER pbi, LONG width, LONG height, DWORD dwImgSize, WORD bitCount, DWORD nColors)
{
	pbi->biSize          = sizeof(BITMAPINFOHEADER);
	pbi->biWidth         = width;
	pbi->biHeight        = height;
	pbi->biPlanes        = 1;
	pbi->biBitCount      = bitCount;
	pbi->biCompression   = BI_RGB;
	pbi->biSizeImage     = dwImgSize;
	pbi->biXPelsPerMeter = 0;
	pbi->biYPelsPerMeter = 0;
	pbi->biClrUsed       = nColors;
	pbi->biClrImportant  = 0;
}

void FillColorTable(PALETTEENTRY* pEntries, int nEntries)
{
	for (int i = 0; i < nEntries; i++)  {	// BYTE k = (BYTE)i<<3;
		pEntries[i].peRed   = i;	// Gray Scale !
		pEntries[i].peGreen = i;
		pEntries[i].peBlue  = i;
		pEntries[i].peFlags = 0;
	}
}

HPALETTE CreatePalette(PALETTEENTRY* pEntries, int nEntries)
{
	LOGPALETTE* pal = (LOGPALETTE*) new BYTE[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * nEntries];
	if (!pal)
		return NULL;

	pal->palVersion = 0x300;
	pal->palNumEntries = nEntries;

	for (int i = 0; i < nEntries; i++)
		pal->palPalEntry[i] = pEntries[i];

	HPALETTE hPal = ::CreatePalette(pal);

	delete[] pal;
	pal = NULL;

	return hPal;
}

LRESULT OnFeaturesInfo(WPARAM wParam, LPARAM lParam)
{
/*
    GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("FT_FEATURES_INFO");
    GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("FT_FEATURES_INFO");
    CWnd* pWnd = GetDlgItem(IDC_STATIC_DRAWAREA);
    CDC* pDC = pWnd->GetDC();

    switch(lParam)
    {
    case FT_GOOD_FTR:
        GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("FT_GOOD_FTR");
        TRACE("FT_GOOD_FTR\n");
        m_pGray8Dib->UsePalette(pDC, true);
        m_pGray8Dib->Draw(pDC, 0, 0, g_FPBuffer);

        if(!g_bIsVerifying)
        {
            CString s;
							if(g_nFingerCount != 2)
									s.Format("%d fingers left", 3 - g_nFingerCount);
							else
									s.Format("%d finger left", 3 - g_nFingerCount);
            GetDlgItem(IDC_EDIT_PROMPT)->SetWindowText(s);
            g_nFingerCount++;
        }
        break;
        
    case FT_NOT_ENOUGH_FTR:
        m_pGray8Dib->UsePalette(pDC, true);
        m_pGray8Dib->Draw(pDC, 0, 0, g_FPBuffer);
        GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("FT_NOT_ENOUGH_FTR");
        TRACE("FT_NOT_ENOUGH_FTR\n");
        break;

    case FT_NO_CENTRAL_REGION:
        m_pGray8Dib->UsePalette(pDC, true);
        m_pGray8Dib->Draw(pDC, 0, 0, g_FPBuffer);
        GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("FT_NO_CENTRAL_REGION");
        TRACE("FT_NO_CENTRAL_REGION\n");
        break;

    case FT_UNKNOWN_FTR_QUALITY:
        m_pGray8Dib->UsePalette(pDC, true);
        m_pGray8Dib->Draw(pDC, 0, 0, g_FPBuffer);
        GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("FT_UNKNOWN_FTR_QUALITY");
        TRACE("FT_UNKNOWN_FTR_QUALITY\n");
        break;
        
		case FT_AREA_TOO_SMALL:
        m_pGray8Dib->UsePalette(pDC, true);
        m_pGray8Dib->Draw(pDC, 0, 0, g_FPBuffer);
        GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("FT_AREA_TOO_SMALL");
        TRACE("FT_AREA_TOO_SMALL\n");
        break;

    default:
        GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("Unknown lParam");
        break;
    }
    ReleaseDC(pDC);
*/
	return 0;
}
