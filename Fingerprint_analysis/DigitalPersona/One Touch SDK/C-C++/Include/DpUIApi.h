/*****************************************************************************
**                                                                          **
**  Copyright © 1996-2008 DigitalPersona, Inc.                              **
**                                                                          **
******************************************************************************
**
**  File:       DPUIApi.h
**
**  Contents:   DigitalPersona One Touch for Windows GUI Prototypes and Definitions
**
******************************************************************************/
#pragma once

#include <windows.h>
#include <wincrypt.h>

#ifdef DPFPUIAPI_EXPORTS
#define DPFPUI_STDAPI             EXTERN_C __declspec(dllexport) HRESULT STDAPICALLTYPE
#define DPFPUI_STDAPI_(type)      EXTERN_C __declspec(dllexport) type STDAPICALLTYPE
#else
#define DPFPUI_STDAPI             EXTERN_C __declspec(dllimport) HRESULT STDAPICALLTYPE
#define DPFPUI_STDAPI_(type)      EXTERN_C __declspec(dllimport) type STDAPICALLTYPE
#endif

typedef VOID DP_VERIFY_CONTEXT;

#define DP_DEFAULT_FALSE_ACCEPT_RATE	-1

/*! \enum DP_ENROLLMENT_ACTION
**  \brief Defines the enrollment action.
**
*/
typedef enum 
{
	DP_ENROLLMENT_ADD, /*!< Enroll the fingerprint template */
	DP_ENROLLMENT_DELETE /*!< Delete the fingerprint template */
}
DP_ENROLLMENT_ACTION;

/*! \brief This is the application defined callback funtion.
**
**  This is the application provided callback function. This function is called while enrolling 
**  new fingerprint or deleting of enrolled fingerprint. The application should handle storing 
**  of new fingerprint template for comparison or deleting of the enrolled fingerprint template 
**  in this callback. The application can display its own success or error messages.
**
**  \param [in] hParentWnd The handle to the parent window.
**  \param [in] enrollmentAction Specifies whether to enroll the figerprint or delete it.
**  \param [in] uiFingerIndex The index of the finger to be enrolled. This will be as defined 
**  in ANSI/NIST-ITL 1.\n
**  <TABLE>
**  <TR>
**      <TD>Finger Position<TD>ANSI Code
**  <TR>
**      <TD>Right thumb<TD>1
**  <TR>
**      <TD>Right index finger<TD>2
**  <TR>
**      <TD>Right middle finger<TD>3
**  <TR>
**      <TD>Right ring finger<TD>4
**  <TR>
**      <TD>Right little finger<TD>5
**  <TR>
**      <TD>Left thumb<TD>6
**  <TR>
**      <TD>Left index finger<TD>7
**  <TR>
**      <TD>Left middle finger<TD>8
**  <TR>
**      <TD>Left ring finger<TD>9
**  <TR>
**      <TD>Left little finger<TD>10
**  </TABLE>
**  \param [in] pFingerprintTemplate If the enrollment action DP_ENROLLMENT_ADD then 
**  it has the pointer to the enrollment template else it is NULL.
**  \param [in] pUserData The pointer to the user data.
**
**  \return If application saves the fingerprint template successfully, then it should 
**  return S_OK to stop scanning of fingerprint for enrollment. If there is an error, 
**  then application can return __HRESULT_FROM_WIN32(ERROR_RETRY) to try again fingerprint scanning and enrollment 
**  or it can return E_ABORT to stop enrollment and close the enrollment dialog box. 
**  While deleting the enrolled fingerprint, application should return S_OK which will 
**  update the fingerprint mask. 
*/
typedef HRESULT (CALLBACK *DPENROLLMENTPROC)(HWND					hParentWnd,
											 DP_ENROLLMENT_ACTION	enrollmentAction,
											 UINT					uiFingerIndex,
											 PDATA_BLOB				pFingerprintTemplate,
											 LPVOID					pUserData);

