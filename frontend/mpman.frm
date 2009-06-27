VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmMain 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "MPMan frontend 0.23"
   ClientHeight    =   5325
   ClientLeft      =   4320
   ClientTop       =   3165
   ClientWidth     =   7230
   Icon            =   "mpman.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   5325
   ScaleWidth      =   7230
   Begin VB.CommandButton cmdKill 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   6240
      TabIndex        =   19
      ToolTipText     =   "Cancel upload / download"
      Top             =   4680
      Width           =   735
   End
   Begin VB.CommandButton cmdRemove 
      DisabledPicture =   "mpman.frx":030A
      DownPicture     =   "mpman.frx":0454
      Height          =   375
      Left            =   2400
      Picture         =   "mpman.frx":059E
      Style           =   1  'Graphical
      TabIndex        =   7
      ToolTipText     =   "Delete selected file(s) in MPMan"
      Top             =   120
      Width           =   375
   End
   Begin VB.CommandButton cmdMoveFileDown 
      DisabledPicture =   "mpman.frx":06E8
      DownPicture     =   "mpman.frx":0832
      Height          =   375
      Left            =   2040
      Picture         =   "mpman.frx":097C
      Style           =   1  'Graphical
      TabIndex        =   6
      ToolTipText     =   "Move selected file one position down in MPMan"
      Top             =   120
      Width           =   375
   End
   Begin VB.CommandButton cmdMoveFileUp 
      DisabledPicture =   "mpman.frx":0AC6
      DownPicture     =   "mpman.frx":0C10
      Height          =   375
      Left            =   1680
      Picture         =   "mpman.frx":0D5A
      Style           =   1  'Graphical
      TabIndex        =   5
      ToolTipText     =   "Move selected file one position up in MPMan"
      Top             =   120
      Width           =   375
   End
   Begin VB.CommandButton cmdDownload 
      DisabledPicture =   "mpman.frx":0EA4
      DownPicture     =   "mpman.frx":0FEE
      Height          =   375
      Left            =   1320
      Picture         =   "mpman.frx":1138
      Style           =   1  'Graphical
      TabIndex        =   4
      ToolTipText     =   "Download selected file(s) from MPMan"
      Top             =   120
      Width           =   375
   End
   Begin VB.Frame Frame3 
      Caption         =   "MPMan content:"
      Height          =   1695
      Left            =   240
      TabIndex        =   12
      Top             =   720
      Width           =   6735
      Begin MSComctlLib.ListView ListView1 
         Height          =   1335
         Left            =   120
         TabIndex        =   8
         Top             =   240
         Width           =   6495
         _ExtentX        =   11456
         _ExtentY        =   2355
         LabelEdit       =   1
         Sorted          =   -1  'True
         MultiSelect     =   -1  'True
         LabelWrap       =   -1  'True
         HideSelection   =   0   'False
         FullRowSelect   =   -1  'True
         _Version        =   393217
         ForeColor       =   -2147483640
         BackColor       =   -2147483633
         BorderStyle     =   1
         Appearance      =   1
         NumItems        =   0
      End
   End
   Begin VB.CommandButton cmdUpload 
      DisabledPicture =   "mpman.frx":1282
      DownPicture     =   "mpman.frx":13CC
      Height          =   375
      Left            =   960
      Picture         =   "mpman.frx":1516
      Style           =   1  'Graphical
      TabIndex        =   3
      ToolTipText     =   "Upload selected file(s) to MPMan"
      Top             =   120
      Width           =   375
   End
   Begin VB.CommandButton cmdInit 
      DisabledPicture =   "mpman.frx":1660
      DownPicture     =   "mpman.frx":17AA
      Height          =   375
      Left            =   600
      Picture         =   "mpman.frx":18F4
      Style           =   1  'Graphical
      TabIndex        =   2
      ToolTipText     =   "Initialize MPMan"
      Top             =   120
      Width           =   375
   End
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   1000
      Left            =   3960
      Top             =   3240
   End
   Begin VB.CommandButton cmdRefresh 
      DisabledPicture =   "mpman.frx":1A3E
      DownPicture     =   "mpman.frx":1B88
      Height          =   375
      Left            =   240
      Picture         =   "mpman.frx":1CD2
      Style           =   1  'Graphical
      TabIndex        =   1
      ToolTipText     =   "Refresh info from MPMan and frontend program directory"
      Top             =   120
      Width           =   375
   End
   Begin VB.Frame Frame1 
      Caption         =   "MPMan info:"
      Height          =   615
      Left            =   3000
      TabIndex        =   10
      ToolTipText     =   "MPMan size, used space and free space"
      Top             =   0
      Width           =   3975
      Begin VB.ListBox List2 
         BackColor       =   &H8000000F&
         Height          =   255
         Left            =   120
         TabIndex        =   0
         ToolTipText     =   "MPMan size, used space and free space"
         Top             =   240
         Width           =   3735
      End
   End
   Begin VB.Frame Frame2 
      Height          =   1695
      Left            =   240
      TabIndex        =   11
      ToolTipText     =   "Change ""path to mp3 files"" in Settings menu"
      Top             =   2640
      Width           =   6735
      Begin VB.DriveListBox Drive1 
         Height          =   315
         Left            =   1080
         TabIndex        =   15
         Top             =   480
         Visible         =   0   'False
         Width           =   1215
      End
      Begin VB.DirListBox Dir1 
         Height          =   315
         Left            =   1080
         TabIndex        =   14
         Top             =   840
         Visible         =   0   'False
         Width           =   1215
      End
      Begin VB.FileListBox File1 
         Height          =   480
         Left            =   2400
         TabIndex        =   13
         Top             =   480
         Visible         =   0   'False
         Width           =   1215
      End
      Begin MSComctlLib.ListView ListView2 
         Height          =   1335
         Left            =   120
         TabIndex        =   9
         ToolTipText     =   "Change ""path to mp3 files"" in Settings menu"
         Top             =   240
         Width           =   6495
         _ExtentX        =   11456
         _ExtentY        =   2355
         LabelEdit       =   1
         Sorted          =   -1  'True
         MultiSelect     =   -1  'True
         LabelWrap       =   -1  'True
         HideSelection   =   0   'False
         FullRowSelect   =   -1  'True
         _Version        =   393217
         ForeColor       =   -2147483640
         BackColor       =   -2147483633
         BorderStyle     =   1
         Appearance      =   1
         NumItems        =   0
      End
   End
   Begin MSComctlLib.ProgressBar ProgressBar1 
      Height          =   255
      Left            =   240
      TabIndex        =   18
      Top             =   4800
      Width           =   5295
      _ExtentX        =   9340
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
   End
   Begin VB.Label Label3 
      Appearance      =   0  'Flat
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   240
      TabIndex        =   17
      Top             =   4440
      Width           =   6735
   End
   Begin VB.Label Label1 
      Height          =   255
      Left            =   5640
      TabIndex        =   16
      Top             =   4800
      Width           =   495
   End
   Begin VB.Menu mnuFileItem 
      Caption         =   "&File"
      Begin VB.Menu mnuRefreshItem 
         Caption         =   "&Refresh"
      End
      Begin VB.Menu mnuUploadItem 
         Caption         =   "&Upload"
      End
      Begin VB.Menu mnuMoveUpItem 
         Caption         =   "&Move File Up"
      End
      Begin VB.Menu mnuDownloadItem 
         Caption         =   "&Download"
      End
      Begin VB.Menu mnuMoveDownItem 
         Caption         =   "&Move File Down"
      End
      Begin VB.Menu mnuRemoveItem 
         Caption         =   "&Delete"
      End
      Begin VB.Menu mnuIntializeItem 
         Caption         =   "&Initialize"
      End
      Begin VB.Menu mnuExitItem 
         Caption         =   "&Exit"
      End
   End
   Begin VB.Menu mnuSettings 
      Caption         =   "&Settings"
      Begin VB.Menu mnuSettingItem 
         Caption         =   "&Settings"
      End
   End
   Begin VB.Menu mnuAbout 
      Caption         =   "&Help"
      Begin VB.Menu mnuAboutItem 
         Caption         =   "&About"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim Timer1Done, SortOrder2, bInitDone, bAbort As Boolean
