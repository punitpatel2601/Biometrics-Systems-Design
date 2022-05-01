// MainDlg.h
#ifndef ENROLLMENT_MAIN_DIALOG
#define ENROLLMENT_MAIN_DIALOG

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "EnrollmentDlg.h"
#include "VerificationDlg.h"
#include "Utilities.h"

class CMainDlg : public CDialogImpl<CMainDlg> {
public:
	enum { IDD = IDD_MAIN };

	CMainDlg() : CDialogImpl<CMainDlg>()
	{
		::ZeroMemory(&m_RegTemplate, sizeof(m_RegTemplate));
	}

	CMainDlg::~CMainDlg() {
		delete [] m_RegTemplate.pbData;
		m_RegTemplate.cbData = 0;
		m_RegTemplate.pbData = NULL;
	}

	BEGIN_MSG_MAP(CMainDlg)
		COMMAND_HANDLER(IDC_FP_REGISTRATION, BN_CLICKED, OnBnClickedEnrollment)
		COMMAND_HANDLER(IDC_FP_VERIFICATION, BN_CLICKED, OnBnClickedVerification)
		COMMAND_HANDLER(IDC_SAVE_TEMPLATE, BN_CLICKED, OnBnClickedSaveTemplate)
		COMMAND_HANDLER(IDC_READ_TEMPLATE, BN_CLICKED, OnBnClickedReadTemplate)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnCancel);
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel);
	END_MSG_MAP()

	// This function runs Enrollment dialog. It starts all necessary operations in OnInitDialog
	// and does cleanup in OnDestroy.
	LRESULT OnBnClickedEnrollment(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
		CEnrollmentDlg dlg;
		dlg.DoModal();
		dlg.GetRegTemplate(m_RegTemplate); // Copy the Enrollment template for use in the Verification dialog.

		::ShowWindow(GetDlgItem(IDC_TEMPLATE_LOADED), m_RegTemplate.cbData && m_RegTemplate.pbData ? SW_SHOW : SW_HIDE);

		return 0;
	}

	LRESULT OnBnClickedVerification(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
		if (m_RegTemplate.cbData == 0 || m_RegTemplate.pbData == NULL) {
			MessageBox(L"Before attempting fingerprint verification, you must either select \"Fingerprint Enrollment\" to create a Fingerprint Enrollment Template, or read a previously saved template using \"Read Fingerprint Enrollment Template\".", L"Fingerprint Verification", MB_OK | MB_ICONINFORMATION);
		}
		else {
			CVerificationDlg dlg;
			dlg.LoadRegTemplate(m_RegTemplate); // To do verification, we need a Enrollment Template to compare with.
			                                    // In this case we simply get the one we just produced when registering fingerprint.
			                                    // In real life it would be loaded from a database, file, etc.
			dlg.DoModal();
		}
		return 0;
	}

	// This function simply saves BLOB of the Enrollment template created previously in to
	// user-specified file.
	LRESULT OnBnClickedSaveTemplate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
		if (m_RegTemplate.cbData == 0 || m_RegTemplate.pbData == NULL) {
			MessageBox(L"Before attempting to save a template, select \"Fingerprint Enrollment\" to create a Fingerprint Enrollment Template or select \"Read Fingerprint Enrollment Template\" to read a template.", L"Save Fingerprint Template", MB_OK | MB_ICONINFORMATION);
			return 0;
		}

		HRESULT hr = S_OK;
		try {
			WCHAR FileNameBuffer[MAX_PATH+1] = {0};

			OPENFILENAME ofn = {0};
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = m_hWnd;
			ofn.hInstance = _Module.GetResourceInstance();
			ofn.lpstrFilter = L"Fingerprint Templates\0*.fpt";
			ofn.lpstrFile = FileNameBuffer;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
			ofn.lpstrDefExt = L".fpt";

			if (GetSaveFileName(&ofn)) {
				HANDLE hFile = ::CreateFile(FileNameBuffer, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				if (INVALID_HANDLE_VALUE != hFile) {
					DWORD dwWritten = 0;
					BOOL bOK = ::WriteFile(hFile, m_RegTemplate.pbData, m_RegTemplate.cbData, &dwWritten, 0);
					::CloseHandle(hFile);
					if (!bOK)
						_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));
				}
				else
					_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));
			}
		}
		catch(_com_error& E) {
			hr = E.Error();
		}
		catch(...) {
			hr = E_UNEXPECTED;
		}

		if (FAILED(hr)) {
			ReportError(m_hWnd, hr);
		}
		return 0;
	}

	// This function simply reads BLOB of the Enrollment template from the file 
	// where it was saved previously.
	LRESULT OnBnClickedReadTemplate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
		HRESULT hr = S_OK;
		try {
			WCHAR FileNameBuffer[MAX_PATH+1] = {0};

			OPENFILENAME ofn = {0};
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = m_hWnd;
			ofn.hInstance = _Module.GetResourceInstance();
			ofn.lpstrFilter = L"Fingerprint Templates(*.fpt)\0*.fpt";
			ofn.lpstrFile = FileNameBuffer;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
			ofn.lpstrDefExt = L".fpt";

			if (GetOpenFileName(&ofn)) {
				HANDLE hFile = ::CreateFile(FileNameBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (INVALID_HANDLE_VALUE != hFile) {
					DWORD dwWritten = 0;

					DWORD dwSize = GetFileSize(hFile, NULL);
					if (dwSize) {
						BYTE* buffer = new BYTE[dwSize];
						if (!buffer) {
							::CloseHandle(hFile);
							_com_issue_error(E_OUTOFMEMORY);
						}

						DWORD dwNumRead = 0;
						if (::ReadFile(hFile, buffer, dwSize, &dwNumRead, 0)) {
							delete [] m_RegTemplate.pbData;

							m_RegTemplate.pbData = buffer;
							m_RegTemplate.cbData = dwNumRead;
						}
						else {
							delete [] buffer;
							::CloseHandle(hFile);
							_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));
						}
						::CloseHandle(hFile);
					}
					else {
						::CloseHandle(hFile);
						_com_issue_error(E_FAIL);
					}
				}
				else
					_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));
			}

			::ShowWindow(GetDlgItem(IDC_TEMPLATE_LOADED), m_RegTemplate.cbData && m_RegTemplate.pbData ? SW_SHOW : SW_HIDE);
		}
		catch(_com_error& E) {
			hr = E.Error();
		}
		catch(...) {
			hr = E_UNEXPECTED;
		}

		if (FAILED(hr)) {
			ReportError(m_hWnd, hr);
		}
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
		EndDialog(wID);
		return 0;
	}

private:
	DATA_BLOB  m_RegTemplate;   // BLOB that keeps Enrollment Template. It is used to pass it from Enrollment to Verification and also for saving/reading from file.
};

#endif //ENROLLMENT_MAIN_DIALOG
