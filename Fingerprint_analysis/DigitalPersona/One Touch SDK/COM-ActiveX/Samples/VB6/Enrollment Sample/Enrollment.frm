VERSION 5.00
Begin VB.Form Enrollment 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Fingerprint Enrollment"
   ClientHeight    =   5295
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   7815
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5295
   ScaleWidth      =   7815
   StartUpPosition =   2  'CenterScreen
   Begin VB.PictureBox HiddenPict 
      AutoRedraw      =   -1  'True
      AutoSize        =   -1  'True
      Height          =   495
      Left            =   720
      ScaleHeight     =   435
      ScaleWidth      =   555
      TabIndex        =   8
      Top             =   3240
      Visible         =   0   'False
      Width           =   615
   End
   Begin VB.ListBox Status 
      Height          =   3375
      Left            =   3240
      TabIndex        =   6
      Top             =   1080
      Width           =   4335
   End
   Begin VB.CommandButton Close 
      Caption         =   "Close"
      Height          =   375
      Left            =   6360
      TabIndex        =   4
      Top             =   4680
      Width           =   1215
   End
   Begin VB.PictureBox Picture1 
      AutoRedraw      =   -1  'True
      Height          =   2775
      Left            =   240
      ScaleHeight     =   2715
      ScaleWidth      =   2715
      TabIndex        =   0
      Top             =   240
      Width           =   2775
   End
   Begin VB.Label Samples 
      BorderStyle     =   1  'Fixed Single
      Height          =   375
      Left            =   2640
      TabIndex        =   7
      Top             =   4680
      Width           =   615
   End
   Begin VB.Label Label3 
      Caption         =   "Fingerprint samples needed:"
      Height          =   375
      Left            =   240
      TabIndex        =   5
      Top             =   4680
      Width           =   2175
   End
   Begin VB.Label Label2 
      Caption         =   "Status:"
      Height          =   255
      Left            =   3240
      TabIndex        =   3
      Top             =   840
      Width           =   2655
   End
   Begin VB.Label Prompt 
      BorderStyle     =   1  'Fixed Single
      Caption         =   "Touch the fingerprint reader."
      Height          =   255
      Left            =   3240
      TabIndex        =   2
      Top             =   480
      Width           =   4335
   End
   Begin VB.Label Label1 
      Caption         =   "Prompt:"
      Height          =   255
      Left            =   3240
      TabIndex        =   1
      Top             =   240
      Width           =   2655
   End
End
Attribute VB_Name = "Enrollment"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim WithEvents Capture As DPFPCapture
Attribute Capture.VB_VarHelpID = -1
Dim CreateFtrs As DPFPFeatureExtraction
Dim CreateTempl As DPFPEnrollment
Dim ConvertSample As DPFPSampleConversion

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
Private Sub ReportStatus(ByVal Str As String)
 ' Add string to list box.
 Status.AddItem (Str)
 ' Move list box selection down.
 Status.ListIndex = Status.NewIndex
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
 ' Create DPFPEnrollment object.
 Set CreateTempl = New DPFPEnrollment
 ' Show number of samples needed.
 Samples.Caption = CreateTempl.FeaturesNeeded
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
Private Sub Capture_OnSampleQuality(ByVal ReaderSerNum As String, ByVal Feedback As DPFPCaptureFeedbackEnum)
 If Feedback = CaptureFeedbackGood Then
  ReportStatus ("The quality of the fingerprint sample is good.")
  Else
  ReportStatus ("The quality of the fingerprint sample is poor.")
  End If
End Sub

Private Sub Capture_OnComplete(ByVal ReaderSerNum As String, ByVal Sample As Object)
 Dim Feedback As DPFPCaptureFeedbackEnum
 
 ReportStatus ("The fingerprint sample was captured.")
 ' Draw fingerprint image.
 DrawPicture ConvertSample.ConvertToPicture(Sample)
 ' Process sample and create feature set for purpose of enrollment.
 Feedback = CreateFtrs.CreateFeatureSet(Sample, DataPurposeEnrollment)
 ' Quality of sample is not good enough to produce feature set.
 If Feedback = CaptureFeedbackGood Then
  ReportStatus ("The fingerprint feature set was created.")
  Prompt.Caption = "Touch the fingerprint reader again with the same finger."
  ' Add feature set to template.
  CreateTempl.AddFeatures CreateFtrs.FeatureSet
  ' Show number of samples needed to complete template.
  Samples.Caption = CreateTempl.FeaturesNeeded
  ' Check if template has been created.
  If CreateTempl.TemplateStatus = TemplateStatusTemplateReady Then
    MainFrame.SetTemplete CreateTempl.Template
    ' Template has been created, so stop capturing samples.
    Capture.StopCapture
    Prompt.Caption = "Click Close, and then click Fingerprint Verification."
    MsgBox "The fingerprint template was created."
  End If
 End If
 End Sub

