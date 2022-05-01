// VerificationDlg.cpp : implementation file
//
#include "stdafx.h"
#include "VerificationDlg.h"
#include "Utilities.h"
#include "dpFtrEx.h"
#include "dpMatch.h"

/**************************************************************************************************
                                Sample code for Fingerprint Verification
                                Copyright Digital Persona, Inc. 1996-2007
/*************************************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CVerificationDlg dialog

CVerificationDlg::CVerificationDlg() : 
	CDialogImpl<CVerificationDlg>(), 
	m_bDoLearning(FT_FALSE),
	m_hOperationVerify(0),
	m_fxContext(0),
	m_mcContext(0)
{
	::ZeroMemory(&m_rcDrawArea, sizeof(m_rcDrawArea));
	::ZeroMemory(&m_RegTemplate, sizeof(m_RegTemplate));
}

CVerificationDlg::~CVerificationDlg() {
	delete [] m_RegTemplate.pbData;
	m_RegTemplate.cbData = 0;
	m_RegTemplate.pbData = NULL;
}

void CVerificationDlg::LoadRegTemplate(const DATA_BLOB& rRegTemplate) {
	// Delete the old stuff that may be in the template.
	delete [] m_RegTemplate.pbData;
	m_RegTemplate.pbData = NULL;
	m_RegTemplate.cbData = 0;

	// Copy Enrollment template data into member of this class
	m_RegTemplate.pbData = new BYTE[rRegTemplate.cbData];
	if (!m_RegTemplate.pbData) _com_issue_error(E_OUTOFMEMORY);
	::CopyMemory(m_RegTemplate.pbData, rRegTemplate.pbData, rRegTemplate.cbData);
	m_RegTemplate.cbData = rRegTemplate.cbData;
}

void CVerificationDlg::AddStatus(LPCWSTR s) {
	int lIdx = SendDlgItemMessage(IDC_STATUS, LB_ADDSTRING, 0, (LPARAM)s);
	SendDlgItemMessage(IDC_STATUS, LB_SETTOPINDEX, lIdx, 0);
}

LRESULT CVerificationDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = S_OK;
	try {
		FT_RETCODE rc = FT_OK;

		// Create Context for Feature Extraction
		if (FT_OK != (rc = FX_createContext(&m_fxContext))) {
			MessageBox(L"Cannot create Feature Extraction Context.", L"Fingerprint Verification", MB_OK | MB_ICONSTOP);
			EndDialog(IDCANCEL);
			return TRUE;  // return TRUE  unless you set the focus to a control
		}

		// Create Context for Matching
		if (FT_OK != (rc = MC_createContext(&m_mcContext))) {
			MessageBox(L"Cannot create Matching Context.", L"Fingerprint Verification", MB_OK | MB_ICONSTOP);
			EndDialog(IDCANCEL);
			return TRUE;  // return TRUE  unless you set the focus to a control
		}

		::GetWindowRect(GetDlgItem(IDC_STATIC_DRAWAREA_SIZE), &m_rcDrawArea);

		// Start Verification.
		DP_ACQUISITION_PRIORITY ePriority = DP_PRIORITY_NORMAL; // Using Normal Priority, i.e. fingerprint will be sent to 
											  // this process only if it has active window on the desktop.
		_com_test_error(DPFPCreateAcquisition(ePriority, GUID_NULL, DP_SAMPLE_TYPE_IMAGE, m_hWnd, WMUS_FP_NOTIFY, &m_hOperationVerify));
		_com_test_error(DPFPStartAcquisition(m_hOperationVerify));

		SetDlgItemText(IDC_EDIT_PROMPT, L"Scan your finger for verification.");
	}
	catch(_com_error& E) {
		hr = E.Error();
	}
	catch(...) {
		hr = E_UNEXPECTED;
	}

	if (FAILED(hr)) {
		SetDlgItemText(IDC_EDIT_PROMPT, L"Error happened");
		ReportError(m_hWnd, hr);
		EndDialog(IDCANCEL);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CVerificationDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_hOperationVerify) {
		DPFPStopAcquisition(m_hOperationVerify);    // No error checking - what can we do at the end anyway?
		DPFPDestroyAcquisition(m_hOperationVerify);
		m_hOperationVerify = 0;
	}

	if (m_fxContext) {
		FX_closeContext(m_fxContext);
		m_fxContext = 0;
	}

	if (m_mcContext) {
		MC_closeContext(m_mcContext);
		m_mcContext = 0;
	}

	return 0;
}

LRESULT CVerificationDlg::OnFpNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	switch(wParam) {
		case WN_COMPLETED: {
	        AddStatus(L"Fingerprint image captured");

			DATA_BLOB* pImageBlob = reinterpret_cast<DATA_BLOB*>(lParam);

			// Display fingerprint image
			DisplayFingerprintImage(pImageBlob->pbData);

			// Match the new fingerprint image and Enrollment template
			Verify(pImageBlob->pbData, pImageBlob->cbData);
			break;
		}
		case WN_ERROR: {
			WCHAR buffer[101] = {0};
			_snwprintf(buffer, 100, L"Error happened. Error code: 0x%X", lParam);
	        AddStatus(buffer);
			break;
		}
		case WN_DISCONNECT:
	        AddStatus(L"Fingerprint reader disconnected");
			break;
		case WN_RECONNECT:
	        AddStatus(L"Fingerprint reader connected");
			break;
		case WN_FINGER_TOUCHED:
	        AddStatus(L"Finger touched");
			break;
		case WN_FINGER_GONE:
	        AddStatus(L"Finger gone");
			break;
		case WN_IMAGE_READY:
	        AddStatus(L"Fingerprint image ready");
			break;
		case WN_OPERATION_STOPPED:
	        AddStatus(L"Fingerprint Verification Operation stopped");
			break;
	}
	return 0;
}

LRESULT CVerificationDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}
void CVerificationDlg::DisplayFingerprintImage(FT_IMAGE_PT pFingerprintImage)
{
	HRESULT hr = S_OK;
	BITMAPINFO* pOutImage = NULL;
	try {
		// Prepare bitmap header and color table
		LONG dx = m_rcDrawArea.right - m_rcDrawArea.left;
		LONG dy = m_rcDrawArea.bottom - m_rcDrawArea.top;

		WORD uBPP = 8;
		ULONG LineWidth = WIDTHBYTES(dx * uBPP);

		int nColors = 256;
		DWORD dwColorsSize = nColors * sizeof(PALETTEENTRY);

		ULONG OutImgSize = sizeof(BITMAPINFOHEADER) + dwColorsSize + LineWidth * dy;
		if (NULL == (pOutImage = (BITMAPINFO*)new BYTE[OutImgSize])) _com_issue_error(E_OUTOFMEMORY);

		ULONG uImgSize = LineWidth*dy;

		// Height should be negative because the bitmap is a top-down.
		FillBitmapInfoHeader(&pOutImage->bmiHeader, dx, -dy, uImgSize, uBPP, nColors);

		PALETTEENTRY* pEntries = (PALETTEENTRY*)((LPBYTE)pOutImage + pOutImage->bmiHeader.biSize);
		FillColorTable(pEntries, nColors);

		FT_IMAGE_SIZE ImageSize = {LineWidth, dy};

		// call the toolkit function to get image pixels and draw the bitmap
		FT_RETCODE rc = FX_getDisplayImage(m_fxContext,         // Context of Feature Extraction
		                                   pFingerprintImage,   // Pointer to the image buffer
		                                   &ImageSize,          // Dimensions of the image - width must be divisible by 4 as appropriate for bitmaps
		                                   FALSE,               // Rotation, FALSE means do not rotate
		                                   nColors,             // requested number of intensity levels
		                                   ((BYTE *)pOutImage) + sizeof(BITMAPINFOHEADER) + dwColorsSize);
		if (FT_OK <= rc) {
			// Create bitmap and set its handle to the control for display.
			LPBYTE dstBits = NULL;
			HBITMAP hBmp = CreateDIBSection(0, pOutImage, DIB_RGB_COLORS, (void**)&dstBits, 0, 0);
			memcpy(dstBits, ((BYTE *)pOutImage) + sizeof(BITMAPINFOHEADER) + dwColorsSize, uImgSize);
			HBITMAP hOldBmp = reinterpret_cast<HBITMAP>(SendDlgItemMessage(IDC_STATIC_DRAWAREA, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp));
			if (hOldBmp)
				DeleteObject(hOldBmp);
		}
	}
	catch(_com_error E) {
		hr = E.Error();
	}
	catch(...) {
		hr = E_UNEXPECTED;
	}
	delete [] pOutImage;

	if (FAILED(hr))
		ReportError(m_hWnd, hr);
}

void CVerificationDlg::Verify(FT_IMAGE_PT pFingerprintImage, int iFingerprintImageSize) {
	HRESULT hr = S_OK;
	FT_BYTE* pVerTemplate = NULL;
	try {
		FT_RETCODE rc = FT_OK;

		// Get recommended length of the Pre-Enrollment feature sets.
		// It actually could be done once at the beginning, but this is just sample code...
		int iRecommendedVerFtrLen = 0;
		rc = FX_getFeaturesLen(FT_VER_FTR,
		                       &iRecommendedVerFtrLen,
		                       NULL); // Minimal Fingerprint feature set Length - do not use if possible
		if (FT_OK != rc) _com_issue_error(E_FAIL);

		// Extract Features (i.e. create fingerprint template)
		FT_IMG_QUALITY imgQuality;
		FT_FTR_QUALITY ftrQuality;
		FT_BOOL bEextractOK = FT_FALSE;

		if (NULL == (pVerTemplate = new FT_BYTE[iRecommendedVerFtrLen])) _com_issue_error(E_OUTOFMEMORY);
		rc = FX_extractFeatures(m_fxContext,             // Context of Feature Extraction
		                        iFingerprintImageSize,   // Size of the fingerprint image
		                        pFingerprintImage,       // Pointer to the image buffer
		                        FT_VER_FTR,              // Requested Verification Features
		                        iRecommendedVerFtrLen,   // Length of the Features(template) buffer received previously from the SDK
		                        pVerTemplate,            // Pointer to the buffer where the template to be stored
		                        &imgQuality,             // Returned quality of the image. If feature extraction fails because used did not put finger on the reader well enough, this parameter be used to tell the user how to put the finger on the reader better. See FT_IMG_QUALITY enumeration.
		                        &ftrQuality,             // Returned quality of the features. It may happen that the fingerprint of the user cannot be used. See FT_FTR_QUALITY enumeration.
		                        &bEextractOK);           // Returned result of Feature extraction. FT_TRUE means extracted OK.

		if (FT_OK <= rc && bEextractOK == FT_TRUE) {
			// Features extracted OK (i.e. fingerprint Verification template was created successfully)
			// Now match this template and the Enrollment template.
			FT_BOOL bRegFeaturesChanged = FT_FALSE;
			FT_VER_SCORE VerScore = FT_UNKNOWN_SCORE;
			double dFalseAcceptProbability = 0.0;
			FT_BOOL bVerified = FT_FALSE;

			rc = MC_verifyFeaturesEx(m_mcContext,              // Matching Context
			                         m_RegTemplate.cbData,     // Pointer to the Enrollment template content
			                         m_RegTemplate.pbData,     // Length of the Enrollment template
			                         iRecommendedVerFtrLen,    // Length of the Verification template
			                         pVerTemplate,             // Pointer to the Verification template content
			                         m_bDoLearning,            // Whether the Learning is desired - got it from checkbox in the dialog
			                         &bRegFeaturesChanged,     // Out: Whether the Learning actually happened
			                         NULL,                     // Reserved, must be NULL
			                         &VerScore,                // Out: Matching score, see score enumeration FT_VER_SCORE
			                         &dFalseAcceptProbability, // Out: Probability to falsely match these fingerprints
			                         &bVerified);              // Returned result of fingerprint match. FT_TRUE means fingerprints matched.
			if (FT_OK <= rc) {
				if (FT_OK != rc) {
					WCHAR buffer[101] = {0};
					ULONG uSize = 100;
					_snwprintf(buffer, uSize, L"Warning: %ld (see dpRCodes.h)", rc);
			        AddStatus(buffer);
				}

				if (bVerified == FT_TRUE) {
			        AddStatus(L"Fingerprint Matches!");
			        SetDlgItemText(IDC_EDIT_PROMPT, L"Scan another finger to run verification again.");
					WCHAR buffer[101] = {0};
					ULONG uSize = 100;
					_snwprintf(buffer, uSize, L"%lf", dFalseAcceptProbability);
					SetDlgItemText(IDE_PROBABILITY, buffer);
				}
				else {
			        AddStatus(L"Fingerprint did not Match!");
			        SetDlgItemText(IDC_EDIT_PROMPT, L"Scan another finger to run verification again.");
					WCHAR buffer[101] = {0};
					ULONG uSize = 100;
					_snwprintf(buffer, uSize, L"%lf", dFalseAcceptProbability);
					SetDlgItemText(IDE_PROBABILITY, buffer);
				}
			}
			else {
				WCHAR buffer[101] = {0};
				ULONG uSize = 100;
				_snwprintf(buffer, uSize, L"Verification Operation failed, Error: %ld.", rc);
				MessageBox(buffer, L"Fingerprint Verification", MB_OK | MB_ICONINFORMATION);
				SetDlgItemText(IDC_EDIT_PROMPT, L"Scan your finger for verification again.");
			}
		}
	}
	catch(_com_error E) {
		hr = E.Error();
	}
	catch(...) {
		hr = E_UNEXPECTED;
	}
	delete [] pVerTemplate;

	if (FAILED(hr))
		ReportError(m_hWnd, hr);
}
