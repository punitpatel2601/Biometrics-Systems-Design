/*****************************************************************************
**                                                                          **
**  Copyright © 1996-2008 DigitalPersona, Inc.                              **
**                                                                          **
******************************************************************************
**
**  name:     dpFtrEx.h
**
**  version:  4.1.1
**
**  language: ANSI C
**
**  notes:    public header
**            prefix FX_
**            Fingerprint Feature Extraction Functions
**
******************************************************************************/

/********************************* OVERVIEW ********************************** 

A fingerprint is an impression of the ridges on the skin of a finger. The DigitalPersona fingerprint recognition 
system uses the distinctive and persistent characteristics from the ridges, also referred to as fingerprint 
features, to distinguish one finger (or person) from another. These fingerprint features are then converted, or 
extracted, and used in comparison and decision-making to provide reliable personal recognition.

This file contains code for creating fingerprint feature sets by applying fingerprint feature extraction to the 
fingerprint image obtained from the fingerprint reader to compute repeatable and distinctive information. 
Depending on the specified feature set purpose, this information can be used for either fingerprint enrollment or 
verification.

*************************** SUMMARY OF FUNCTIONS *****************************

Fingerprint Feature Extraction Module Initialization, Termination, and Settings
    FX_getVersionInfo: Retrieves the software version information of the fingerprint feature extraction module.
    FX_init: Initializes the fingerprint feature extraction module.
    FX_terminate: Terminates the fingerprint feature extraction module and releases the resources associated with it.
    FX_createContext: Creates a context for the fingerprint feature extraction module.
    FX_closeContext: Destroys a feature extraction context and releases the resources associated with it.

Fingerprint Feature Extraction
    FX_extractFeatures: Creates a fingerprint feature set for the purpose of fingerprint enrollment or verification.
    FX_getDisplayImage: Prepares the fingerprint image obtained from the fingerprint reader for display.

*******************************************************************************/

#ifndef FX_DEFINED
#define FX_DEFINED

#ifndef FX_DLL_INTERFACE

#if defined(__GNUC__) && (__GNUC__ >= 3)
# define FX_IMPORT
#elif defined(_MSC_VER)
# define FX_IMPORT __declspec(dllimport)
#else
# define FX_IMPORT
#endif

#ifdef __cplusplus
#ifdef USING_STATIC_LIBS
#define FX_DLL_INTERFACE    extern "C"
#else
#define FX_DLL_INTERFACE    extern "C" FX_IMPORT
#endif // USING_STATIC_LIBS
#else 
#ifdef USING_STATIC_LIBS
#define FX_DLL_INTERFACE
#else
#define FX_DLL_INTERFACE    FX_IMPORT
#endif // USING_STATIC_LIBS
#endif // __cplusplus

#endif // !FX_DLL_INTERFACE

FX_DLL_INTERFACE void
FX_getVersionInfo(
  OUT FT_VERSION_INFO_PT fxModuleVersionPt); /* Pointer to the buffer containing the fingerprint feature extraction 
                                                module software version information */
                                                
/* Retrieves the software version information of the fingerprint feature extraction module in the structure of 
type FT_VERSION_INFO. 
----------------------------------------------------------------------------------------*/

FX_DLL_INTERFACE FT_RETCODE
FX_init(void);

/* Initializes the fingerprint feature extraction module. This function must be called before any other functions 
in the module are called. 

Return Codes:
  FT_OK:                  The function succeeded.
  FT_ERR_NO_MEMORY:       There is not enough memory to initialize the fingerprint feature extraction module.
  FT_ERR_BAD_INI_SETTING: Initialization settings are corrupted.
----------------------------------------------------------------------------------------*/

FX_DLL_INTERFACE FT_RETCODE
FX_terminate(void);

/* Terminates the fingerprint feature extraction module and releases the resources associated with it. 

Return Codes:
  FT_OK: The function succeeded.
  FT_WRN_NO_INIT: The fingerprint feature extraction module is not initialized.           
----------------------------------------------------------------------------------------*/

FX_DLL_INTERFACE FT_RETCODE
FX_createContext(
  OUT FT_HANDLE* fxContext); /* Pointer to the memory receiving the handle to the feature extraction context */

