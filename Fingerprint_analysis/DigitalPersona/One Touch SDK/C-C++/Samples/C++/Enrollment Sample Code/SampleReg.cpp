// FTSampleReg.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MainDlg.h"
#include "Utilities.h"
#include "dpFtrEx.h"
#include "dpMatch.h"

CComModule _Module;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	HRESULT hr = S_OK;

	_Module.Init(NULL, hInstance);

	try {
		_com_test_error(DPFPInit()); // Always initialize DPFPApi

		if (FT_OK == FX_init())      // Always initialize Gold SDK Feature Extraction
		{
			if (FT_OK == MC_init())  // Always initialize Gold SDK Matching
			{
				CMainDlg dlg;        // The rest of the work is done in the dialog
				dlg.DoModal();

				MC_terminate();      // All MC_init  must be matched with MC_terminate to free up the resources
			}
			else {
				::MessageBox(0, L"Cannot initialize Matching.", L"ERROR", MB_OK | MB_ICONSTOP);
			}

			FX_terminate();             // All FX_init  must be matched with FX_terminate to free up the resources
		}
		else {
			::MessageBox(0, L"Cannot initialize Feature Extraction.", L"ERROR", MB_OK | MB_ICONSTOP);
		}

		DPFPTerm();                     // All DPFPInit must be matched with DPFPTerm     to free up the resources
	}
	catch(_com_error& E) {
		hr = E.Error();
	}
	catch(...) {
		hr = E_UNEXPECTED;
	}

	ReportError(0, hr);

 	_Module.Term();

	return 0;
}
