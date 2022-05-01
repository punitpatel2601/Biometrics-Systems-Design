VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form MainFrame 
   Caption         =   "Fingerprint Enrollment and Verification Sample"
   ClientHeight    =   2985
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   5265
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2985
   ScaleWidth      =   5265
   StartUpPosition =   2  'CenterScreen
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   840
      Top             =   2400
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton Close 
      Caption         =   "Close"
      Height          =   375
      Left            =   2640
      TabIndex        =   4
      Top             =   2520
      Width           =   1335
   End
   Begin VB.CommandButton ReadTemplate 
      Caption         =   "Read Fingerprint Template"
      Height          =   375
      Left            =   240
      TabIndex        =   3
      Top             =   1920
      Width           =   3735
   End
   Begin VB.CommandButton SaveTemplate 
      Caption         =   "Save Fingerprint Template"
      Height          =   375
      Left            =   240
      TabIndex        =   2
      Top             =   1320
      Width           =   3735
   End
   Begin VB.CommandButton Verify 
      Caption         =   "Fingerprint Verification"
      Height          =   375
      Left            =   240
      TabIndex        =   1
      Top             =   720
      Width           =   3735
   End
   Begin VB.CommandButton Enroll 
      Caption         =   "Fingerprint Enrollment"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   3735
   End
End
Attribute VB_Name = "MainFrame"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim Templ As DPFPTemplate

Private Sub Close_Click()
 ' Unload form.
 Unload Me
End Sub

Private Sub Enroll_Click()
 ' Start enrollment.
 Enrollment.Show vbModal, Me
End Sub

Public Function GetTemplate() As Object
 ' Template can be empty. If so, then returns Nothing.
 If Templ Is Nothing Then
 Else
  Set GetTemplate = Templ
 End If
End Function

Public Sub SetTemplete(ByVal Template As Object)
 Set Templ = Template
End Sub

Private Sub ReadTemplate_Click()
 Dim blob() As Byte
 CommonDialog1.Filter = "Fingerprint Template File|*.fpt"
 ' Set dialog box so an error occurs if dialog box is cancelled.
 CommonDialog1.CancelError = True
 On Error Resume Next
 ' Show Open dialog box.
 CommonDialog1.ShowOpen
 If Err Then
    ' This code runs if dialog box was cancelled.
    Exit Sub
 End If
 ' Read binary data from file.
 Open CommonDialog1.FileName For Binary As #1
 ReDim blob(LOF(1))
 Get #1, , blob()
 Close #1
 ' Template can be empty, it must be created first.
 If Templ Is Nothing Then Set Templ = New DPFPTemplate
 ' Import binary data to template.
 Templ.Deserialize blob
End Sub

Private Sub SaveTemplate_Click()
 Dim blob() As Byte
 ' First verify that template is not empty.
 If Templ Is Nothing Then
  MsgBox "You must create a fingerprint template before you can save it."
  Exit Sub
 End If
 CommonDialog1.Filter = "Fingerprint Template File|*.fpt"
 ' Set dialog box so an error occurs if dialog box is cancelled.
 CommonDialog1.CancelError = True
 On Error Resume Next
 ' Show Save As dialog box.
 CommonDialog1.ShowSave
 If Err Then
    ' This code runs if the dialog box was cancelled.
    Exit Sub
 End If
 ' Export template to binary data.
 blob = Templ.Serialize
 ' Save binary data to file.
 Open CommonDialog1.FileName For Binary As #1
 Put #1, , blob
 Close #1
End Sub

Private Sub Verify_Click()
 ' First verify that template is not empty.
 If Templ Is Nothing Then
  MsgBox "You must create fingerprint template before you can perform verification."
  Exit Sub
 End If
 ' Show Fingerprint Verification dialog box.
 Verification.Show vbModal, Me
End Sub
