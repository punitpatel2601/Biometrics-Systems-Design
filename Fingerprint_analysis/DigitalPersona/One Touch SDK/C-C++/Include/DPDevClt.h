/*++
Copyright (C) 1997-2008, DigitalPersona, Inc

Module Name:
		DPDevClt.h

Abstract:
		DigitalPersona API for Fingerprint Cupture Device support.
Notes:
		Use doxygen to produce documentation.
--*/

/**

\defgroup apimain API Reference

The API Reference provides information on all events, data types and functions 
in the Digital Persona API for Fingerprint Capture Device support.
*/
#ifndef __DP_DEVICE_CLIENT_H__
#pragma once

#include <windows.h>

#pragma warning( disable : 4518 4502)

#ifdef DPFPAPI_EXPORTS

#define DPFP_STDAPI             EXTERN_C __declspec(dllexport) HRESULT STDAPICALLTYPE
#define DPFP_STDAPI_(type)      EXTERN_C __declspec(dllexport) type STDAPICALLTYPE

#else

#define DPFP_STDAPI             EXTERN_C __declspec(dllimport) HRESULT STDAPICALLTYPE
#define DPFP_STDAPI_(type)      EXTERN_C __declspec(dllimport) type STDAPICALLTYPE

#endif //DPFPAPI_EXPORTS

/** Acquisition priority. \ingroup typedefs */
typedef enum DP_ACQUISITION_PRIORITY
{	
	DP_PRIORITY_HIGH	= 1,					///< High acquisition priority. Subscriber uses this priority acquires device events exclusively. Only one subscriber with this priority is allowed.
	DP_PRIORITY_NORMAL	= 2,					///< Normal acquisition priority. Subscriber uses this priority acquires device events only if it runs as a foreground. Multiple subscribers with this priority are allowed.
	DP_PRIORITY_LOW		= 3						///< Low acquisition priority. Subscriber uses this priority acquires device events only if there are no subscribers with higher priority. Only one subscriber with this priority is allowed.
} 	DP_ACQUISITION_PRIORITY;

/** Device hardware/firmware version number. \ingroup typedefs */
typedef struct DP_DEVICE_VERSION 
{
	ULONG   uMajor;						///< Device major version.
	ULONG   uMinor;						///< Device minor version.
	ULONG   uBuild;						///< Device build number.
} DP_DEVICE_VERSION;

/** Device usage modality. \ingroup typedefs */
typedef enum DP_DEVICE_MODALITY
{
	DP_UNKNOWN_DEVICE_MODALITY = 0,				///< Device modality is unknown
	DP_SWIPE_DEVICE,							///< Swipe device
	DP_AREA_DEVICE,								///< Area device
	DP_DEVICE_MODALITY_NUM,						///< Count of different modalities defined
}DP_DEVICE_MODALITY;

/** Device technology. \ingroup typedefs */
typedef enum DP_DEVICE_TECHNOLOGY
{
	DP_UNKNOWN_DEVICE_TECHNOLOGY = 0,			///< Technology used is unknown
	DP_OPTICAL_DEVICE,							///< Optical device
	DP_CAPACITIVE_DEVICE,						///< Capacitive device
	DP_THERMAL_DEVICE,							///< Termal device
	DP_PRESSURE_DEVICE,							///< Pressure device
	DP_DEVICE_TECHNOLOGY_NUM,					///< Count of different technologies defined
}DP_DEVICE_TECHNOLOGY;

/** Type of UUID identifying the device. \ingroup typedefs */
typedef enum DP_DEVICE_UID_TYPE 
{
	DP_PERSISTENT_DEVICE_UID = 0,				///< Unique hardware identifier. HW dependent.
	DP_VOLATILE_DEVICE_UID,						///< Software generated identifier.
}DP_DEVICE_UID_TYPE;

#define  DP_MAX_USB_STRING_SIZE   (128) // use the full size supported by USB devices

/** Device hardware information. \ingroup typedefs */
typedef struct DP_HW_INFO 
{
	unsigned int   uLanguageId;						///< Device language
	wchar_t  szVendor[DP_MAX_USB_STRING_SIZE];		///< Manufacturer name (e.g. "Digital Persona, Inc.")
	wchar_t  szProduct[DP_MAX_USB_STRING_SIZE];		///< Product name (e.g. "U.are.U 4000B")
	wchar_t  szSerialNb[DP_MAX_USB_STRING_SIZE];	///< Serial number (e.g. "{7C265680-0056-FFFF-680D-A74033B09615}")
	DP_DEVICE_VERSION     HardwareRevision;            ///< Hardware revision
	DP_DEVICE_VERSION     FirmwareRevision;            ///< Firmware revision
} DP_HW_INFO, * PDP_HW_INFO;

