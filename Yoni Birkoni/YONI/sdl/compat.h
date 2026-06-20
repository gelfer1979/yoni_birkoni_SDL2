#ifndef COMPAT_H
#define COMPAT_H

// MFC include protection stubs
#define _AFXWIN_H_
#define __AFXWIN_H__
#define _AFXEXT_H_
#define _AFXTEMPL_H_
#define _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cassert>
#include <stdarg.h>

inline std::string normalize_path(const std::string& path) {
    std::string s = path;
    for (char& c : s) {
        if (c == '\\') {
            c = '/';
        }
    }
    return s;
}

#define SDL_MAIN_HANDLED
#include <SDL.h>

// Calling conventions and attributes stubs
#ifndef CALLBACK
#define CALLBACK
#endif
#ifndef EXPORT
#define EXPORT
#endif
#ifndef _fastcall
#define _fastcall
#endif
#ifndef __fastcall
#define __fastcall
#endif
#ifndef WINAPI
#define WINAPI
#endif

// Windows types stubs for non-Windows platforms
#ifdef _WIN32
  #include <windows.h>
  #include <shellapi.h>
#else
  #include <sys/stat.h>
  typedef unsigned long DWORD;
  typedef unsigned short WORD;
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
  typedef int BOOL;
  
  #define TRUE 1
  #define FALSE 0
  
  typedef void* HWND;
  typedef void* HDC;
  typedef void* HBITMAP;
  typedef void* HICON;
  typedef void* HCURSOR;
  typedef long LONG;
  
  typedef unsigned char* LPBYTE;
  typedef const char* LPCSTR;
  typedef char* LPSTR;
  typedef int INT;
  
  typedef uintptr_t WPARAM;
  typedef intptr_t LPARAM;
  typedef intptr_t LRESULT;
  
  struct RECT {
      LONG left;
      LONG top;
      LONG right;
      LONG bottom;
  };
  typedef RECT* LPRECT;
  
  struct POINT {
      LONG x;
      LONG y;
  };
  
  struct LOGFONT {
      LONG lfHeight;
      LONG lfWidth;
      LONG lfEscapement;
      LONG lfOrientation;
      LONG lfWeight;
      BYTE lfItalic;
      BYTE lfUnderline;
      BYTE lfStrikeOut;
      BYTE lfCharSet;
      BYTE lfOutPrecision;
      BYTE lfClipPrecision;
      BYTE lfQuality;
      BYTE lfPitchAndFamily;
      char lfFaceName[32];
  };

  #define FILE_ATTRIBUTE_NORMAL 0
  #define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
  
  // Win32 API functions stubs for non-Windows platforms
  inline DWORD GetTickCount() {
      return (DWORD)SDL_GetTicks();
  }
  
  inline DWORD GetTempPath(DWORD nBufferLength, LPSTR lpBuffer) {
      const char* tmp = getenv("TMPDIR");
      if (!tmp) tmp = getenv("TMP");
      if (!tmp) tmp = getenv("TEMP");
      if (!tmp) tmp = "/tmp";
      strncpy(lpBuffer, tmp, nBufferLength);
      return (DWORD)strlen(lpBuffer);
  }
  
  inline DWORD GetFileAttributes(LPCSTR lpFileName) {
      struct stat st;
      std::string normalized = normalize_path(lpFileName);
      if (stat(normalized.c_str(), &st) == 0) {
          return FILE_ATTRIBUTE_NORMAL;
      }
      return INVALID_FILE_ATTRIBUTES;
  }
  
  inline BOOL CopyFile(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists) {
      (void)bFailIfExists;
      FILE* src = fopen(normalize_path(lpExistingFileName).c_str(), "rb");
      if (!src) return FALSE;
      FILE* dst = fopen(normalize_path(lpNewFileName).c_str(), "wb");
      if (!dst) {
          fclose(src);
          return FALSE;
      }
      char buf[4096];
      size_t bytes;
      while ((bytes = fread(buf, 1, sizeof(buf), src)) > 0) {
          fwrite(buf, 1, bytes, dst);
      }
      fclose(src);
      fclose(dst);
      return TRUE;
  }
  
  inline BOOL SetFileAttributes(LPCSTR lpFileName, DWORD dwFileAttributes) {
      (void)lpFileName;
      (void)dwFileAttributes;
      return TRUE;
  }
  
  #define SW_SHOW 5
  inline void* ShellExecute(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd) {
      (void)hwnd; (void)lpOperation; (void)lpParameters; (void)lpDirectory; (void)nShowCmd;
      printf("ShellExecute stub called: %s\n", lpFile);
      return (void*)32;
  }
  
  #define SHORT short
  inline short GetAsyncKeyState(int vKey) {
      const Uint8* state = SDL_GetKeyboardState(nullptr);
      SDL_Scancode sc = SDL_SCANCODE_UNKNOWN;
      switch (vKey) {
          case 0x25: sc = SDL_SCANCODE_LEFT; break;
          case 0x26: sc = SDL_SCANCODE_UP; break;
          case 0x27: sc = SDL_SCANCODE_RIGHT; break;
          case 0x28: sc = SDL_SCANCODE_DOWN; break;
          case 0x57: sc = SDL_SCANCODE_W; break;
          case 0x53: sc = SDL_SCANCODE_S; break;
          case 0x41: sc = SDL_SCANCODE_A; break;
          case 0x44: sc = SDL_SCANCODE_D; break;
          case 0x1B: sc = SDL_SCANCODE_ESCAPE; break;
          case 0x0D: sc = SDL_SCANCODE_RETURN; break;
          case 0x20: sc = SDL_SCANCODE_SPACE; break;
          case 0x08: sc = SDL_SCANCODE_BACKSPACE; break;
          case 0x59: sc = SDL_SCANCODE_Y; break;
          case 0x4E: sc = SDL_SCANCODE_N; break;
      }
      if (sc != SDL_SCANCODE_UNKNOWN && state[sc]) {
          return (short)0x8000;
      }
      return 0;
  }
  
  #define MB_ICONERROR 0x00000010L
  #define MB_YESNO 0x00000004L
  #define MB_OK 0x00000000L
  #define IDYES 6
  #define IDOK 1
  #define SM_CXICON 11
  #define SM_CYICON 12
  #define MF_SEPARATOR 0x00000800L
  #define MF_STRING 0x00000000L
  
  inline int GetSystemMetrics(int nIndex) {
      (void)nIndex;
      return 32;
  }
  
  inline UINT MapVirtualKey(UINT uCode, UINT uMapType) {
      (void)uMapType;
      switch (uCode) {
          case 0x1e: return 'A';
          case 0x30: return 'B';
          case 0x2e: return 'C';
          case 0x20: return 'D';
          case 0x12: return 'E';
          case 0x21: return 'F';
          case 0x22: return 'G';
          case 0x23: return 'H';
          case 0x17: return 'I';
          case 0x24: return 'J';
          case 0x25: return 'K';
          case 0x26: return 'L';
          case 0x32: return 'M';
          case 0x31: return 'N';
          case 0x18: return 'O';
          case 0x19: return 'P';
          case 0x10: return 'Q';
          case 0x13: return 'R';
          case 0x1f: return 'S';
          case 0x14: return 'T';
          case 0x16: return 'U';
          case 0x2f: return 'V';
          case 0x11: return 'W';
          case 0x2d: return 'X';
          case 0x15: return 'Y';
          case 0x2c: return 'Z';
          default: return 0;
      }
  }
  
  inline char* itoa(int value, char* str, int base) {
      if (base == 10) {
          sprintf(str, "%d", value);
      } else if (base == 16) {
          sprintf(str, "%x", value);
      } else {
          sprintf(str, "%d", value);
      }
      return str;
  }
  
  inline int MessageBox(HWND hwnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
      (void)hwnd; (void)uType;
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, lpCaption ? lpCaption : "Game Message", lpText, nullptr);
      return IDOK;
  }
  
  #define WM_APP 0x8000
  #define WM_USER 0x0400
  #define WM_SYSCOMMAND 0x0112
  #define WM_PAINT 0x000F
  #define WM_DESTROY 0x0002
  #define WM_ICONERASEBKGND 0x0027
  #define WM_LBUTTONDOWN 0x0201
  #define WM_MOUSEMOVE 0x0200
  #define WM_KILLFOCUS 0x0008
  #define WM_KEYDOWN 0x0100
  #define WM_KEYUP 0x0101
  #define WM_SYSKEYDOWN 0x0104
  #define WM_SYSKEYUP 0x0105
  
  #define VK_TAB 0x09
  #define VK_ESCAPE 0x1B
  #define VK_CONTROL 0x11
  
  #define DEFAULT_CHARSET 1
  #define RUSSIAN_CHARSET 204
  
  inline int ShowCursor(BOOL bShow) {
      return SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);
  }
  
  inline void Sleep(DWORD dwMilliseconds) {
      SDL_Delay(dwMilliseconds);
  }
  
  inline BOOL DestroyWindow(HWND hwnd) {
      (void)hwnd;
      return TRUE;
  }
