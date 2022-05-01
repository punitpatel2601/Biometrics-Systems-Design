// DPFpUITest.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "DPFpUITestDialog.h"

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
#ifdef _DEBUG
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif //_DEBUG
	CDPFpUITestDialog	testDlg;

	testDlg.DoModal();
	return 0;
}

