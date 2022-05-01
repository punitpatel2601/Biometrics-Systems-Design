VERSION 5.00
Object = "{B49E66FF-6927-4378-9685-937F14679ADD}#1.0#0"; "DPFPCtlX.dll"
Begin VB.Form VerificationDialog 
   Caption         =   "Verify Your Identity"
   ClientHeight    =   2085
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   4920
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2085
   ScaleWidth      =   4920
   StartUpPosition =   2  'CenterScreen
   Begin DPFPCtlXLibCtl.DPFPVerificationControl DPFPVerificationControl1 
      Height          =   735
      Left            =   3960
      TabIndex        =   2
      TabStop         =   0   'False
      Top             =   360
      Width           =   735
      _cx             =   1296
      _cy             =   1296
      ReaderSerialNumber=   "{00000000-0000-0000-0000-000000000000}"
   End
   Begin VB.CommandButton Close 
      Caption         =   "Close"
      Height          =   375
      Left            =   3360
      TabIndex        =   0
      Top             =   1560
      Width           =   1455
   End
   Begin VB.Label Label1 
      Caption         =   "To verify your identity, touch fingerprint reader with any enrolled finger."
      Height          =   735
      Left            =   240
      TabIndex        =   1
      Top             =   360
      Width           =   3375
   End
End
Attribute VB_Name = "VerificationDialog"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim Ver As DPFPVerification

Private Sub Form_Load()
 ' Create DPFPVerification object.
 Set Ver = New DPFPVerification
End Sub

Private Sub Close_Click()
 Unload Me
End Sub

Private Sub DPFPVerificationControl1_OnComplete(ByVal Ftrs As Object, ByVal Stat As Object)
 Dim Res As Object
 Dim Templ As Object
 ' Compare feature set with all stored templates.
 For i = 0 To 10
    ' Get template from storage.
    Set Templ = MainForm.GetTemplate(i)
    If Templ Is Nothing Then
    Else
        ' Compare feature set with particular template.
        Set Res = Ver.Verify(Ftrs, Templ)
        ' If match, exit from loop.
        If Res.Verified = True Then Exit For
    End If
 Next
 If Res Is Nothing Then
  Stat.Status = EventHandlerStatusFailure
  Exit Sub
 ElseIf Res.Verified = False Then
  ' If non-match, notify caller.
  Stat.Status = EventHandlerStatusFailure
  MainForm.Verified.Value = 0
 Else
  MainForm.Verified.Value = 1
 End If
 ' Show false accept rate.
 MainForm.FAR.Caption = Res.FARAchieved
 End Sub