#endif

#include <assert.h>
#ifndef ASSERT
#define ASSERT(f) assert(f)
#endif

// DirectInput / DirectDraw stub definitions needed globally if they clash or are needed
struct CPoint : public POINT {
    CPoint() { x = 0; y = 0; }
    CPoint(int _x, int _y) { x = _x; y = _y; }
};
class CRect : public RECT {
public:
    CRect() { left = top = right = bottom = 0; }
    CRect(int l, int t, int r, int b) { left = l; top = t; right = r; bottom = b; }
    int Width() const { return right - left; }
    int Height() const { return bottom - top; }
};

// CString stub wrapping std::string
class CString : public std::string {
public:
    CString() : std::string() {}
    CString(const char* s) : std::string(s ? s : "") {}
    CString(const std::string& s) : std::string(s) {}
    
    void Format(const char* format, ...) {
        char buf[2048];
        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        *this = buf;
    }
    
    void LoadString(UINT id) {
        (void)id;
        *this = ""; // Dialog caption or similar
    }
    
    char* GetBuffer(int nMinBufferLength = 0) {
        (void)nMinBufferLength;
        return &(*this)[0];
    }
    
    void ReleaseBuffer(int nNewLength = -1) {
        (void)nNewLength;
    }
    
    bool IsEmpty() const { return empty(); }
    operator const char*() const { return c_str(); }
    
