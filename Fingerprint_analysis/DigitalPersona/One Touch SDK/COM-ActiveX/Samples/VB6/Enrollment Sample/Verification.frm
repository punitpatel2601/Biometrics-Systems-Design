VERSION 5.00
Begin VB.Form Verification 
   Caption         =   "Fingerprint Verification"
   ClientHeight    =   4290
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   7830
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4290
   ScaleWidth      =   7830
   StartUpPosition =   2  'CenterScreen
   Begin VB.PictureBox HiddenPict 
      AutoRedraw      =   -1  'True
      AutoSize        =   -1  'True
      Height          =   495
      Left            =   960
      ScaleHeight     =   435
      ScaleWidth      =   555
      TabIndex        =   8
      Top             =   3240
      Visible         =   0   'False
      Width           =   615
   End
   Begin VB.PictureBox Picture1 
      AutoRedraw      =   -1  'True
      Height          =   2775
      Left            =   120
      ScaleHeight     =   2715
      ScaleWidth      =   2715
      TabIndex        =   2
      Top             =   120
      Width           =   2775
   End
   Begin VB.CommandButton Close 
      Caption         =   "Close"
      Height          =   375
      Left            =   6480
      TabIndex        =   1
      Top             =   3720
      Width           =   1215
   End
   Begin VB.ListBox Status 
      Height          =   1815
      Left            =   3120
      TabIndex        =   0
      Top             =   960
      Width           =   4575
   End
   Begin VB.Label FAR 
      BorderStyle     =   1  'Fixed Single
      Height          =   375
      Left            =   5040
      TabIndex        =   7
      Top             =   3120
      Width           =   2655
   End
   Begin VB.Label Label3 
      Caption         =   "False Accept Rate:"
      Height          =   255
      Left            =   3120
      TabIndex        =   6
      Top             =   3120
      Width           =   1815
   End
   Begin VB.Label Label1 
      Caption         =   "Prompt:"
      Height          =   255
      Left            =   3120
      TabIndex        =   5
      Top             =   120
      Width           =   2655
   End
   Begin VB.Label Prompt 
      BorderStyle     =   1  'Fixed Single
      Caption         =   "Touch the fingerprint reader."
      Height          =   375
      Left            =   3120
      TabIndex        =   4
      Top             =   360
      Width           =   4575
   End
   Begin VB.Label Label2 
      Caption         =   "Status:"
      Height          =   255
      Left            =   3120
      TabIndex        =   3
      Top             =   720
      Width           =   2655
   End
End
Attribute VB_Name = "Verification"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim WithEvents Capture As DPFPCapture
Attribute Capture.VB_VarHelpID = -1
Dim CreateFtrs As DPFPFeatureExtraction
Dim Verify As DPFPVerification
Dim ConvertSample As DPFPSampleConversion

Private Sub ReportStatus(ByVal Str As String)
 ' Add string to list box.
 Status.AddItem (Str)
 ' Move list box selection down.
 Status.ListIndex = Status.NewIndex
End Sub

Private Sub DrawPicture(ByVal Pict As IPictureDisp)
 ' Must use hidden PictureBox to easily resize picture.
 Set HiddenPict.Picture = Pict
 Picture1.PaintPicture HiddenPict.Picture, _
       0, 0, Picture1.ScaleWidth, _
       Picture1.ScaleHeight, _
       0, 0, HiddenPict.ScaleWidth, _
       HiddenPict.ScaleHeight, vbSrcCopy
 Picture1.Picture = Picture1.Image
End Sub


Private Sub Close_Click()
 ' Stop capture operation. This code is optional.
 Capture.StopCapture
 ' Unload form.
 Unload Me
End Sub

Private Sub Form_Load()
 ' Create capture operation.
 Set Capture = New DPFPCapture
 ' Start capture operation.
 Capture.StartCapture
 ' Create DPFPFeatureExtraction object.
 Set CreateFtrs = New DPFPFeatureExtraction
 ' Create DPFPVerification object.
 Set Verify = New DPFPVerification
 ' Create DPFPSampleConversion object.
 Set ConvertSample = New DPFPSampleConversion
End Sub

Private Sub Capture_OnReaderConnect(ByVal ReaderSerNum As String)
 ReportStatus ("The fingerprint reader was connected.")
End Sub

Private Sub Capture_OnReaderDisconnect(ByVal ReaderSerNum As String)
 ReportStatus ("The fingerprint reader was disconnected.")
End Sub

Private Sub Capture_OnFingerTouch(ByVal ReaderSerNum As String)
 ReportStatus ("The fingerprint reader was touched.")
End Sub
Private Sub Capture_OnFingerGone(ByVal ReaderSerNum As String)
 ReportStatus ("The finger was removed from the fingerprint reader.")
End Sub
Private Sub Capture_OnSampleQuality(ByVal ReaderSerNum As String, ByVal Feadback As DPFPCaptureFeedbackEnum)
 If Feadback = CaptureFeedbackGood Then
  ReportStatus ("The quality of fingerprint sample is good.")
  Else
  ReportStatus ("The quality of fingerprint sample is poor.")
  End If
End Sub

Private Sub Capture_OnComplete(ByVal ReaderSerNum As String, ByVal Sample As Object)
 Dim Feadback As DPFPCaptureFeedbackEnum
 Dim Res As DPFPVerificationResult
 Dim Templ As Object
 ReportStatus ("The fingerprint was captured.")
 ' Draw fingerprint image.
 DrawPicture ConvertSample.ConvertToPicture(Sample)
 ' Process sample and create feature set for purpose of verification.
 Feedback = CreateFtrs.CreateFeatureSet(Sample, DataPurposeVerification)
 ' Quality of sample is not good enough to produce feature set.
 If Feadback = CaptureFeedbackGood Then
  Prompt.Caption = "Touch the fingerprint reader with a different finger."
  Set Templ = MainFrame.GetTemplate
  If Templ Is Nothing Then
   MsgBox "You must create a fingerprint template before you can perform verification."
  Else
    ' Compare feature set with template.
    Set Res = Verify.Verify(CreateFtrs.FeatureSet, Templ)
    ' Show results of comparison.
    FAR.Caption = Res.FARAchieved
    If Res.Verified = True Then
      ReportStatus ("The fingerprint was verified.")
    Else
      ReportStatus ("The fingerprint was not verified.")
    End If
  End If
 End If
 End Sub

