#include "stdafx.h"

#include "trapkeys.h"
//#include <Windows.h>


/*typedef struct tagKBDLLHOOKSTRUCT {
    DWORD vkCode;      // virtual key code
    DWORD scanCode;    // scan code
    DWORD flags;       // flags
    DWORD time;        // time stamp for this message
    DWORD dwExtraInfo; // extra info from the driver or keybd_event
} KBDLLHOOKSTRUCT, FAR *LPKBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;*/
 
//const int LLKHF_ALTDOWN=32;
//const int WH_KEYBOARD_LL=13;

HHOOK hhkLowLevelKybd;

BOOL PeekAndPump()
{
    static MSG msg;

    while (::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) 
    {
        if (!AfxGetApp()->PumpMessage()) 
        {
            ::PostQuitMessage(0);
            return FALSE;
        }   
    }

    return TRUE;
}

bool ostypent(void)
{
        OSVERSIONINFO vi;

        vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&vi);
        if (vi.dwPlatformId!=VER_PLATFORM_WIN32_NT)
        return false;
        return true;
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, 
   WPARAM wParam, LPARAM lParam) {

   BOOL fEatKeystroke = FALSE;

   if (nCode == HC_ACTION) {
      switch (wParam) {
      case WM_KEYDOWN:  case WM_SYSKEYDOWN:
      case WM_KEYUP:    case WM_SYSKEYUP: 


		  PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
         fEatKeystroke = 
            ((p->vkCode == VK_TAB) && ((p->flags & LLKHF_ALTDOWN) != 0)) ||
			(p->vkCode == 91)||(p->vkCode == 92)||
            ((p->vkCode == VK_ESCAPE) && 
            ((p->flags & LLKHF_ALTDOWN) != 0)) ||
            ((p->vkCode == VK_ESCAPE) && ((GetKeyState(VK_CONTROL) & 
             0x8000) != 0));
         break;
      }
   }
   return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, 
          lParam));
}

void trapsyskeys()
{
	if (ostypent())
	  hhkLowLevelKybd  = SetWindowsHookEx(WH_KEYBOARD_LL, 
      LowLevelKeyboardProc,GetModuleHandle(NULL), 0);
	else SystemParametersInfo (SPI_SCREENSAVERRUNNING,1,NULL,0);
}

void untrapsyskeys()
{
	if (ostypent()) UnhookWindowsHookEx(hhkLowLevelKybd);
		else SystemParametersInfo (SPI_SCREENSAVERRUNNING,0,NULL,0);
}