    CString operator+(const char* s) const {
        return CString(std::string(*this) + (s ? s : ""));
    }
    CString operator+(const std::string& s) const {
        return CString(std::string(*this) + s);
    }
    friend CString operator+(const char* s1, const CString& s2) {
        return CString((s1 ? s1 : "") + std::string(s2));
    }
};

// MFC CDC stub
class CDC {
public:
    HDC GetSafeHdc() const { return nullptr; }
    void DrawIcon(int x, int y, HICON hIcon) { (void)x; (void)y; (void)hIcon; }
};

class CPaintDC : public CDC {
public:
    CPaintDC(void* pWnd) { (void)pWnd; }
};

// MFC CMenu stub
class CMenu {
public:
    void AppendMenu(UINT nFlags, UINT nIDNewItem = 0, const char* lpszNewItem = nullptr) {
        (void)nFlags; (void)nIDNewItem; (void)lpszNewItem;
    }
};

// MFC CWnd stub
class CWnd {
public:
    HWND m_hWnd;
    void GetClientRect(RECT* lpRect) const {
        if (lpRect) { lpRect->left = lpRect->top = 0; lpRect->right = 800; lpRect->bottom = 600; }
    }
    BOOL IsIconic() const { return FALSE; }
    CMenu* GetSystemMenu(BOOL bRevert) const { (void)bRevert; static CMenu menu; return &menu; }
    
