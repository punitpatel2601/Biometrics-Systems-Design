VERSION 5.00
Begin VB.Form MainForm 
   Caption         =   "VB Demo"
   ClientHeight    =   5655
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   4125
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5655
   ScaleWidth      =   4125
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton Close 
      Caption         =   "Quit"
      Height          =   375
      Left            =   1200
      TabIndex        =   15
      Top             =   5160
      Width           =   1695
   End
   Begin VB.Frame Frame1 
      Caption         =   "Verification"
      Height          =   2175
      Left            =   120
      TabIndex        =   6
      Top             =   2880
      Width           =   3855
      Begin VB.Frame Frame2 
         Caption         =   "Return Values"
         Height          =   1215
         Left            =   120
         TabIndex        =   8
         Top             =   360
         Width           =   3615
         Begin VB.CheckBox Verified 
            Caption         =   "Verified"
            Enabled         =   0   'False
            Height          =   255
            Left            =   1920
            TabIndex        =   10
            Top             =   360
            Width           =   255
         End
         Begin VB.Label FAR 
            BorderStyle     =   1  'Fixed Single
            Height          =   375
            Left            =   1920
            TabIndex        =   16
            Top             =   720
            Width           =   1575
         End
         Begin VB.Label Label4 
            Caption         =   "False Accept Rate"
            Height          =   255
            Left            =   240
            TabIndex        =   11
            Top             =   720
            Width           =   1575
         End
         Begin VB.Label Label3 
            Caption         =   "Feature set matched?"
            Height          =   255
            Left            =   240
            TabIndex        =   9
            Top             =   360
            Width           =   1575
         End
      End
      Begin VB.CommandButton Verify 
         Caption         =   "Verify Fingerprint"
         Enabled         =   0   'False
         Height          =   375
         Left            =   1080
         TabIndex        =   7
         Top             =   1680
         Width           =   1695
      End
   End
   Begin VB.Frame Enrollment 
      Caption         =   "Enrollment"
      Height          =   2655
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   3855
      Begin VB.Frame Frame3 
         Caption         =   "Event Handler Status"
         Height          =   735
         Left            =   0
         TabIndex        =   12
         Top             =   1320
         Width           =   3735
         Begin VB.OptionButton Failure 
            Caption         =   "Failure"
            Height          =   255
            Left            =   1920
            TabIndex        =   14
            Top             =   360
            Width           =   1575
         End
         Begin VB.OptionButton Success 
            Caption         =   "Success"
            Height          =   195
            Left            =   240
            TabIndex        =   13
            Top             =   360
            Width           =   1335
         End
      End
      Begin VB.CommandButton Enroll 
         Caption         =   "Enroll Fingerprints"
         Height          =   315
         Left            =   1035
         TabIndex        =   0
         Top             =   2160
         Width           =   1695
      End
      Begin VB.TextBox MaxCount 
         Height          =   285
         Left            =   2760
         TabIndex        =   5
         Text            =   "10"
         Top             =   840
         Width           =   975
      End
      Begin VB.TextBox Mask 
         Height          =   285
         Left            =   2760
         TabIndex        =   3
         Text            =   "0"
         Top             =   360
         Width           =   975
      End
      Begin VB.Label Label2 
         Caption         =   "Max. Enrolled Fingers Count"
         Height          =   375
         Left            =   120
         TabIndex        =   4
         Top             =   840
         Width           =   2295
      End
      Begin VB.Label Label1 
         Caption         =   "Fingerprint Mask"
         Height          =   255
         Left            =   120
         TabIndex        =   2
         Top             =   360
         Width           =   1455
      End
   End
End
Attribute VB_Name = "MainForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim Templates(0 To 10) As Object

Private Sub Close_Click()
 Unload Me
End Sub

Private Sub Enroll_Click()
 'Show enrollment dialog box.
 EnrollmentDialog.Show vbModal, Me
End Sub

Private Sub Form_Load()
    Success.Value = True
    Failure.Value = False
End Sub

Private Sub Verify_Click()
 'Show verification dialog box.
 VerificationDialog.Show vbModal, Me
End Sub

Public Function GetTemplate(ByVal Mask As Long) As Object
 If Templates(Mask) Is Nothing Then
 Else
   ' Get fingerprint template from the memory.
   Set GetTemplate = Templates(Mask)
 End If
End Function

Public Sub SetTemplate(ByVal Mask As Long, ByVal Templ As Object)
  ' Store fingerprint template in the memory.
  Set Templates(Mask) = Templ
End Sub