/* Creates a context for the fingerprint feature extraction module. If this function succeeds, it returns the 
handle to the context that is created. All of the operations in this context require this handle. 

Return Codes:
 FT_OK:            The function succeeded.
 FT_ERR_NO_INIT:   The fingerprint feature extraction module is not initialized.
 FT_ERR_NO_MEMORY: There is not enough memory to create a feature extraction context.
----------------------------------------------------------------------------------------*/

FX_DLL_INTERFACE FT_RETCODE
FX_closeContext(
  IN  FT_HANDLE fxContext); /* Handle to the feature extraction context */

/* Destroys a feature extraction context and releases the resources associated with it. 

Return Codes:
  FT_OK:                  The function succeeded.
  FT_ERR_NO_INIT:         The fingerprint feature extraction module is not initialized.
  FT_ERR_INVALID_CONTEXT: The given feature extraction context is not valid.
----------------------------------------------------------------------------------------*/

FX_DLL_INTERFACE FT_RETCODE
FX_getFeaturesLen(
  IN  FT_FTR_TYPE featureSetPurpose,  /* Feature set purpose. Specifies the purpose for which the fingerprint 
                                         feature set is to be created. For a fingerprint feature set to be used 
                                         for enrollment, use the value FT_PRE_REG_FTR; for verification, use 
                                         FT_VER_FTR. FT_REG_FTR is not a valid value for this function. */
  OUT int* recommendedFeatureSetSize, /* Pointer to the memory receiving the size of the buffer for the fingerprint 
                                         feature set recommended for best recognition accuracy, or NULL. If NULL 
                                         is passed, minimumFeatureSetSize must not be NULL. */
  OUT int* minimumFeatureSetSize);    /* Pointer to the memory receiving the minimum size of the buffer for the 
                                         fingerprint feature set, or NULL. If NULL is passed, 
                                         recommendedFeatureSet must not be NULL. */

/* Retrieves the size of the buffer for the fingerprint feature set. This function returns either the minimum or 
the recommended size that provides the best recognition accuracy, or both. 

Return Codes:
  FT_OK:                The function succeeded.
  FT_ERR_NO_INIT:       The fingerprint feature extraction module is not initialized.
  FT_ERR_INVALID_PARAM: The parameter featureSetPurpose is not valid.
----------------------------------------------------------------------------------------*/

FX_DLL_INTERFACE FT_RETCODE
FX_extractFeatures(
  IN  FT_HANDLE fxContext,                 /* Handle to the feature extraction context */
  IN  int imageSize,                       /* Size of the fingerprint image obtained from the fingerprint reader, in bytes */
  IN  const FT_IMAGE_PTC imagePt,          /* Pointer to the buffer containing the fingerprint image obtained from 
                                              the fingerprint reader */
  IN  FT_FTR_TYPE featureSetPurpose,       /* Feature set purpose. Specifies the purpose for which the fingerprint 
                                              feature set is to be created. For a fingerprint feature set to be used 
                                              for enrollment, use the value FT_PRE_REG_FTR; for verification, use 
                                              FT_VER_FTR. FT_REG_FTR is not a valid value for this function. */
  IN  int featureSetSize,                  /* Fingerprint feature set size. This parameter is the size, in bytes, 
                                              of the fingerprint feature set. Use the FX_getFeaturesLen function to 
                                              obtain information about which fingerprint feature set size to use. */
  OUT FT_BYTE* featureSet,                 /* Pointer to the location of the buffer receiving the fingerprint feature set */
  OUT FT_IMG_QUALITY_PT imageQualityPt,    /* Pointer to the buffer containing information about the quality of the 
                                              fingerprint image. Fingerprint image quality is represented by one of 
                                              the following values: FT_GOOD_IMG. The fingerprint image quality is 
                                              good. FT_IMG_TOO_LIGHT. The fingerprint image is too light. 
                                              FT_IMG_TOO_DARK. The fingerprint image is too dark. FT_IMG_TOO_NOISY. 
                                              The fingerprint image is too noisy. FT_LOW_CONTRAST. The fingerprint 
                                              image contrast is too low. FT_UNKNOWN_IMG_QUALITY. The fingerprint image 
                                              quality is undetermined. */
  OUT FT_FTR_QUALITY_PT featuresQualityPt, /* Pointer to the buffer containing information about the quality of the 
                                              fingerprint features. If the fingerprint image quality (imageQualityPt) 
                                              is not equal to the value FT_GOOD_IMG, fingerprint feature extraction
                                              is not attempted, and this parameter is set to the value 
                                              FT_UNKNOWN_FTR_QUALITY. Fingerprint features quality is represented by 
                                              one of the following values: FT_GOOD_FTR. The fingerprint features 
                                              quality is good. FT_NOT_ENOUGH_FTR. There are not enough fingerprint 
                                              features. FT_NO_CENTRAL_REGION. The fingerprint image does not contain 
                                              the central portion of the finger. FT_AREA_TOO_SMALL. The fingerprint 
                                              image area is too small. FT_UNKNOWN_FTR_QUALITY. The fingerprint features 
                                              quality is undetermined. */
  OUT FT_BOOL* featureSetCreated);         /* Pointer to the memory receiving the value of whether the fingerprint 
                                              feature set is created. If the value of this parameter is FT_TRUE, the 
                                              fingerprint feature set was written to featureSet. If the value is 
                                              FT_FALSE, a fingerprint feature set was not created. */