    int MessageBox(LPCSTR lpText, LPCSTR lpCaption = nullptr, UINT uType = 0) {
        (void)uType;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, lpCaption ? lpCaption : "Game Dialog", lpText, nullptr);
        return 1;
    }
    
    LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) {
        (void)message; (void)wParam; (void)lParam;
        return 0;
    }
    
    HICON SetIcon(HICON hIcon, BOOL bBigIcon) {
        (void)hIcon; (void)bBigIcon;
        return nullptr;
    }
};

// MFC CDialog stub
#define IDD_PACKMAN_DIALOG 102
class CDataExchange;
class CDialog : public CWnd {
public:
    CDialog(UINT idd, CWnd* pParent = nullptr) { (void)idd; (void)pParent; m_hWnd = (HWND)1; }
    virtual ~CDialog() {}
    virtual BOOL OnInitDialog() { return TRUE; }
    virtual void DoDataExchange(CDataExchange* pDX) { (void)pDX; }
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
        (void)message; (void)wParam; (void)lParam;
        return 0;
    }
    virtual void OnSysCommand(UINT nID, LPARAM lParam) { (void)nID; (void)lParam; }
    virtual void OnPaint() {}
    virtual void OnDestroy() {}
    int DoModal() { return 1; }
};

// MFC CWinApp stub
class CWinApp {
public:
    BOOL PumpMessage() { return TRUE; }
    HCURSOR LoadStandardCursor(LPCSTR lp) { (void)lp; return nullptr; }
    HCURSOR LoadCursor(LPCSTR lp) { (void)lp; return nullptr; }
    HICON LoadIcon(UINT id) { (void)id; return nullptr; }
    HICON LoadIcon(LPCSTR lp) { (void)lp; return nullptr; }
};

inline CWinApp* AfxGetApp() {
    static CWinApp app;
    return &app;
}

// MFC CFile stub
class CFile {
private:
    FILE* f;
public:
    static const UINT modeRead = 1;
    static const UINT modeWrite = 2;
    static const UINT modeCreate = 4;
    static const UINT typeBinary = 8;
    
    CFile() : f(nullptr) {}
    ~CFile() { Close(); }
    
    BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags) {
        char mode[8] = "";
        if (nOpenFlags & modeCreate) {
            if (nOpenFlags & modeRead) strcpy(mode, "w+b");
            else strcpy(mode, "wb");
        } else {
            if (nOpenFlags & modeWrite) strcpy(mode, "r+b");
            else strcpy(mode, "rb");
        }
        f = fopen(normalize_path(lpszFileName).c_str(), mode);
        return f != nullptr;
    }
    
    UINT Read(void* lpBuf, UINT nCount) {
        if (!f) return 0;
        return (UINT)fread(lpBuf, 1, nCount, f);
    }
    
    void Write(const void* lpBuf, UINT nCount) {
        if (f) fwrite(lpBuf, 1, nCount, f);
    }
    
    void Close() {
        if (f) {
            fclose(f);
            f = nullptr;
        }
    }
    
    unsigned long GetLength() {
        if (!f) return 0;
        long current = ftell(f);
        fseek(f, 0, SEEK_END);
        long end = ftell(f);
        fseek(f, current, SEEK_SET);
        return (unsigned long)end;
    }
};

// MFC message map macros stubs
#define DECLARE_MESSAGE_MAP()
#define BEGIN_MESSAGE_MAP(class_name, base_class_name) inline void class_name##_dummy_msg_map() {
#define END_MESSAGE_MAP() }
#define afx_msg

#define ON_WM_SYSCOMMAND()
#define ON_WM_PAINT()
#define ON_WM_QUERYDRAGICON()
#define ON_WM_DESTROY()
#define ON_WM_CHAR()
#define ON_WM_CREATE()
#define ON_WM_KILLFOCUS()
#define ON_WM_LBUTTONDOWN()
#define ON_WM_MOUSEMOVE()
#define ON_WM_SETCURSOR()

#endif // COMPAT_H
