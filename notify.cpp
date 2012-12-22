#pragma once
#include "pch.h"

extern UINT MYWM_NOTIFYICON;

BOOL TaskBarAddIcon(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip)
{
    // try to register taskbar icon mouse message
    LPSTR lpMouseMessage = "MyTaskbarIconMouseMessage";

    MYWM_NOTIFYICON = RegisterWindowMessage(lpMouseMessage);
    if (!MYWM_NOTIFYICON) {
        return false;
    }

    BOOL res;
    NOTIFYICONDATA tnid;

    tnid.cbSize = sizeof(NOTIFYICONDATA);
    tnid.hWnd = hwnd;
    tnid.uID = uID;

    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnid.uCallbackMessage = MYWM_NOTIFYICON;
    tnid.hIcon = hicon;
    if (lpszTip)
        lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
    else
        tnid.szTip[0] = '\0';

    res = Shell_NotifyIcon(NIM_ADD, &tnid);

    if (hicon)
        DestroyIcon(hicon);

    return res;
}

// ---------------------------------------------------------------------------
BOOL TaskBarDeleteIcon(HWND hwnd, UINT uID) 
{ 
    BOOL res; 
    NOTIFYICONDATA tnid; 
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hwnd; 
    tnid.uID = uID; 
         
    res = Shell_NotifyIcon(NIM_DELETE, &tnid); 

    return res; 
}

// ---------------------------------------------------------------------------