Dim sPort, sMpManSize, sMP3Path As String
Dim Mpinfo(64, 5) As String
Public Sub cmdRefresh_Click() 'Refresh
Dim sLineOfText As String
Dim iFileNumber As Integer
    Call subDisableMenu
    Call subMPinfo
    iFileNumber = FreeFile
    Open (sMP3Path & "\mpman.bat") For Output As #iFileNumber
        Print #iFileNumber, Mid(sMP3Path, 1, 2)
        Print #iFileNumber, "CD " + sMP3Path
        Print #iFileNumber, "if not exist mpmb.exe" + " " + "copy " + """" + App.Path + "\mpmb.exe" + """" + " " + "mpmb.exe"
        Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -l >status.txt"
    Close #iFileNumber
    glPid = Shell((sMP3Path & "\mpman.bat"), 0)
    Call funcDelay(5)
    Call subDisp
End Sub

Private Sub cmdInit_Click() 'Initialize
Dim iFileNumber, iStyle, iI, iResponse As Integer
Dim sMsg, sTitle, sLineOfText As String
    Call subDisableMenu
    If ListView1.ListItems.Count > 0 Then
        sMsg = "Initialize MPMan?"   ' Define message.
        iStyle = vbYesNo + vbExclamation + vbDefaultButton2   ' Define buttons.
        sTitle = "Initialize confirmation"   ' Define title.
        iResponse = MsgBox(sMsg, iStyle, sTitle)
        If iResponse = vbYes Then   ' User chose Yes.
        Call subMPinfo
            iFileNumber = FreeFile
            Open (sMP3Path & "\mpman.bat") For Output As #iFileNumber
                Print #iFileNumber, Mid(sMP3Path, 1, 2)
                Print #iFileNumber, "cd " + sMP3Path
                Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -i"
                Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -l >status.txt"
            Close #iFileNumber
            glPid = Shell((sMP3Path + "\mpman.bat"), 0)
            Label3.Caption = "Initializing MPMan"
            Call funcDelay(5)
            Call subDisp
            Label3.Caption = ""
        Else   ' User chose No.
            Call subEnableMenu
        End If
    End If
    Call subEnableMenu
