// SampleRegDlg.cpp : implementation file
//
#include "stdafx.h"
#include "EnrollmentDlg.h"
#include "Utilities.h"
#include "dpFtrEx.h"
#include "dpMatch.h"

/**************************************************************************************************
                                Sample code for Fingerprint Enrollment
                                Copyright Digital Persona, Inc. 1996-2007
/*************************************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CEnrollmentDlg dialog

CEnrollmentDlg::CEnrollmentDlg() : 
	CDialogImpl<CEnrollmentDlg>(), 
	m_hOperationEnroll(0),
	m_fxContext(0),
	m_mcContext(0),
	m_TemplateArray(NULL),
	m_nRegFingerprint(0),
	m_mcRegOptions(FT_DEFAULT_REG)
{
	::ZeroMemory(&m_rcDrawArea, sizeof(m_rcDrawArea));
	::ZeroMemory(&m_RegTemplate, sizeof(m_RegTemplate));
}

CEnrollmentDlg::~CEnrollmentDlg() {
	delete [] m_RegTemplate.pbData;
	m_RegTemplate.cbData = 0;
	m_RegTemplate.pbData = NULL;
}

void CEnrollmentDlg::GetRegTemplate(DATA_BLOB& rRegTemplate) const {
	if (m_RegTemplate.cbData && m_RegTemplate.pbData) { // only copy template if it is not empty
		// Delete the old stuff that may be in the template.
		delete [] rRegTemplate.pbData;
		rRegTemplate.pbData = NULL;
		rRegTemplate.cbData = 0;

		// Copy the new template, but only if it has been created.
		rRegTemplate.pbData = new BYTE[m_RegTemplate.cbData];
		if (!rRegTemplate.pbData) _com_issue_error(E_OUTOFMEMORY);
		::CopyMemory(rRegTemplate.pbData, m_RegTemplate.pbData, m_RegTemplate.cbData);
		rRegTemplate.cbData = m_RegTemplate.cbData;
	}
}

void CEnrollmentDlg::AddStatus(LPCWSTR s) {
	int lIdx = SendDlgItemMessage(IDC_STATUS, LB_ADDSTRING, 0, (LPARAM)s);
	SendDlgItemMessage(IDC_STATUS, LB_SETTOPINDEX, lIdx, 0);
}

LRESULT CEnrollmentDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = S_OK;
	try {
		FT_RETCODE rc = FT_OK;

		// Create Context for Feature Extraction
		if (FT_OK != (rc = FX_createContext(&m_fxContext))) {
			MessageBox(L"Cannot create Feature Extraction Context.", L"Fingerprint Enrollment", MB_OK | MB_ICONSTOP);
			_com_issue_error(E_FAIL);
		}

		// Create Context for Matching
		if (FT_OK != (rc = MC_createContext(&m_mcContext))) {
			MessageBox(L"Cannot create Matching Context.", L"Fingerprint Enrollment", MB_OK | MB_ICONSTOP);
			_com_issue_error(E_FAIL);
		}

		// Get number of Pre-Enrollment feature sets needed to create on Enrollment template
		// allocate array that keeps those Pre-Enrollment and set the first index to 0;
		MC_SETTINGS mcSettings = {0};
		if (FT_OK != (rc = MC_getSettings(&mcSettings))) {
			MessageBox(L"Cannot get number of Pre-Reg feature sets needed to create one Enrollment template.", L"Fingerprint Enrollment", MB_OK | MB_ICONSTOP);
			_com_issue_error(E_FAIL);
		}

		m_NumberOfPreRegFeatures = mcSettings.numPreRegFeatures;
		if (NULL == (m_TemplateArray = new FT_BYTE*[m_NumberOfPreRegFeatures]))
			_com_issue_error(E_OUTOFMEMORY);

		::ZeroMemory(m_TemplateArray, sizeof(FT_BYTE**)*m_NumberOfPreRegFeatures);

		m_nRegFingerprint = 0;  // This is index of the array where the first template is put.


		// Get area of the fingerprint image on the screen from the helper control
		::GetWindowRect(GetDlgItem(IDC_STATIC_DRAWAREA_SIZE), &m_rcDrawArea);

		// Start Enrollment.
		DP_ACQUISITION_PRIORITY ePriority = DP_PRIORITY_NORMAL; // Using Normal Priority, i.e. fingerprint will be sent to 
											  // this process only if it has active window on the desktop.
		_com_test_error(DPFPCreateAcquisition(ePriority, GUID_NULL, DP_SAMPLE_TYPE_IMAGE, m_hWnd, WMUS_FP_NOTIFY, &m_hOperationEnroll));
		_com_test_error(DPFPStartAcquisition(m_hOperationEnroll));

		SetDlgItemText(IDC_EDIT_PROMPT, L"Scan your finger for enrollment.");
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

LRESULT CEnrollmentDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_hOperationEnroll) {
		DPFPStopAcquisition(m_hOperationEnroll);    // No error checking - what can we do at the end anyway?
		DPFPDestroyAcquisition(m_hOperationEnroll);
		m_hOperationEnroll = 0;
	}

	if (m_fxContext) {
		FX_closeContext(m_fxContext);
		m_fxContext = 0;
	}

	if (m_mcContext) {
		MC_closeContext(m_mcContext);
		m_mcContext = 0;
	}

	if(m_TemplateArray){
		for (int i=0; i<m_NumberOfPreRegFeatures; ++i)
			if(m_TemplateArray[i]) delete [] m_TemplateArray[i], m_TemplateArray[i] = NULL; // Delete Pre-Enrollment feature sets stored in the array.
		
		delete [] m_TemplateArray; // delete the array itself.
		m_TemplateArray = NULL;
	}

	return 0;
}

LRESULT CEnrollmentDlg::OnFpNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	switch(wParam) {
		case WN_COMPLETED: {
	        AddStatus(L"Fingerprint image captured");
			DATA_BLOB* pImageBlob = reinterpret_cast<DATA_BLOB*>(lParam);

			// Display fingerprint image
			DisplayFingerprintImage(pImageBlob->pbData);

			// Add fingerprint image to the Enrollment
			AddToEnroll(pImageBlob->pbData, pImageBlob->cbData);
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
	        AddStatus(L"Fingerprint Enrollment Operation stopped");
			break;
	}
	return 0;
}

LRESULT CEnrollmentDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

//LRESULT CEnrollmentDlg::OnModeCheckBox(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
//{
//	m_mcRegOptions = 0;
//
//	if (BST_CHECKED == IsDlgButtonChecked(IDB_DISABLE_DEFAULT_REG))
//		m_mcRegOptions |= FT_DISABLE_DEFAULT_REG;
//
//	if (BST_CHECKED == IsDlgButtonChecked(IDB_ENABLE_LEARNING))
//		m_mcRegOptions |= FT_ALLOW_LEARNING;
//
//	if (BST_CHECKED == IsDlgButtonChecked(IDB_ENABLE_XTF_REG))
//		m_mcRegOptions |= FT_ENABLE_XTF_REG;
//
//	return 0;
//}

void CEnrollmentDlg::DisplayFingerprintImage(FT_IMAGE_PT pFingerprintImage)
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

void CEnrollmentDlg::AddToEnroll(FT_IMAGE_PT pFingerprintImage, int iFingerprintImageSize)
{
	HRESULT hr = S_OK;
	FT_BYTE* pPreRegTemplate = NULL;
	FT_BYTE* pRegTemplate = NULL;
	try {
		if (m_nRegFingerprint < m_NumberOfPreRegFeatures) { // Do not generate more Pre-Enrollment feature sets than needed
			FT_RETCODE rc = FT_OK;

			// Get recommended length of the Pre-Enrollment Fingerprint feature sets.
			// It actually could be done once at the beginning, but this is just sample code...
			int iRecommendedPreRegFtrLen = 0;
			rc = FX_getFeaturesLen(FT_PRE_REG_FTR,
								   &iRecommendedPreRegFtrLen,
								   NULL); // Minimal Fingerprint feature set Length - do not use if possible
			if (FT_OK != rc) _com_issue_error(E_FAIL);

			// Extract Features (i.e. create fingerprint template)
			FT_IMG_QUALITY imgQuality;
			FT_FTR_QUALITY ftrQuality;
			FT_BOOL bEextractOK = FT_FALSE;

			if (NULL == (pPreRegTemplate = new FT_BYTE[iRecommendedPreRegFtrLen])) _com_issue_error(E_OUTOFMEMORY);
			rc = FX_extractFeatures(m_fxContext,              // Context of Feature Extraction
									iFingerprintImageSize,    // Size of the fingerprint image
									pFingerprintImage,        // Pointer to the image buffer
									FT_PRE_REG_FTR,           // Requested Pre-Enrollment Features
									iRecommendedPreRegFtrLen, // Length of the Features(template) buffer received previously from the SDK
									pPreRegTemplate,          // Pointer to the buffer where the template to be stored
									&imgQuality,              // Returned quality of the image. If feature extraction fails because used did not put finger on the reader well enough, this parameter be used to tell the user how to put the finger on the reader better. See FT_IMG_QUALITY enumeration.
									&ftrQuality,              // Returned quality of the features. It may happen that the fingerprint of the user cannot be used. See FT_FTR_QUALITY enumeration.
									&bEextractOK);            // Returned result of Feature extraction. FT_TRUE means extracted OK.

			// If feature extraction succeeded, add the pre-Enrollment feature sets 
			// to the set of 4 templates needed to create Enrollment template.
			if (FT_OK <= rc && bEextractOK == FT_TRUE) {
				AddStatus(L"Pre-Enrollment feature set generated successfully");

				m_TemplateArray[m_nRegFingerprint++] = pPreRegTemplate;
				pPreRegTemplate = NULL;   // This prevents deleting at the end of the function

				// Display current fingerprint image number and total required number of images.
				WCHAR buffer[101] = {0};
				ULONG uSize = 100;
				_snwprintf(buffer, uSize, L"%ld", m_nRegFingerprint);
				SetDlgItemText(IDC_IMAGE_NUM, buffer);
				_snwprintf(buffer, uSize, L"%ld", m_NumberOfPreRegFeatures);
				SetDlgItemText(IDC_IMAGE_TOTAL, buffer);

				if (m_nRegFingerprint == m_NumberOfPreRegFeatures) { // We collected enough Pre-Enrollment feature sets, create Enrollment template out of them.
					// Get the recommended length of the fingerprint template (features).
					// It actually could be done once at the beginning, but this is just sample code...
					int iRecommendedRegFtrLen = 0;
					rc = MC_getFeaturesLen(FT_REG_FTR,
										   m_mcRegOptions,
										   &iRecommendedRegFtrLen,
										   NULL);

					if (FT_OK != rc) _com_issue_error(E_FAIL);

					if (NULL == (pRegTemplate = new FT_BYTE[iRecommendedRegFtrLen])) _com_issue_error(E_OUTOFMEMORY);

					FT_BOOL bRegSucceeded = FT_FALSE;
					rc = MC_generateRegFeatures(m_mcContext,              // Matching Context
												m_mcRegOptions,           // Options - collected from checkboxes in the dialog
												m_NumberOfPreRegFeatures, // Number of Pre-Enrollment feature sets submitted. It must be number received previously from the SDK.
												iRecommendedPreRegFtrLen, // Length of every Pre-Enrollment feature sets in the array
												m_TemplateArray,          // Array of pointers to the Pre-Enrollment feature sets
												iRecommendedRegFtrLen,    // Length of the Enrollment Features(template) buffer received previously from the SDK
												pRegTemplate,             // Pointer to the buffer where the Enrollment template to be stored
												NULL,                     // Reserved. Must always be NULL.
												&bRegSucceeded);          // Returned result of Enrollment Template creation. FT_TRUE means extracted OK.

					if (FT_OK <= rc && bRegSucceeded == FT_TRUE) {
						// Enrollment template is generated.
						// Normally it should be saved in some database, etc.
						// Here we just put it into a BLOB to use later for verification.
						m_RegTemplate.pbData = pRegTemplate;
						m_RegTemplate.cbData = iRecommendedRegFtrLen;

						pRegTemplate = NULL;   // This prevents deleting at the end of the function

						AddStatus(L"Enrollment Template generated successfully");
						SetDlgItemText(IDC_EDIT_PROMPT, L"Close this dialog and run Verification to verify fingerprint");
					}
					else {
						MessageBox(L"Creation of Enrollment Template Failed.", L"Fingerprint Enrollment", MB_OK | MB_ICONINFORMATION);
						SetDlgItemText(IDC_EDIT_PROMPT, L"Scan your finger for enrollment.");
						// Enrolment faled, cleanup data.
						m_nRegFingerprint = 0;

						for (int i=0; i<m_NumberOfPreRegFeatures; ++i)
							if(m_TemplateArray[i]) delete [] m_TemplateArray[i], m_TemplateArray[i] = NULL; // Delete Pre-Enrollment feature sets stored in the array.
					}
				}
				else {
					// This is normal cource of events, before all 4 fingerprint images are captured.
					SetDlgItemText(IDC_EDIT_PROMPT, L"Scan same finger again.");
				}
			}
			else {
				MessageBox(L"Creation of Pre-Enrollment feature set Failed.", L"Fingerprint Enrollment", MB_OK | MB_ICONINFORMATION);
				SetDlgItemText(IDC_EDIT_PROMPT, L"Scan your finger for enrollment.");
			}
		}
		else {
			MessageBox(L"An Enrollment Template has already been created.\n\nClose the dialog, then select \"Fingerprint Verification\" or \"Save Fingerprint Enrollment Template\".", L"Fingerprint Enrollment", MB_OK | MB_ICONSTOP);
		}
	}
	catch(_com_error E) {
		hr = E.Error();
	}
	catch(...) {
		hr = E_UNEXPECTED;
	}
	delete [] pPreRegTemplate;
	delete [] pRegTemplate;

	if (FAILED(hr))
		ReportError(m_hWnd, hr);
}