/*! \brief Brief description.
**         Brief description continued.
**
**  This is the application provided callback function. This function is called when 
**  fingerprint feature set is ready for comparison. The application should handle 
**  the comparison of this fingerprint feature set against the enrolled fingerprint 
**  templates.
**
**  \param [in] hParentWnd The handle to the parent window.
**  \param [in] pVerificationFeatureSet The pointer to fingerprint feature set to be 
**  verified.
**  \param [in] pUserData The pointer to the user data.
**
**  \return If the fingerprint feature set to be verified, matches one of the enrolled 
**  fingerprint templates, then application should return S_OK to close the verification 
**  dialog box. If there is mismatch, the application can return __HRESULT_FROM_WIN32(ERROR_RETRY) to try again or 
**  it can return E_ABORT to stop verification and close the verification dialog box. 
*/
typedef HRESULT (CALLBACK *DPVERIFYPROC)(HWND		hParentWnd,
										 PDATA_BLOB	pVerificationFeatureSet,
										 LPVOID		pUserData);

/*! \brief This function displays the fingerprint enrollment UI.
**
**  This function displays the UI for enrolling the fingerprints and returns after closing of UI.
**  This function does not store the fingerprint template instead it calls the application 
**  defined function DPENROLLMENTPROC for each enrollment or deletion of fingerprint.
**
**  \param [in] hParentWnd Handle to the parent window.
**  \param [in] usMaxEnrollFingerCount Maximum number of fingers can be enrolled. 
**  The value should be between 1 to 10 (both inclusive)
**  \param [in,out] pulEnrolledFingersMask Bitwise mask which specifies the fingers enrolled.\n
**  <TABLE>
**  <TR>
**      <TD>Bit Poisition<TD>4<TD>3<TD>2<TD>1<TD>0
**  <TR>
**      <TD>Finger Position<TD>Left thumb<TD>Left index finger<TD>Left middle finger
**      <TD>Left ring finger<TD>Left little finger
**  <TR>
**      <TD>Bit Position<TD>9<TD>8<TD>7<TD>6<TD>5
**  <TR>
**      <TD>Finger Position<TD>Right little finger<TD>Right ring finger
**      <TD>Right middle finger<TD>Right index finger<TD>Right thumb
**  </TABLE>
**  \n
**  \param [in] dpEnrollmentProc This is pointer to the function to be notified when fingerprint 
**  template is available for enrollment
**  \param [in] pUserData The pointer to the user data.
**
**  \return On success it returns S_OK. If user cancels the enrollment, then it
**  returns __HRESULT_FROM_WIN32(ERROR_RETRY). If user aborts the enrollment, then it reurns E_ABORT.
*/
DPFPUI_STDAPI DPEnrollUI(HWND hParentWnd,
						 USHORT usMaxEnrollFingerCount,
						 PULONG	pulEnrolledFingersMask,
						 DPENROLLMENTPROC dpEnrollmentProc,
						 LPVOID pUserData);

/*! \brief This function displays the fingerprint verification UI.
**
**  The title, text and banner image of the verification can be customized.
**  If the value of 
**
**  \param [in] hParentWnd Handle to the parent window.
**  \param [in] dpVerifyProc Pointer to the callback function.
**  \param [in] lpszCaption The caption of the dialog box.
**  \param [in] lpszText The text of the dialog box.
**  \param [in] hBanner The custom banner bitmap.
**  \param [in] pUserData The pointer to the user data.
**
**  \return On success it returns S_OK. If user cancels the verification, then it
**  returns __HRESULT_FROM_WIN32(ERROR_RETRY). If user aborts the verification, then it reurns E_ABORT.
*/
DPFPUI_STDAPI DPVerifyUI(HWND hParentWnd,
						 DPVERIFYPROC dpVerifyProc,
						 LPCWSTR lpszCaption,
						 LPCWSTR lpszText,
						 HBITMAP hBanner,
						 LPVOID pUserData);