/** Device information. \ingroup typedefs  */
typedef struct DP_DEVICE_INFO 
{
	GUID	              DeviceUid;			///< Device unique identifier
	DP_DEVICE_UID_TYPE   eUidType;				///< Persistent or volatile
	DP_DEVICE_MODALITY   eDeviceModality;		///< Usage modality
	DP_DEVICE_TECHNOLOGY eDeviceTech;			///< Type of technology used for the fingerprint reader 
	DP_HW_INFO           HwInfo;				///< Hardware description
} DP_DEVICE_INFO, * PDP_DEVICE_INFO;

/* Fingerprint image quality. */
typedef enum DP_SAMPLE_QUALITY {
/* Area (touch) sensor quality events. */
	DP_QUALITY_GOOD = 0,			// The image is of good quality.
	DP_QUALITY_NONE = 1,			// There is no image.
	DP_QUALITY_TOOLIGHT = 2,		// The image is too light.
	DP_QUALITY_TOODARK = 3,			// The image is too dark.
	DP_QUALITY_TOONOISY = 4,		// The image is too noisy.
	DP_QUALITY_LOWCONTR = 5,		// The image contrast is too low.
	DP_QUALITY_FTRNOTENOUGH = 6,	// The image does not contain enough information.
	DP_QUALITY_NOCENTRAL = 7,		// The image is not centered.
/* Swipe sensor quality events. */
	DP_QUALITY_NOFINGER = 8,		// The object scanned is not a finger.
	DP_QUALITY_TOOHIGH = 9,			// The finger was too high on the swipe sensor.
	DP_QUALITY_TOOLOW = 10,			// The finger was too low on the swipe sensor.
	DP_QUALITY_TOOLEFT = 11,		// The finger was too close to the left border of swipe sensor.
	DP_QUALITY_TOORIGHT	= 12,		// The finger was too close to the right border of swipe sensor.
	DP_QUALITY_TOOSTRANGE = 13,		// The scan looks strange.
	DP_QUALITY_TOOFAST = 14,		// The finger was swiped too quickly.
	DP_QUALITY_TOOSKEWED = 15,		// The image is too skewed.
	DP_QUALITY_TOOSHORT = 16,		// The image is too short.
	DP_QUALITY_TOOSLOW = 17,		// The finger was swiped too slowly.
} DP_SAMPLE_QUALITY;

/** DigitalPersona product version. \ingroup typedefs */
typedef struct DP_PRODUCT_VERSION
{
	WORD	wMajor;								///< Major product number.
	WORD	wMinor;								///< Minor product number.
	WORD	wRevision;							///< Revision product number.
	WORD	wBuild;								///< Build number.
}DP_PRODUCT_VERSION, *PDP_PRODUCT_VERSION;



#define DP_SAMPLE_TYPE_IMAGE	4

/** 
\defgroup events Notification Events
\ingroup apimain

\n

During the creation of an operation, the client specifies the handle of the window to be notified on operation-related events as well as the window message to be sent as a notification. 
The wParam of the message specifies the event type. The value of lParam is event-specific. 
*/

#define WN_COMPLETED				(0)	///< Operation completed successfully. The fingerprint image is returned in lParam as pointer to DATA_BLOB structure.\ingroup events
#define WN_ERROR					(1)	///< An error occurred. The error code is returned in lParam. \ingroup events

#define WN_DISCONNECT				(2)	///< The device has been disconnected. The pointer to device UID is retrned in lParam. \ingroup events
#define WN_RECONNECT				(3)	///< The device has been reconnected. The pointer to device UID is retrned in lParam. \ingroup events

#define WN_SAMPLE_QUALITY			(4)	// Provides information about the quality of the fingerprint image. lParam
										// contains the fingerprint image quality listed in the enum of type
										// DP_SAMPLE_QUALITY.

#define WN_FINGER_TOUCHED			(5) ///< The reader has been touched. \ingroup events
#define WN_FINGER_GONE				(6) ///< The finger has been removed from the reader. \ingroup events
#define WN_IMAGE_READY				(7)	///< An image is ready from the reader. The pointer to device UID is retrned in lParam. \ingroup events

#define WN_OPERATION_STOPPED		(10) ///< Sent when an operaton was stopped by calling \ref DPFPStopAcquisition. \ingroup events


/** 
\defgroup typedefs Data Types 
\ingroup apimain

\n

The data structures used in this SDK are:
*/

/** Operation handle. \ingroup typedefs */
typedef unsigned long HDPOPERATION;


/** 
\defgroup initfn Initialization functions 
\ingroup apimain

\n
*/

