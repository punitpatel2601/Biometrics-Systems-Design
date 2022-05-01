// EnrollmentDlg.h : header file
//

#if !defined(AFX_FTSAMPLEREGDLG_H__FC844F46_1C95_11D2_87EB_00A0C9C9343F__INCLUDED_)
#define AFX_FTSAMPLEREGDLG_H__FC844F46_1C95_11D2_87EB_00A0C9C9343F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
#include "DPDevClt.h"

/**************************************************************************************************
                                Sample code for FPToolkit
                                Copyright Digital Persona, Inc. 1996-2007
/*************************************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// CEnrollmentDlg dialog

class CEnrollmentDlg : public CDialogImpl<CEnrollmentDlg> {
public:
	enum { IDD = IDD_SAMPLEREG_DIALOG };
	enum { WMUS_FP_NOTIFY = WM_USER+1 };

	CEnrollmentDlg();
	~CEnrollmentDlg();

	void GetRegTemplate(DATA_BLOB& rTemplate) const;

	BEGIN_MSG_MAP(CEnrollmentDlg)
		//COMMAND_HANDLER(IDB_DISABLE_DEFAULT_REG, BN_CLICKED, OnModeCheckBox)
		//COMMAND_HANDLER(IDB_ENABLE_XTF_REG, BN_CLICKED, OnModeCheckBox)
		//COMMAND_HANDLER(IDB_ENABLE_LEARNING, BN_CLICKED, OnModeCheckBox)

		COMMAND_HANDLER(IDOK, BN_CLICKED, OnCancel)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel)

		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WMUS_FP_NOTIFY, OnFpNotify)
	END_MSG_MAP()

private:
	FT_HANDLE      m_fxContext;              // Context for Feature Extraction functions, must be created before calling any of the FT_ functions
	FT_HANDLE      m_mcContext;              // Context for Matching functions, must be created before calling any of the MC_ functions

	HDPOPERATION   m_hOperationEnroll;     // Handle of the Operation, must be closed at exit.

	RECT           m_rcDrawArea;             // Rectangle with the desired sizes of the fingerprint image to display

	int            m_NumberOfPreRegFeatures; // Number of the pre-Enrollment fingerprint templates needed to create one Enrollment template.
	FT_BYTE**      m_TemplateArray;          // Array that keeps pre-Enrollment fingerprint templates. It is allocated with the size equal to number of pre-Enrollment templetes to create a Enrollment template (usually 4)
	int            m_nRegFingerprint;        // Pre-Enrollment number, index in the above array.
	FT_REG_OPTIONS m_mcRegOptions;           // Enrollment options. Collected from check boxes of the dialog.

	DATA_BLOB      m_RegTemplate;            // BLOB that keeps Enrollment Template.

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy   (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFpNotify  (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCancel         (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//LRESULT OnModeCheckBox (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	void DisplayFingerprintImage(FT_IMAGE_PT pFingerprintImage);
	void AddToEnroll(FT_IMAGE_PT pFingerprintImage, int iFingerprintImageSize);

	void AddStatus(LPCWSTR s);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTSAMPLEREGDLG_H__FC844F46_1C95_11D2_87EB_00A0C9C9343F__INCLUDED_)
