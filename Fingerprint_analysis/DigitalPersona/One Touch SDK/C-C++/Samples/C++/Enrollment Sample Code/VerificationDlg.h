
// VerificationDlg.h : header file
//

#if !defined(AFX_FTSAMPLEREGDLG_H__3DDDBC15_859E_4dfa_8042_587C71BB0E19__INCLUDED_)
#define AFX_FTSAMPLEREGDLG_H__3DDDBC15_859E_4dfa_8042_587C71BB0E19__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
#include "DPDevClt.h"

/**************************************************************************************************
                                Sample code for FPToolkit
                                Copyright Digital Persona, Inc. 1996-2007

								Tis dialog runs
/*************************************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// CVerificationDlg dialog

class CVerificationDlg : public CDialogImpl<CVerificationDlg> {
public:
	enum { IDD = IDD_SAMPLEVER_DIALOG };
	enum { WMUS_FP_NOTIFY = WM_USER+1 };

	CVerificationDlg();
	~CVerificationDlg();

	void LoadRegTemplate(const DATA_BLOB& rRegTemplate);

	BEGIN_MSG_MAP(CVerificationDlg)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnCancel)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel)

		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WMUS_FP_NOTIFY, OnFpNotify)
	END_MSG_MAP()

private:
	FT_HANDLE      m_fxContext;        // Context for Feature Extraction functions, must be created before calling any of the FT_ functions
	FT_HANDLE      m_mcContext;        // Context for Matching functions, must be created before calling any of the MC_ functions

	BOOL           m_bDoLearning;      // User selects whether to do learning or not via checking checkbox in the dialog
	                                   // NOTE: The Enrollment template must be created with Learning allowed
	                                   // to use this feature during verification.

	HDPOPERATION   m_hOperationVerify; // Handle of the Operation, must be closed at exit.

	RECT           m_rcDrawArea;       // Rectangle with the desired sizes of the fingerprint image to display

	DATA_BLOB      m_RegTemplate;      // BLOB that keeps Enrollment Template. It is received from the outside (from Enrollment dialog)

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy   (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFpNotify  (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCancel           (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void DisplayFingerprintImage(FT_IMAGE_PT pFingerprintImage);
	void Verify(FT_IMAGE_PT pFingerprintImage, int iFingerprintImageSize);

	void AddStatus(LPCWSTR s);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTSAMPLEREGDLG_H__3DDDBC15_859E_4dfa_8042_587C71BB0E19__INCLUDED_)