End Sub

Private Sub cmdUpload_Click() 'Upload
Dim sMsg, sTitle, sText, sLineOfText, sFilename As String
Dim iStyle, iResponse, iFileNumber, iA, iB As Integer
Dim itmFound As ListItem
Dim lFileSize As Long
Dim lFreeSpace As Long
    Call subDisableMenu
    If ListView2.ListItems.Count > 0 And ListView1.ListItems.Count < 64 _
    And (Mpinfo(1, 1) <> "") Then
        sMsg = "Upload selected file(s) to MPMan?" ' Define message.
        iStyle = vbYesNo + vbInformation + vbDefaultButton2   ' Define buttons.
        sTitle = "Upload confirmation"   ' Define title.
        iResponse = MsgBox(sMsg, iStyle, sTitle)
        If iResponse = vbYes Then   ' User chose Yes.
            Call subMPinfo
            lFileSize = 0
            For iA = 1 To ListView2.ListItems.Count
                If (ListView2.ListItems.Item(iA).Selected = True) Then
                    sFilename = ListView2.ListItems.Item(iA).ListSubItems(1)
                    sFilename = Replace(sFilename, ".Mp3", ".mp3")
                    sFilename = Replace(sFilename, ".mP3", ".mp3")
                    lFileSize = lFileSize + Int((CStr(ListView2.ListItems.Item(iA).ListSubItems(2)) / 32768) + 1)
                    lFreeSpace = (CStr(Mid(Mpinfo(3, 1), 8, (Len(Mpinfo(3, 1)) - 9))) / 32)
                    If ListView1.ListItems.Count > 0 Then
                        Set itmFound = ListView1.FindItem(sFilename, 1, 1, 1)
                    Else
                        Set itmFound = Nothing
                    End If
                    If (lFreeSpace > lFileSize) And (itmFound Is Nothing) Then
                        iFileNumber = FreeFile
                        Open (sMP3Path & "\mpman.bat") For Output As #iFileNumber
                            Print #iFileNumber, Mid(sMP3Path, 1, 2)
                            Print #iFileNumber, "cd " + sMP3Path
                            Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -u " + """" + sFilename + """" + " >load.txt"
                            Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -l >status.txt"
                        Close #iFileNumber
                        glPid = Shell((sMP3Path + "\mpman.bat"), 0)
                        Label3.Caption = "Uploading: " + sFilename
                        Call funcDelay(5)
                        iB = 0
                        ProgressBar1.Min = 1
                        ProgressBar1.Max = 100
                        ProgressBar1.Value = 1
                        cmdKill.Visible = True
                        While Not iB >= ProgressBar1.Max
                            iFileNumber = FreeFile
                            Open (sMP3Path & "\load.txt") For Input As #iFileNumber
                                Do Until EOF(iFileNumber)
                                    Line Input #iFileNumber, sLineOfText
                                    iB = Mid(sLineOfText, 16)
                                    If bAbort = True Then
                                        ProgressBar1.Value = 1
                                        Label3.Caption = ""
                                        Label1.Caption = ""
                                        bAbort = False
                                        Call subDisp
                                        Exit Sub
                                    End If
                                    DoEvents
                                Loop
                            Close #iFileNumber
                            ProgressBar1.Value = iB
                            Label1.Caption = CStr(iB) + " %"
                            Call funcDelay(1)
                        Wend
                        ProgressBar1.Value = 1
                        Label3.Caption = ""
                        Label1.Caption = ""
                        cmdKill.Visible = False
                        Call funcDelay(5)
                    End If
                End If
            Next
        Else
            Call subEnableMenu
        End If
    Call subDisp
    End If
    Call subEnableMenu
