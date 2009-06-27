VERSION 5.00
Begin VB.Form frmSettings 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Settings"
   ClientHeight    =   4965
   ClientLeft      =   5505
   ClientTop       =   3180
   ClientWidth     =   5115
   Icon            =   "Form2.frx":0000
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4965
   ScaleWidth      =   5115
   Begin VB.Frame Frame4 
      Caption         =   "Drive with mp3 files:"
      Height          =   735
      Left            =   360
      TabIndex        =   10
      Top             =   2040
      Width           =   2655
      Begin VB.DriveListBox Drive1 
         BackColor       =   &H8000000F&
         Height          =   315
         Left            =   120
         TabIndex        =   11
         ToolTipText     =   "Mp3 file drive. Do not select CD-rom drive!"
         Top             =   240
         Width           =   2415
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Parallel port address"
      Height          =   1455
      Left            =   360
      TabIndex        =   0
      Top             =   360
      Width           =   1695
      Begin VB.OptionButton Option2 
         Caption         =   "3BCH"
         Height          =   375
         Left            =   240
         TabIndex        =   4
         Top             =   600
         Width           =   975
      End
      Begin VB.OptionButton Option3 
         Caption         =   "278H"
         Height          =   375
         Left            =   240
         TabIndex        =   3
         Top             =   960
         Width           =   975
      End
      Begin VB.OptionButton Option1 
         Caption         =   "378H"
         Height          =   375
         Left            =   240
         TabIndex        =   2
         Top             =   240
         Width           =   975
      End
   End
   Begin VB.Frame Frame2 
      Caption         =   "MPMan size"
      Height          =   1455
      Left            =   3000
      TabIndex        =   1
      Top             =   360
      Width           =   1695
      Begin VB.OptionButton Option6 
         Caption         =   "64 Mb"
         Height          =   375
         Left            =   240
         TabIndex        =   7
         Top             =   960
         Width           =   975
      End
      Begin VB.OptionButton Option5 
         Caption         =   "32 Mb"
         Height          =   375
         Left            =   240
         TabIndex        =   6
         Top             =   600
         Width           =   975
      End
      Begin VB.OptionButton Option4 
         Caption         =   "16 Mb"
         Height          =   375
         Left            =   240
         TabIndex        =   5
         Top             =   240
         Width           =   975
      End
   End
   Begin VB.Frame Frame3 
      Caption         =   "Path to mp3 files:"
      Height          =   1575
      Left            =   360
      TabIndex        =   8
      Top             =   3000
      Width           =   4335
      Begin VB.DirListBox Dir1 
         BackColor       =   &H8000000F&
         Height          =   1215
         Left            =   120
         TabIndex        =   9
         ToolTipText     =   "Folder with mp3 files"
         Top             =   240
         Width           =   4095
      End
   End
End
Attribute VB_Name = "frmSettings"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim iFileNumber As Integer
Dim sLineOfText As String
Dim sSti As String
Dim Mpinfo(5) As String

Private Sub Dir1_Change()
Mpinfo(5) = Dir1.Path
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, Mpinfo(3)
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
End Sub

Private Sub Drive1_Change()
Dir1.Path = Drive1.Drive
Mpinfo(4) = Drive1.Drive
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, Mpinfo(3)
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
End Sub

Private Sub Form_Load()
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Input As #iFileNumber
  Line Input #iFileNumber, sLineOfText
  sLineOfText = Mid((sLineOfText), 6, 3)
  Mpinfo(1) = sLineOfText
  Line Input #iFileNumber, sLineOfText
  sLineOfText = Mid((sLineOfText), 7, 2)
  Mpinfo(2) = sLineOfText
  Line Input #iFileNumber, sLineOfText
  Mpinfo(3) = sLineOfText
  Line Input #iFileNumber, sLineOfText
  Mpinfo(4) = sLineOfText
  Line Input #iFileNumber, sLineOfText
  Mpinfo(5) = sLineOfText
Close #iFileNumber
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, "False"
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
Dir1.Path = Mpinfo(5)
Drive1.Drive = Mpinfo(4)
Select Case Mpinfo(1)
  Case "378"
    Option1.Value = True
  Case "3BC"
    Option2.Value = True
  Case "278"
    Option3.Value = True
End Select
Select Case Mpinfo(2)
  Case "16"
    Option4.Value = True
  Case "32"
    Option5.Value = True
  Case "64"
    Option6.Value = True
End Select
End Sub

Private Sub Form_Unload(Cancel As Integer)
frmSettings.Visible = False
frmMain.cmdRefresh_Click
End Sub

Private Sub Option1_Click()
Mpinfo(1) = "378"
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, Mpinfo(3)
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
End Sub

Private Sub Option2_Click()
Mpinfo(1) = "3BC"
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, Mpinfo(3)
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
End Sub

Private Sub Option3_Click()
Mpinfo(1) = "278"
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, Mpinfo(3)
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
End Sub

Private Sub Option4_Click()
Mpinfo(2) = "16"
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, Mpinfo(3)
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
End Sub

Private Sub Option5_Click()
Mpinfo(2) = "32"
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, Mpinfo(3)
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
End Sub

Private Sub Option6_Click()
Mpinfo(2) = "64"
iFileNumber = FreeFile
sSti = App.Path
Open (sSti & "\mpman.ini") For Output As #iFileNumber
  Print #iFileNumber, "port " + Mpinfo(1)
  Print #iFileNumber, "mpman " + Mpinfo(2)
  Print #iFileNumber, Mpinfo(3)
  Print #iFileNumber, Mpinfo(4)
  Print #iFileNumber, Mpinfo(5)
Close #iFileNumber
End Sub
