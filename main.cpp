// ---------------------------------------------------------------------------
#pragma comment(lib,"PowrProf.lib")

#include "pch.h"
#include "notify.cpp"
#include "geterror.cpp"

HPOWERNOTIFY hPwNotify;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool InitApplication(HINSTANCE hInstance);
bool InitInstance(HINSTANCE hInstance, int nCmdShow);
bool getDebugPriv();

const int IDC_OK_BUTTON = 101; // Button identifier
HWND hButton;
HMENU PopUpMenu;
UINT MYWM_NOTIFYICON;

// ---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	InitApplication(hInstance);
	InitInstance(hInstance, nCmdShow);

	MSG msg;

	while (GetMessageA(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClassA(_T("sqrPowerManager"), hInstance);
	return msg.wParam;
}

// ---------------------------------------------------------------------------
bool InitApplication(HINSTANCE hInstance) {
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursorA(0, IDC_ARROW);
	wc.hIcon = LoadIconA(0, IDI_SHIELD);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("MainWClass");

	return RegisterClassEx(&wc);
}

// ---------------------------------------------------------------------------
bool InitInstance(HINSTANCE hInstance, int nCmdShow) {
	HWND hWindow = CreateWindowEx(NULL, // window style O_o
		_T("MainWClass"), // name of window class
		_T("Window"), // window->caption
		WS_OVERLAPPEDWINDOW, // style of window
		CW_USEDEFAULT, CW_USEDEFAULT, // x,y
		236, 120, // // width, height
		NULL, // parent HWND
		NULL, // menu
		hInstance, NULL // lParam
		);

	// ShowWindow(hWindow, SW_SHOW);

	return hWindow != NULL;
}

// ---------------------------------------------------------------------------
void ProcPaint(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, 10, 10, _T("Hello, World!"), 13);
	EndPaint(hWnd, &ps);
}

// ---------------------------------------------------------------------------
const UINT SomeTaskbarIcon = 0xf;

int PowerManager(WPARAM wParam) {
	switch (wParam) {
	case PBT_APMPOWERSTATUSCHANGE: {
			// Power status has changed.
			// Notifies applications of a change in the power status of the computer,
			// such as a switch from battery power to A/C. The system also broadcasts
			// this event when remaining battery power slips below the threshold
			// specified by the user or if the battery power changes by a specified percentage.

			SYSTEM_POWER_STATUS SystemPowerStatus;
			if (!GetSystemPowerStatus(&SystemPowerStatus)) {
				return GetLastError();
			}

			switch (SystemPowerStatus.ACLineStatus) {
			case 0: {
					PowerSetActiveScheme(NULL, &GUID_MAX_POWER_SAVINGS);
				} break;
			case 1: {
					PowerSetActiveScheme(NULL, &GUID_MIN_POWER_SAVINGS);
				} break;
			case 255: {
				} break;
			}

		} break;
	case PBT_APMRESUMEAUTOMATIC: {
			// that message reseived after sleep mode ex.
			// SetWindowTextA(hWnd, "app resumed from low-power =)\0");
		} break;
	case PBT_APMRESUMESUSPEND: {
			// SetWindowTextA(hWnd, "User activity detected=)\0");
		} break;
	case PBT_POWERSETTINGCHANGE: {
			// SetWindowTextA(hWnd, "Some power setting changes\0");
		} break;
	default: {
			// SetWindowTextA(hWnd, "Unknown power message\0");
		}
	}
	return 0;
}

#define POPUPEXIT 1

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
			if (!TaskBarAddIcon(hWnd, SomeTaskbarIcon, LoadIcon(0, IDI_SHIELD), _T("Power scheme Manager"))) {
				ErrorExit(_T("Can't create taskbar notify icon"));
			}

			hPwNotify = RegisterPowerSettingNotification(hWnd, &GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);

			if (!hPwNotify) {
				ErrorExit(_T("Can't init power manager"));
			}

			PopUpMenu = CreatePopupMenu();
			if (!PopUpMenu) {
				ErrorExit(_T("Can't create menu"));
			} else {
				AppendMenu(PopUpMenu, MF_STRING, POPUPEXIT, "&Exit\0");
			}
		} break;
	case WM_POWERBROADCAST: {
			PowerManager(wParam);
		} break;
	case WM_COMMAND: {
			switch (LOWORD(wParam)) {
			case POPUPEXIT: {
                    PostMessage(hWnd,WM_DESTROY,0,0);
				} break;
			}
		} break;
	case WM_DESTROY: {
			UnregisterPowerSettingNotification(hPwNotify);
			DestroyMenu(PopUpMenu);
			if (!TaskBarDeleteIcon(hWnd, SomeTaskbarIcon)) {
				ErrorExit(_T("TaskBarDeleteIcon"));
			}
			PostQuitMessage(0);
		} break;
	default:
		if (MYWM_NOTIFYICON) {
			switch (LOWORD(lParam)) {
			case WM_CONTEXTMENU: {
					__asm INT 3;
					__asm INT 3;
				} break;
			case WM_LBUTTONDOWN: {
					POINT lpPoint;
					GetCursorPos(&lpPoint);
					TrackPopupMenuEx(PopUpMenu, TPM_LEFTBUTTON | TPM_NOANIMATION | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
						lpPoint.x, lpPoint.y, hWnd, NULL);
				} break;
			default: ;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
