========================================================================
       DigitalPersona, Inc. : Enrollment sample code project
========================================================================

This sample code demonstrates how to use the enrollment and verification 
features of DPFPApi.

To build the sample, use VC++ 7.0 or later.

The sample dialog boxes are based on ATL dialogs.

Main Dialog
====================

The main dialog contains buttons for calling the Enrollment and Verification dialogs, as 
well as saving the Enrollment template to disk and reading it from disk.

When the Enrollment dialog is closed, the fingerprint Enrollment template is passed 
to the Main dialog, where in turn, it can be passed to the Verification dialog or saved to 
disk.

It is also possible to read a previously saved template from disk and use it for fingerprint 
verification instead of creating a new Enrollment template using the  Enrollment dialog.

Enrollment Dialog
====================

This dialog enrolls fingerprints, i.e. it captures four fingerprint images, converts them 
into fingerprint pre-enrollment feature sets, and then creates the Enrollment template from 
them. 

The function OnInitDialog creates the required Feature Extraction and Matching contexts, 
and then starts fingerprint image capture using DPFPApi functions. It also gets the 
dimensions of the fingerprint image to be drawn in the dialog from the helper control 
IDC_STATIC_DRAWAREA_SIZE.

The Enrollment dialog contains a number of checkboxes for specifying the parameters of 
the Enrollment template. The states of each of these checkboxes are collected and put into 
the variable m_mcRegOptions, which is passed to the enrollment function.

When any fingerprint event happens, the dialog receives a message, (WMUS_FP_NOTIFY, which 
was previously passed to DPFPApi) and processes the parameters of the message. wParam 
contains the code for the event. All events are traced in the list box in the dialog. 
Receipt of a  WN_COMPLETED event means that the fingerprint image has been captured. lParam 
contains a  pointer to the image. The dialog displays the image using the helper function 
DisplayFingerprintImage and then passes it to the helper function AddToEnroll.

The function DisplayFingerprintImage first prepares a regular bitmap header, then uses the 
SDK function to get the fingerprint image bits and finally, uses CreateDIBSection to put 
the bits into the bitmap. Then it sets the bitmap handle to the control in the dialog 
(IDC_STATIC_DRAWAREA) that displays the image.

Function AddToEnroll uses SDK functions to create a fingerprint pre-enrollment feature set 
from the fingerprint image and add it to the array of pre-enrollment feature sets. When the 
number of pre-enrollment feature sets in the array reaches the number required for 
enrollment, it uses the SDK function MC_generateRegFeatures to create one Enrollment 
template. It uses m_mcRegOptions to create the template with the appropriate features. The 
template is saved in the member BLOB variable m_RegTemplate. This variable is copied from 
this dialog into the main dialog, where it can be passed to the Verification dialog or 
saved on disk.

The OnDestroy function closes all contexts and stops fingerprint image capture.


Verification Dialog
====================

This dialog verifies a fingerprint, i.e. it captures a fingerprint image, converts 
it into a fingerprint verification feature set, and compares the feature set against a fingerprint 
enrollment template (either one previously created in the Enrollment dialog or one read 
from disk.)

The function OnInitDialog creates the required Feature Extraction and Matching 
contexts, and then starts fingerprint image capture using DPFPApi functions. 
It also gets the dimensions of the fingerprint image to be drawn in the dialog from 
the helper control IDC_STATIC_DRAWAREA_SIZE.

The dialog contains a checkbox for specifying that template adaption is needed. If the 
Enrollmet template has been created with learning allowed and this checkbox is 
checked, the Enrollment template could be modified to contain more recent fingerprint 
features.

When any fingerprint event happens, the dialog receives a message, (WMUS_FP_NOTIFY, which 
was previously passed to DPFPApi) and processes the parameters of the message. wParam contains 
the code for the event. All events are traced in the list box in the dialog. Receipt of a 
WN_COMPLETED event means that the fingerprint image has been captured. lParam contains a pointer 
to the image. The dialog displays the image using the helper function DisplayFingerprintImage and 
then compares it with the Enrolled fingerprint template using the Verify helper function. The 
Enrolled template has been passed to the dialog previously from the calling function, shortly 
after construction of the dialog.

The function DisplayFingerprintImage first prepares a regular bitmap header, then uses an SDK 
function to get the fingerprint image bits and finally, uses CreateDIBSection to put the bits into 
the bitmap. Then it sets the bitmap handle to the control in the dialog (IDC_STATIC_DRAWAREA) that 
displays the image.

The Verify function uses SDK functions to create a fingerprint feature set from the fingerprint Image, 
and then compares that feature set with the Enrolled template previously passed to the dialog from 
the outside.

The OnDestroy function closes all contexts and stops fingerprint image capture.

