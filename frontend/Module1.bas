Attribute VB_Name = "Module1"
Option Explicit
Public glPid     As Long
Public glHandle  As Long
Public colHandle As New Collection

Public Const WM_CLOSE = &H10
Public Const WM_DESTROY = &H2

Declare Function SendMessage Lib "user32" Alias "SendMessageA" (ByVal hwnd As Long, ByVal wMsg As Long, ByVal wParam As Long, lParam As Any) As Long
Declare Function GetParent Lib "user32" (ByVal hwnd As Long) As Long
Declare Function GetWindowThreadProcessId Lib "user32" (ByVal hwnd As Long, lpdwProcessId As Long) As Long
Declare Function EnumWindows Lib "user32" (ByVal lpEnumFunc As Long, ByVal lParam As Long) As Long

Public Function fEnumWindowsCallBack(ByVal hwnd As Long, ByVal lpData As Long) As Long
Dim lParent    As Long
Dim lThreadId  As Long
Dim lProcessId As Long
'
' This callback function is called by Windows (from the EnumWindows
' API call) for EVERY top-level window that exists.  It populates a
' collection with the handles of all parent windows owned by the
' process that we started.
'
fEnumWindowsCallBack = 1
lThreadId = GetWindowThreadProcessId(hwnd, lProcessId)

If glPid = lProcessId Then
    lParent = GetParent(hwnd)
    If lParent = 0 Then
        colHandle.Add hwnd
    End If
End If
End Function



Public Function fEnumWindows() As Boolean
Dim hwnd As Long
'
' The EnumWindows function enumerates all top-level windows
' by passing the handle of each window, in turn, to an
' application-defined callback function. EnumWindows
' continues until the last top-level window is enumerated or
' the callback function returns FALSE.
'
Call EnumWindows(AddressOf fEnumWindowsCallBack, hwnd)
End Function

