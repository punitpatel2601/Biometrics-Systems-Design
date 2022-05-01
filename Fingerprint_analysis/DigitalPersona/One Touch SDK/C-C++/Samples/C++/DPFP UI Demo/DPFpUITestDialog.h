// DPFpUITestDialog.h : Declaration of the CDPFpUITestDialog

#pragma once

#include "resource.h"       // main symbols

#include <atlhost.h>


// CDPFpUITestDialog

class CDPFpUITestDialog : 
	public CAxDialogImpl<CDPFpUITestDialog>
{
public:
	CDPFpUITestDialog();
	~CDPFpUITestDialog();

	enum { IDD = IDD_DPFPUITESTDIALOG };

BEGIN_MSG_MAP(CDPFpUITestDialog)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	COMMAND_HANDLER(IDC_BUTTON_ENROLL, BN_CLICKED, OnBnClickedEnroll)
	COMMAND_HANDLER(IDC_BUTTON_VERIFY, BN_CLICKED, OnBnClickedVerify)
	CHAIN_MSG_MAP(CAxDialogImpl<CDPFpUITestDialog>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CDPFpUITestDialog>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		bHandled = TRUE;
		CheckRadioButton(IDC_RADIO_ENROLL_SUCCESS, IDC_RADIO_ENROLL_ABORT, IDC_RADIO_ENROLL_SUCCESS);
		CheckRadioButton(IDC_RADIO_VERIFY_FAIL, IDC_RADIO_VERIFY_ABORT, IDC_RADIO_VERIFY_FAIL);
		SetDlgItemInt(IDC_EDIT_MASK, 0);
		SetDlgItemInt(IDC_EDIT_MAX_COUNT, 2);
		EnableVerification(FALSE);

		return 1;  // Let the system set the focus
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
	LRESULT OnBnClickedEnroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedVerify(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

protected:
	void EnableVerification(BOOL bEnable){
		::EnableWindow(GetDlgItem(IDC_BUTTON_VERIFY), bEnable);
		::EnableWindow(GetDlgItem(IDC_RADIO_VERIFY_ABORT), bEnable);
		::EnableWindow(GetDlgItem(IDC_RADIO_VERIFY_FAIL), bEnable);
		::EnableWindow(GetDlgItem(IDC_EDIT_CAPTION), bEnable);
		::EnableWindow(GetDlgItem(IDC_EDIT_TEXT), bEnable);
		::EnableWindow(GetDlgItem(IDC_EDIT_BANNER), bEnable);
	}
};