/* Creates a fingerprint feature set by applying fingerprint feature extraction to the fingerprint image obtained 
from the fingerprint reader to compute repeatable and distinctive information. Depending on the specified feature 
set purpose, this information can be used for either fingerprint enrollment or verification. 

Return Codes:
    FT_OK:                  The function succeeded.
    FT_ERR_NO_INIT:         The fingerprint feature extraction module is not initialized.
    FT_ERR_INVALID_CONTEXT: The given feature extraction context is not valid.
    FT_ERR_INVALID_PARAM:   One or more parameters are not valid.
    FT_ERR_NO_MEMORY:       There is not enough memory to perform fingerprint feature extraction.
    FT_ERR_UNKNOWN_DEVICE:  The fingerprint reader is not supported.
----------------------------------------------------------------------------------------*/

FX_DLL_INTERFACE FT_RETCODE
FX_getDisplayImage(
    IN FT_HANDLE fxContext,               /* Handle to the feature extraction context */
    IN const FT_IMAGE_PTC imagePt,        /* Pointer to the buffer containing the fingerprint image obtained from 
                                             the fingerprint reader */
    IN const FT_IMAGE_SIZE_PT pImageSize, /* Pointer to the buffer containing the size of the fingerprint image 
                                             obtained from the fingerprint reader */
    IN const FT_BOOL imageRotation,       /* Indicates whether the image is to be rotated. If the value of this 
                                             parameter is equal to FT_TRUE, the image is rotated by 90 degrees. If 
                                             the value is FT_FALSE, the image is not rotated. */
    IN const int numIntensityLevels,      /* Requested number of grayscale intensity levels */
    IN OUT FT_IMAGE_PT pImageBuffer);     /* Pointer to the buffer containing the image obtained from the fingerprint 
                                             reader. The image to be displayed is returned in the same buffer. */

/* Prepares the fingerprint image obtained from the fingerprint reader for display. This may involve resizing, 
changing the number of grayscale intensity levels, rotating, and otherwise processing the fingerprint image to 
ensure that it displays well. The fingerprint image passed to the FX_getDisplayImage function is the same 
fingerprint image used by the FX_extractFeatures function.

Return Codes:
    FT_OK:                  The function succeeded.
    FT_ERR_NO_INIT:         The fingerprint feature extraction module is not initialized.
    FT_ERR_INVALID_CONTEXT: The given feature extraction context is not valid.
    FT_ERR_INVALID_PARAM:   One or more parameters are not valid.
    FT_ERR_NO_MEMORY:       There is not enough memory to perform the function.
    FT_ERR_UNKNOWN_DEVICE:  The fingerprint reader is not supported.
----------------------------------------------------------------------------------------*/

#endif
