VERSION 5.00
Object = "{B49E66FF-6927-4378-9685-937F14679ADD}#1.0#0"; "DPFPCtlX.dll"
Begin VB.Form EnrollmentDialog 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Fingerprint Enrollment"
   ClientHeight    =   5250
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   7440
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5250
   ScaleWidth      =   7440
   ShowInTaskbar   =   0   'False
   StartUpPosition =   2  'CenterScreen
   Begin DPFPCtlXLibCtl.DPFPEnrollmentControl DPFPEnrollmentControl1 
      Height          =   4695
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   7455
      _cx             =   13150
      _cy             =   8281
      MaxEnrollFingerCount=   10
      EnrolledFingersMask=   0
      ReaderSerialNumber=   "{00000000-0000-0000-0000-000000000000}"
   End
   Begin VB.CommandButton Close 
      Caption         =   "Close"
      Height          =   375
      Left            =   5760
      TabIndex        =   0
      Top             =   4800
      Width           =   1455
   End
End
Attribute VB_Name = "EnrollmentDialog"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Close_Click()
 Unload Me
End Sub

Private Sub Form_Activate()
 ' Set properties to DPFPEnrollment object.
 On Error Resume Next
 EnrollmentDialog.DPFPEnrollmentControl1.MaxEnrollFingerCount = MainForm.MaxCount
 If Err.Number <> 0 Then
    MsgBox "Wrong parameter MaxEnrollFingerCount: " + Err.Description
    MainForm.MaxCount = 10
    Unload Me
 Else
    EnrollmentDialog.DPFPEnrollmentControl1.EnrolledFingersMask = MainForm.Mask
    If Err.Number <> 0 Then
       MsgBox "Wrong parameter EnrolledFingersMask: " + Err.Description
       MainForm.Mask = 0
       Unload Me
    End If
 End If
End Sub


Private Sub Form_Unload(Cancel As Integer)
 ' Show new fingerprint mask.
 MainForm.Mask = EnrollmentDialog.DPFPEnrollmentControl1.EnrolledFingersMask
 If EnrollmentDialog.DPFPEnrollmentControl1.EnrolledFingersMask > 0 Then
  MainForm.Verify.Enabled = True
 Else
  MainForm.Verify.Enabled = False
 End If
End Sub

Private Sub DPFPEnrollmentControl1_OnEnroll(ByVal Mask As Long, ByVal Templ As Object, ByVal Stat As Object)
   If MainForm.Failure.Value = True Then
    ' Emulate failure of enrollment operation.
    Stat.Status = EventHandlerStatusFailure
   Else
    ' Enrollment succeeded, so store the template.
    MainForm.SetTemplate Mask, Templ
   End If
End Sub

Private Sub DPFPEnrollmentControl1_OnDelete(ByVal Mask As Long, ByVal Stat As Object)
   If MainForm.Failure.Value = True Then
    ' Emulate failure of deletion operation.
    Stat.Status = EventHandlerStatusFailure
   Else
    ' Deletion succeeded, so remove the template from storage.
    MainForm.SetTemplate Mask, Nothing
   End If
End Sub




