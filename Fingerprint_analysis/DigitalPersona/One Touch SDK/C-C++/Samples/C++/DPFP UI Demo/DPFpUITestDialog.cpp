// DPFpUITestDialog.cpp : Implementation of CDPFpUITestDialog

#include "stdafx.h"
//#include <atlimage.h>
#include "DPFpUITestDialog.h"
#include "DpUIApi.h"
#include "dpDefs.h"
#include "dpRCodes.h"
#include "dpMatch.h"

// CDPFpUITestDialog
static HRESULT retVal = S_OK;
static DATA_BLOB enrollmentTemplate[11] = {0};
static FT_BOOL bIsMatched = FT_FALSE;
static DOUBLE dFAR = 0.0;

CDPFpUITestDialog::CDPFpUITestDialog()
{
}
CDPFpUITestDialog::~CDPFpUITestDialog()
{
	for(UINT i=0; i<11; i++){
		if(enrollmentTemplate[i].pbData) delete [] enrollmentTemplate[i].pbData, enrollmentTemplate[i].pbData = NULL;
	}
}


HRESULT CALLBACK MyEnrollmentProc(HWND hParentWnd, DP_ENROLLMENT_ACTION enrollmentAction, 
					 UINT uiFingerIndex, PDATA_BLOB pFingerprintTemplate, LPVOID pData)
{
	HRESULT hr = S_OK;
	BOOL bSuccess = FALSE;

	switch (enrollmentAction)
	{
		case DP_ENROLLMENT_ADD :
			// Store new fingerprint template in database
			// bSuccess = storing_operation();
			if(enrollmentTemplate[uiFingerIndex].pbData) delete [] enrollmentTemplate[uiFingerIndex].pbData;
			enrollmentTemplate[uiFingerIndex].pbData = NULL;
			enrollmentTemplate[uiFingerIndex].cbData = 0;
			enrollmentTemplate[uiFingerIndex].pbData = new UCHAR[pFingerprintTemplate->cbData];
			memcpy(enrollmentTemplate[uiFingerIndex].pbData, pFingerprintTemplate->pbData, pFingerprintTemplate->cbData);
			enrollmentTemplate[uiFingerIndex].cbData = pFingerprintTemplate->cbData;
			break;
		case DP_ENROLLMENT_DELETE :
			// Delete fingerprint registration template from database
			// bSuccess = deleting_operation();
			if(enrollmentTemplate[uiFingerIndex].pbData) delete [] enrollmentTemplate[uiFingerIndex].pbData;
			enrollmentTemplate[uiFingerIndex].pbData = NULL;
			enrollmentTemplate[uiFingerIndex].cbData = 0;
			break;
	}

	switch (retVal)
	{
		case S_OK :
			MessageBox(hParentWnd, L"Success Message", L"Success", MB_OK);
			// Fingerprint stored successfully, so stop further scanning. Update fingerprint mask
			// Fingerprint deleted successfully, update fingerprintmask
			return S_OK; 
		case __HRESULT_FROM_WIN32(ERROR_RETRY):
			MessageBox(hParentWnd, L"Retry Message", L"Retry", MB_OK);
			// Error while storing/deleting, try again scanning and enrollment
			delete [] enrollmentTemplate[uiFingerIndex].pbData;
			enrollmentTemplate[uiFingerIndex].pbData = NULL;
			enrollmentTemplate[uiFingerIndex].cbData = 0;
			return __HRESULT_FROM_WIN32(ERROR_RETRY); 
		case E_ABORT :
			MessageBox(hParentWnd, L"Abort Message", L"Abort", MB_OK);
			//	Error while storing/deleting, stop scanning and enrollment.
			//	Also close enrollment dialog box
			delete [] enrollmentTemplate[uiFingerIndex].pbData;
			enrollmentTemplate[uiFingerIndex].pbData = NULL;
			enrollmentTemplate[uiFingerIndex].cbData = 0;
			return E_ABORT; 
	}
	return S_FALSE;
}

void Enrollment(HWND hParentWnd, PULONG pulMask, USHORT usMaxFingerCount)
{
	HRESULT	hr;
	
    // check for overflow
    if ( *pulMask > 1023 )
        *pulMask = 1023;

	// Display enrollment dialog box
	hr = DPEnrollUI(hParentWnd, usMaxFingerCount, pulMask, (DPENROLLMENTPROC)MyEnrollmentProc, NULL);
}