/** 
Allocates and initializes necessary resources. It MUST be called before any other DPFPApi calls except for \ref DPFPBufferFree.
\ingroup initfn
\sa DPFPTerm
\return Error code.
\retval S_OK in case of success
\retval S_FALSE if library is already initialized.
\retval 0x800706B3 The RPC server is not listening, which means that the Biometric Authentication Service has not been started.

\remark Every successful (i.e. FAILED() == FALSE) call of DPFPInit must have a corresponding call of \ref DPFPTerm function.
*/
DPFP_STDAPI DPFPInit();

/** 
Deallocates resources allocated by \ref DPFPInit. 
\ingroup initfn
*/
DPFP_STDAPI_(void) DPFPTerm();


/** 
\defgroup versfn Version functions
\ingroup apimain

\n
*/

/**
Gets the API version information.
\ingroup versfn
\param[out] pVersion pointer to the structure to be filled
\return Error code.
\retval S_OK in case of success.
*/
DPFP_STDAPI DPFPGetVersion(
	DP_PRODUCT_VERSION* pVersion
);


/** 
\defgroup memfn Memory management functions
\ingroup apimain

\n
*/

/**
Frees memory previously allocated by a DPFPApi call.
\ingroup memfn
\param[in] p The memory area to be freed.
*/
DPFP_STDAPI_(void) DPFPBufferFree(
	PVOID p
);


/** 
\defgroup acqfn Fingerprint sample acquisition operations
\ingroup apimain

\n
*/

/**
Creates a fingerprint sample acquisition operation. 
\ingroup acqfn
\param[in] eAcquisitionPriority Acquisition priority needed.
\param[in] DevUID Fingerprint Reader serial number. Can be GUID_NULL – if any reader can be used.
\param[in] uSampleType Type of fingerprint sample needed. This parameter must be DP_SAMPLE_TYPE_IMAGE.
\param[in] hWnd Handle of the window to be notified of operation events.
\param[in] uMsg Window message to be sent as an event notification.
\param[out] phOperation Pointer to operation handle to be filled if operation was created successfully.

\return Error code.
\retval S_OK if operation was successfully created.

\attention In order to free memory allocated for the operation created, the client must call \ref DPFPDestroyAcquisition for the handle returned in phOperation.
*/
DPFP_STDAPI DPFPCreateAcquisition(
	DP_ACQUISITION_PRIORITY eAcquisitionPriority,
	REFGUID DevUID,
	ULONG uSampleType,
	HWND hWnd,
	ULONG uMsg,
	HDPOPERATION* phOperation
);

/**
Destroys the operation previously created by \ref DPFPCreateAcquisition and deallocates all resources associated with that operation.
\ingroup acqfn
\param[in] hOperation Handle to operation to be destroyed.

\return Error code.
\retval S_OK if operation has destroyed successfully.
*/
DPFP_STDAPI DPFPDestroyAcquisition(
	HDPOPERATION hOperation
);

/**
Subscribes the client for receiving operation events notifications.
\ingroup acqfn
\param[in] hOperation Operation handle.

\return Error code.
\retval S_OK if subscribed.

\remark Each process can have no more than one active subscription for each operation priority level.
\sa DPFPStopAcquisition 
*/
DPFP_STDAPI DPFPStartAcquisition(
	HDPOPERATION hOperation
);

/**
Unsubscribes the client from receiving operation events notifications.
\ingroup acqfn
\param[in] hOperation Operation handle

\return Error code.
\retval S_OK if unsubscribed.

\sa DPFPStartAcquisition 
*/
DPFP_STDAPI DPFPStopAcquisition(
	HDPOPERATION hOperation
);

/** 
\defgroup devmngfn Device managment functions 
\ingroup apimain

\n
*/

/**
Enumerates available fingerprint readers.
\ingroup devmngfn
\param[out] puDevCount Number of readers available. If no readers are found, this number is 0.
\param[out] ppDevUID Pointer to be filled with the pointer to the array of serial numbers of connected readers. 
If NULL, only the number of available readers will be returned. 

\return Error code.
\retval S_OK if it succeeds.

\attention Caller must release returned memory by calling \ref DPFPBufferFree.
*/
DPFP_STDAPI DPFPEnumerateDevices(
	ULONG* puDevCount,
	GUID** ppDevUID
);

/**
Retrieves information about a particular reader.
\ingroup devmngfn
\param[in] DevUID Reader serial number. 
\param[in,out] pDevInfo Pointer to \ref DP_DEVICE_INFO structure that needs to be filled.

\return Error code.
\retval S_OK if information was retrieved.
*/
DPFP_STDAPI DPFPGetDeviceInfo(
	REFGUID DevUID,
	DP_DEVICE_INFO* pDevInfo
);

#endif // __DP_DEVICE_CLIENT_H__