End Sub

Private Sub cmdDownload_Click() 'Download
Dim sMsg, sTitle, sText, sLineOfText, sFilename As String
Dim iStyle, iResponse, iFileNumber, iA, iB As Integer
    Call subDisableMenu
    If ListView1.ListItems.Count > 0 Then
        sMsg = "Download selected file(s) from MPMan?" ' Define message.
        iStyle = vbYesNo + vbInformation + vbDefaultButton2     ' Define buttons.
        sTitle = "Download confirmation"   ' Define title.
        iResponse = MsgBox(sMsg, iStyle, sTitle)
        If iResponse = vbYes Then   ' User chose Yes.
            Call subMPinfo
            For iA = 1 To ListView1.ListItems.Count
                If (ListView1.ListItems.Item(iA).Selected = True) Then
                    sFilename = ListView1.ListItems.Item(iA).ListSubItems(1)
                    iFileNumber = FreeFile
                    Open (sMP3Path & "\mpman.bat") For Output As #iFileNumber
                        Print #iFileNumber, Mid(sMP3Path, 1, 2)
                        Print #iFileNumber, "cd " + sMP3Path
                        Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -d " + """" + sFilename + """" + " >load.txt"
                        Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -l >status.txt"
                    Close #iFileNumber
                    glPid = Shell((sMP3Path + "\mpman.bat"), 0)
                    Label3.Caption = "Downloading: " + sFilename
                    Call funcDelay(5)
                    iB = 0
                    ProgressBar1.Min = 1
                    ProgressBar1.Max = 100
                    ProgressBar1.Value = 1
                    cmdKill.Visible = True
                    While Not iB >= ProgressBar1.Max
                        iFileNumber = FreeFile
                        Open (sMP3Path & "\load.txt") For Input As #iFileNumber
                            Do Until EOF(iFileNumber)
                                Line Input #iFileNumber, sLineOfText
                                iB = Mid(sLineOfText, 18)
                                If bAbort = True Then
                                    ProgressBar1.Value = 1
                                    Label3.Caption = ""
                                    Label1.Caption = ""
                                    bAbort = False
                                    Kill (sMP3Path & "\" & sFilename)
                                    Call subDisp
                                    Exit Sub
                                End If
                                DoEvents
                            Loop
                        Close #iFileNumber
                        ProgressBar1.Value = iB
                        Label1.Caption = CStr(iB) + " %"
                        Call funcDelay(1)
                    Wend
                    ProgressBar1.Value = 1
                    Label3.Caption = ""
                    Label1.Caption = ""
                    cmdKill.Visible = False
                    Call funcDelay(5)
                Else
                End If
            Next
            Call subDisp
        Else
        End If
    End If
    Call subEnableMenu
End Sub

Private Sub cmdRemove_Click() 'Remove
Dim sMsg, sTitle, sText, sLineOfText, sFilename As String
Dim iStyle, iResponse, iFileNumber, iA, iB As Integer
    Call subDisableMenu
    If ListView1.ListItems.Count > 0 Then
        sFilename = ListView1.SelectedItem.ListSubItems(1)
        sMsg = "Delete selected file(s) in MPMan?"  ' Define message.
        iStyle = vbYesNo + vbExclamation + vbDefaultButton2   ' Define buttons.
        sTitle = "Delete confirmation"   ' Define title.
        iResponse = MsgBox(sMsg, iStyle, sTitle)
        If iResponse = vbYes Then   ' User chose Yes.
            Call subMPinfo
            For iA = 1 To ListView1.ListItems.Count
                If (ListView1.ListItems.Item(iA).Selected = True) Then
                    sFilename = ListView1.ListItems.Item(iA).ListSubItems(1)
                    iFileNumber = FreeFile
                    Open (sMP3Path & "\mpman.bat") For Output As #iFileNumber
                        Print #iFileNumber, Mid(sMP3Path, 1, 2)
                        Print #iFileNumber, "cd " + sMP3Path
                        Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -r " + """" + sFilename + """" + " >load.txt"
                        Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -l >status.txt"
                    Close #iFileNumber
                    glPid = Shell((sMP3Path + "\mpman.bat"), 0)
                    Label3.Caption = "Deleting: " + sFilename
                    Call funcDelay(5)
                End If
            Next
            Call cmdRefresh_Click
            Call subDisp
            Label3.Caption = ""
        Else
        End If
    End If
    Call subEnableMenu
End Sub

Private Sub Form_Activate()
    If bInitDone = False Then
        cmdKill.Visible = False
        bAbort = False
        Call cmdRefresh_Click
        Call subDisp
        bInitDone = True
    End If
End Sub

Private Sub Form_Load()
Dim iFileNumber As Integer
Dim sLineOfText, sSti, MPstatusinfo(5) As String
    bInitDone = False
    SortOrder2 = 0
    iFileNumber = FreeFile
    sSti = App.Path
    Open (sSti & "\mpman.ini") For Input As #iFileNumber
      Line Input #iFileNumber, sLineOfText
      sLineOfText = Mid((sLineOfText), 6, 3)
      MPstatusinfo(1) = sLineOfText
      Line Input #iFileNumber, sLineOfText
      sLineOfText = Mid((sLineOfText), 7, 2)
      MPstatusinfo(2) = sLineOfText
      Line Input #iFileNumber, sLineOfText
      MPstatusinfo(3) = sLineOfText
      If MPstatusinfo(3) = "True" Then
        MPstatusinfo(4) = Mid(App.Path, 1, 2)
        MPstatusinfo(5) = App.Path
      Else
        Line Input #iFileNumber, sLineOfText
        MPstatusinfo(4) = sLineOfText
        Line Input #iFileNumber, sLineOfText
        MPstatusinfo(5) = sLineOfText
      End If
    Close #iFileNumber
    iFileNumber = FreeFile
    sSti = App.Path
    Open (sSti & "\mpman.ini") For Output As #iFileNumber
      Print #iFileNumber, "port " + MPstatusinfo(1)
      Print #iFileNumber, "mpman " + MPstatusinfo(2)
      Print #iFileNumber, "False"
      Print #iFileNumber, MPstatusinfo(4)
      Print #iFileNumber, MPstatusinfo(5)
    Close #iFileNumber

End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call cmdKill_Click
    End
End Sub

Private Sub mnuAboutItem_Click()
    frmAbout.Show vbModal, Me
End Sub

Private Sub mnuDownloadItem_Click()
    Call cmdDownload_Click
End Sub

Private Sub mnuExitItem_Click()
    End
End Sub

Private Sub mnuIntializeItem_Click()
    Call cmdInit_Click
End Sub

Private Sub mnuMoveDownItem_Click()
    Call cmdMoveFileDown_Click
End Sub

Private Sub mnuMoveUpItem_Click()
    Call cmdMoveFileUp_Click
End Sub

Private Sub mnuRefreshItem_Click()
    Call cmdRefresh_Click
End Sub

Private Sub mnuRemoveItem_Click()
    Call cmdRemove_Click
End Sub

Private Sub mnuSettingItem_Click()
    frmSettings.Show vbModal, Me
End Sub

Private Sub mnuUploadItem_Click()
    Call cmdUpload_Click
End Sub

Private Sub Timer1_Timer()
    Timer1Done = True
End Sub

Private Sub subDisp()
Dim sWrap, sLineOfText, FileDate As String
Dim iFileNumber, iAntLinier, iA, iB As Integer
Dim itmX, itmY As ListItem
    sWrap = Chr(13) & Chr(10)
    ListView2.ListItems.Clear
    ListView2.ColumnHeaders.Clear
    ListView2.ColumnHeaders.Add , , "", 0
    ListView2.ColumnHeaders.Add , , "File", ListView2.Width / (18 / 12)
    ListView2.ColumnHeaders.Add , , "Size", ListView2.Width / (18 / 2.5)
    ListView2.ColumnHeaders.Add , , "Date", ListView2.Width / (18 / 2.5)
    ListView2.View = lvwReport
    File1.Path = sMP3Path
    File1.FileName = "*.mp3"
    File1.Refresh
    For iA = 0 To File1.ListCount - 1
        FileDate = Mid(FileDateTime(sMP3Path & "\" & File1.List(iA)), 9, 2) + _
        "/" + Mid(FileDateTime(sMP3Path & "\" & File1.List(iA)), 4, 2) + _
        "/" + Mid(FileDateTime(sMP3Path & "\" & File1.List(iA)), 1, 2)
        Set itmY = ListView2.ListItems.Add()
            itmY.SubItems(1) = File1.List(iA)
            itmY.SubItems(2) = FileLen(sMP3Path & "\" & File1.List(iA))
            itmY.SubItems(3) = FileDate
    Next
    iFileNumber = FreeFile
    For iA = 1 To 3
        Mpinfo(iA, 1) = ""
    Next iA
    Open (sMP3Path & "\status.txt") For Input As #iFileNumber
        iAntLinier = 0
        Do Until EOF(iFileNumber)
            Line Input #iFileNumber, sLineOfText
            iAntLinier = iAntLinier + 1
            Mpinfo(iAntLinier, 1) = sLineOfText
        Loop
    Close #iFileNumber
    List2.Clear
    If iAntLinier = 0 Then
        List2.AddItem ("Error! Can't find MPMan. Check Settings menu")
    Else
        List2.AddItem ((Mpinfo(1, 1)) + "  " + (Mpinfo(2, 1)) + "  " + (Mpinfo(3, 1)))
    End If
    ListView1.ListItems.Clear
    ListView1.ColumnHeaders.Clear
    ListView1.ColumnHeaders.Add , , "", 0
    ListView1.ColumnHeaders.Add , , "File", ListView1.Width / (18 / 12)
    ListView1.ColumnHeaders.Add , , "Size", ListView1.Width / (18 / 2.5)
    ListView1.ColumnHeaders.Add , , "Date", ListView1.Width / (18 / 2.5)
    ListView1.View = lvwReport
    For iA = 4 To iAntLinier
        Mpinfo(iA, 2) = Mid(Left(Mpinfo(iA, 1), 3), 2, 2) 'Filenumber
        Mpinfo(iA, 3) = Mid(Mpinfo(iA, 1), 5, 8) 'Filesize
        Mpinfo(iA, 4) = Mid(Mpinfo(iA, 1), 14, 8) 'Filedate
        iB = (Len(Mpinfo(iA, 1)) - 23) 'lenght of songname
        Mpinfo(iA, 5) = Mid(Mpinfo(iA, 1), 23, iB) 'Filename
        Set itmX = ListView1.ListItems.Add()
            itmX.SubItems(1) = Mpinfo(iA, 5)
            itmX.SubItems(2) = Mpinfo(iA, 3)
            itmX.SubItems(3) = Mpinfo(iA, 4)
    Next iA
    ListView2.Sorted = True
    Call subEnableMenu
End Sub

Public Sub subEnableMenu()
    
    cmdRefresh.Enabled = True 'Refresh
    cmdInit.Enabled = True 'Initialize
    cmdUpload.Enabled = True 'Upload
    cmdDownload.Enabled = True 'Download
    cmdRemove.Enabled = True 'Remove
    cmdMoveFileUp.Enabled = True 'Move up
    cmdMoveFileDown.Enabled = True 'Move down
    mnuFileItem.Enabled = True
    mnuSettings.Enabled = True
    mnuAbout.Enabled = True
    ListView1.Enabled = True
    ListView2.Enabled = True
        
End Sub

Public Sub subDisableMenu()

    cmdRefresh.Enabled = False 'Refresh
    cmdInit.Enabled = False 'Initialize
    cmdUpload.Enabled = False 'Upload
    cmdDownload.Enabled = False 'Download
    cmdRemove.Enabled = False 'Remove
    cmdMoveFileUp.Enabled = False 'Move up
    cmdMoveFileDown.Enabled = False 'Move down
    mnuFileItem.Enabled = False
    mnuSettings.Enabled = False
    mnuAbout.Enabled = False
    ListView1.Enabled = False
    ListView2.Enabled = False
            
End Sub

Private Sub ListView2_ColumnClick(ByVal ColumnHeader As ColumnHeader)
    ' When a ColumnHeader object is clicked, the ListView control is
    ' sorted by the subitems of that column.
    ' Set the SortKey to the Index of the ColumnHeader - 1
    If SortOrder2 = True Then
        ListView2.SortOrder = 0
    Else
        ListView2.SortOrder = 1
    End If
    SortOrder2 = Not SortOrder2
    ListView2.Sorted = True
    ListView2.SortKey = ColumnHeader.Index - 1
    ' Set Sorted to True to sort the list.
End Sub

Private Sub cmdMoveFileUp_Click()
Dim sMsg, sTitle, sText, sLineOfText, sFilename As String
Dim iStyle, iResponse, iFileNumber, iA As Integer
    Call subDisableMenu
    If ListView1.ListItems.Count > 0 Then
        If ListView1.SelectedItem.Index <> 1 Then
            sFilename = ListView1.SelectedItem.ListSubItems(1)
            sMsg = "Move " + sFilename + " one position up in MPMan?" ' Define message.
            iStyle = vbYesNo + vbInformation + vbDefaultButton2   ' Define buttons.
            sTitle = "Move confirmation"   ' Define title.
            iResponse = MsgBox(sMsg, iStyle, sTitle)
            If iResponse = vbYes Then   ' User chose Yes.
                Call subMPinfo
                iFileNumber = FreeFile
                Open (sMP3Path & "\mpman.bat") For Output As #iFileNumber
                    Print #iFileNumber, Mid(sMP3Path, 1, 2)
                    Print #iFileNumber, "cd " + sMP3Path
                    Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -fu " + """" + sFilename + """"
                    Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -l >status.txt"
                Close #iFileNumber
                glPid = Shell((sMP3Path + "\mpman.bat"), 0)
                Label3.Caption = "Moving up file: " + sFilename
                Call funcDelay(5)
                Call subDisp
                Label3.Caption = ""
            End If
        End If
    End If
    Call subEnableMenu
End Sub

Private Sub cmdMoveFileDown_Click()
Dim sMsg, sTitle, sText, sLineOfText, sFilename   As String
Dim iStyle, iResponse, iFileNumber, iA As Integer
    Call subDisableMenu
    If ListView1.ListItems.Count > 0 Then
        If ListView1.SelectedItem.Index <> ListView1.ListItems.Count Then
            sFilename = ListView1.SelectedItem.ListSubItems(1)
            sMsg = "Move " + sFilename + " one position down in MPMan?" ' Define message.
            iStyle = vbYesNo + vbInformation + vbDefaultButton2   ' Define buttons.
            sTitle = "Move confirmation"   ' Define title.
            iResponse = MsgBox(sMsg, iStyle, sTitle)
            If iResponse = vbYes Then   ' User chose Yes.
                Call subMPinfo
                iFileNumber = FreeFile
                Open (sMP3Path & "\mpman.bat") For Output As #iFileNumber
                    Print #iFileNumber, Mid(sMP3Path, 1, 2)
                    Print #iFileNumber, "cd " + sMP3Path
                    Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -fd " + """" + sFilename + """"
                    Print #iFileNumber, "mpmb -p " + sPort + " -m " + sMpManSize + " -l >status.txt"
                Close #iFileNumber
                glPid = Shell((sMP3Path + "\mpman.bat"), 0)
                Label3.Caption = "Moving down file: " + sFilename
                Call funcDelay(5)
                Call subDisp
                Label3.Caption = ""
            End If
        End If
    End If
    Call subEnableMenu
End Sub
Public Sub cmdKill_Click()
Dim i As Long
'
' Enumerate all parent windows for the process.
'
cmdKill.Visible = False
bAbort = True
Call fEnumWindows
'
' Send a close command to each parent window.
' The app may issue a close confirmation dialog
' depending on how it handles the WM_CLOSE message.
'
For i = 1 To colHandle.Count
    glHandle = colHandle.Item(i)
    Call SendMessage(glHandle, WM_CLOSE, 0&, 0&)
Next
End Sub

Public Sub subMPinfo()
Dim sPath, sLineOfText As String
Dim iFileNumber As Integer
    iFileNumber = FreeFile
    sPath = App.Path
    Open (sPath & "\mpman.ini") For Input As #iFileNumber
        Line Input #iFileNumber, sLineOfText
        sLineOfText = Mid((sLineOfText), 6, 3)
        sPort = sLineOfText
        Line Input #iFileNumber, sLineOfText
        sLineOfText = Mid((sLineOfText), 7, 2)
        sMpManSize = sLineOfText
        Line Input #iFileNumber, sLineOfText
        Line Input #iFileNumber, sLineOfText
        Line Input #iFileNumber, sLineOfText
        sMP3Path = sLineOfText
        Frame2.Caption = "Path to mp3 files: " + sMP3Path
    Close #iFileNumber
End Sub

Public Function funcDelay(iDelay)
Dim iI As Integer
    For iI = 1 To iDelay
        Timer1Done = False
        Timer1.Enabled = True
        While Not Timer1Done = True
            DoEvents
        Wend
        Timer1.Enabled = False
    Next
End Function