LRESULT CDPFpUITestDialog::OnBnClickedEnroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	ULONG	ulMask;
	USHORT	usMaxCount;

	ulMask = GetDlgItemInt(IDC_EDIT_MASK);
	usMaxCount = GetDlgItemInt(IDC_EDIT_MAX_COUNT);
	if (IsDlgButtonChecked(IDC_RADIO_ENROLL_SUCCESS))
		retVal = S_OK;
	else
	if (IsDlgButtonChecked(IDC_RADIO_ENROLL_FAIL))
		retVal = __HRESULT_FROM_WIN32(ERROR_RETRY);
	else
	if (IsDlgButtonChecked(IDC_RADIO_ENROLL_ABORT))
		retVal = E_ABORT;
	Enrollment(m_hWnd, &ulMask, usMaxCount);
	if(ulMask) EnableVerification(TRUE);
	else EnableVerification(FALSE);
	SetDlgItemInt(IDC_EDIT_MASK, ulMask);
	return 0;
}


HRESULT CALLBACK MyVerificationProc(HWND hParentWnd, DATA_BLOB *pVerificationFeatureSet, LPVOID pUserData)
{
	HRESULT		hr = __HRESULT_FROM_WIN32(ERROR_RETRY);
	FT_HANDLE	mcContext = static_cast<FT_HANDLE>(pUserData);

	if (mcContext == NULL)
	{
		return E_ABORT;
	}
	for (int fingerIndex = 1; fingerIndex <= 10; fingerIndex++)
	{
		if (enrollmentTemplate[fingerIndex].pbData != NULL)
		{
			double dFalseAcceptRate = 0.;

			MC_verifyFeaturesEx(mcContext,
					 enrollmentTemplate[fingerIndex].cbData,
					 enrollmentTemplate[fingerIndex].pbData,
					 pVerificationFeatureSet->cbData,
					 pVerificationFeatureSet->pbData,
					 0,
					 NULL,
					 NULL,
					 NULL,
					 &dFAR,
					 &bIsMatched);

			if (bIsMatched == FT_TRUE)
				break;
		}
	}
	if (bIsMatched == FT_TRUE)
	{
		MessageBox(hParentWnd, L"Success Message", L"Success", MB_OK);
		return S_OK;
	}
	else
	{
		if (retVal == __HRESULT_FROM_WIN32(ERROR_RETRY))
		{
			MessageBox(hParentWnd, L"Retry Message", L"Retry", MB_OK);
			return __HRESULT_FROM_WIN32(ERROR_RETRY);
		}
		else
		{
			MessageBox(hParentWnd, L"Abort Message", L"Abort", MB_OK);
			return E_ABORT;
		}
	}
}

void Verification(HWND hParentWnd, LPCWSTR szCaption, LPCWSTR szText, HBITMAP hBanner) 
{
	HRESULT		hr = S_OK;;

	// Initialize the fingerprint comparison module.
	if(FT_OK != MC_init()){
		MessageBox(hParentWnd, L"Failed to initialize fingerprint comparison module.", L"Error", MB_OK);
		return;
	}

	// Create a context for the fingerprint comparison module.
	FT_HANDLE mcContext = NULL;
	if(FT_OK != MC_createContext(&mcContext)){
		MessageBox(hParentWnd, L"Failed to create a context for the fingerprint comparison module.", L"Error", MB_OK);
		MC_terminate();
		return;
	}
	// Display verification dialog
	hr = DPVerifyUI(hParentWnd, (DPVERIFYPROC)MyVerificationProc, szCaption, szText, hBanner, (PVOID)mcContext);

	// Destroy a comparison context and releases the resources associated with it.
	MC_closeContext(mcContext);

	// Terminate the fingerprint comparison module and releases the resources associated with it.
	MC_terminate();

}

LRESULT CDPFpUITestDialog::OnBnClickedVerify(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	TCHAR szCaption[1025];
	TCHAR szText[1025];
	TCHAR szFAR[128];
	TCHAR szBannerBitmapFile[1025];
//	CImage imgBanner;
	HBITMAP hBanner = NULL;

	bIsMatched = FT_FALSE;
	dFAR = 0.0;
	if (IsDlgButtonChecked(IDC_RADIO_VERIFY_FAIL))
		retVal = __HRESULT_FROM_WIN32(ERROR_RETRY);
	else
	if (IsDlgButtonChecked(IDC_RADIO_VERIFY_ABORT))
		retVal = E_ABORT;
	GetDlgItemText(IDC_EDIT_CAPTION, szCaption, 1024);
	GetDlgItemText(IDC_EDIT_TEXT, szText, 1024);
	GetDlgItemText(IDC_EDIT_BANNER, szBannerBitmapFile, 1024);
	if (_tcslen(szBannerBitmapFile) > 0)
	{
		hBanner = (HBITMAP)LoadImage(NULL, szBannerBitmapFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		//if (SUCCEEDED(imgBanner.Load(szBannerBitmapFile)))
		//{
		//	hBanner = imgBanner;
		//}
	}
	Verification(m_hWnd, szCaption, szText, hBanner);
	CheckDlgButton(IDC_CHECK_IS_MATCHED, bIsMatched);
	_stprintf(szFAR, L"%f", dFAR);
	SetDlgItemText(IDC_EDIT_FAR, szFAR);

	return 0;
}
