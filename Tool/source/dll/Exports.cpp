#include "CursorClipper.hpp"
#include "ToolException.hpp"
#include <Windows.h>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cwchar>
#include <errhandlingapi.h>
#include <memory.h>
#include <processthreadsapi.h>
#include <stack>
#include <string>
#include <unordered_map>


#ifdef __cplusplus
extern "C" {
#endif
#define WINDOW_RECT_MUTEX_NAME_STRING L"g_ToolWindowRectMutex"
#define MAX_WINDOW_TEXT_LENGTH (512)
#define MAX_ERROR_MESSAGE_LENGTH (512)
#define LENGTHOF(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

std::stack<HWND> *g_lpWndStack;
std::unordered_map<std::wstring, LONG_PTR> g_OriginalWindowStyle;

BOOL DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
  return TRUE;
}

HANDLE g_hMouseClipper;
BOOL g_bMouseClipperNeeded;
HANDLE g_hWindowRectMutex;
WCHAR g_wWindowToClipText[MAX_WINDOW_TEXT_LENGTH];
RECT g_WndClipBorder;
WCHAR g_wLastErrorMessage[MAX_ERROR_MESSAGE_LENGTH];

static VOID AsyncPlaySoundW(LPCWSTR lpszSound) noexcept;
static VOID AdjustWindowRectToMiddleOfScreen(RECT &rect) noexcept;
__declspec(dllexport) VOID ToggleTray() noexcept;
__declspec(dllexport) BOOL MinimizeForegroundWindow();
__declspec(dllexport) BOOL RestoreMinimizedWindow() noexcept;
__declspec(dllexport) VOID ToggleCursorClipper() noexcept;
__declspec(dllexport) VOID MakeForegroundWindowBorderless() noexcept;
__declspec(dllexport) VOID ChangeForegroundWindowInputLanguage() noexcept;
__declspec(dllexport) BOOL InitializeToolDll();
__declspec(dllexport) VOID DeinitializeToolDll();
__declspec(dllexport) VOID CenterWindowClientArea(HWND hWnd) noexcept;

__declspec(dllexport) BOOL InitializeToolDll() {
  g_lpWndStack = new std::stack<HWND>();
  // g_lpBorderlessWindowMap = new std::unordered_map<std::wstring,
  // std::shared_ptr<WINDOWINFO>>();
  return g_lpWndStack != nullptr;
}

__declspec(dllexport) VOID DeinitializeToolDll() {
  delete g_lpWndStack;
  // delete g_lpBorderlessWindowMap;
  CursorClipper::close();
}

__declspec(dllexport) VOID ToggleTray() noexcept {
  HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
  if (IsWindowVisible(hTray)) {
    ShowWindow(hTray, SW_HIDE);
  } else {
    ShowWindow(hTray, SW_SHOWNORMAL);
  }
}

__declspec(dllexport) VOID HideTray() noexcept {
  HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
  ShowWindow(hTray, SW_HIDE);
}

__declspec(dllexport) VOID ShowTray() noexcept {
  HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
  ShowWindow(hTray, SW_SHOWNORMAL);
}

__declspec(dllexport) BOOL MinimizeForegroundWindow() {
  HWND hForeWnd = GetForegroundWindow();
  if (hForeWnd) {
    g_lpWndStack->push(hForeWnd);
    ShowWindow(hForeWnd, SW_MINIMIZE);
    return TRUE;
  }
  return FALSE;
}

__declspec(dllexport) BOOL RestoreMinimizedWindow() noexcept {
  if (g_lpWndStack->empty()) {
    return FALSE;
  }
  HWND hWnd = g_lpWndStack->top();
  g_lpWndStack->pop();
  if (IsWindow(hWnd)) {
    ShowWindow(hWnd, SW_RESTORE);
    return TRUE;
  }
  return FALSE;
}

__declspec(dllexport) VOID ToggleCursorClipper() noexcept {
  if (CursorClipper::is_alive()) {
    CursorClipper::close();
    AsyncPlaySoundW(L"C:\\Windows\\Media\\Windows Print Complete.wav");
    return;
  }
  try {
    CursorClipper::open();
    AsyncPlaySoundW(L"C:\\Windows\\Media\\Windows Notify.wav");
  } catch (ToolException e) {
    e.notify();
  }
}

__declspec(dllexport) VOID MakeWindowBorderless(HWND hWnd) noexcept {
  std::wstring window_text;
  int window_text_length;
  DWORD dwOriginalStyle;
  DWORD dwModifiedStyle;
  RECT rcWindow;
  if (!IsWindow(hWnd) ||
      0 == (dwOriginalStyle = GetWindowLongW(hWnd, GWL_STYLE)) ||
      !GetClientRect(hWnd, &rcWindow) ||
      !(window_text_length = GetWindowTextLengthW(hWnd))) {
    return;
  }
  window_text.resize(window_text_length);
  if (g_OriginalWindowStyle.find(window_text) == g_OriginalWindowStyle.end()) {
    g_OriginalWindowStyle[std::move(window_text)] = dwOriginalStyle;
  }
  dwModifiedStyle = dwOriginalStyle &
                    ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX |
                      WS_MINIMIZEBOX); // 去除边框后的窗口样式
  RedrawWindow(hWnd, NULL, NULL,
               RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_INTERNALPAINT);
  Sleep(500);
  AdjustWindowRect(&rcWindow, dwModifiedStyle, FALSE);
  AdjustWindowRectToMiddleOfScreen(rcWindow);
  SetWindowLongPtrW(hWnd, GWL_STYLE, dwModifiedStyle);
  Sleep(500);
  SetWindowPos(hWnd, 0, rcWindow.left, rcWindow.top,
               rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top,
               SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED |
                   SWP_NOSENDCHANGING);
  Sleep(500);
  RedrawWindow(hWnd, NULL, NULL,
               RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_INTERNALPAINT);
}

__declspec(dllexport) VOID MakeForegroundWindowBorderless() noexcept {
  HWND hWnd = GetForegroundWindow();
  std::wstring window_text;
  int window_text_length;
  DWORD dwOriginalStyle;
  DWORD dwModifiedStyle;
  RECT rcWindow;
  try {
    if (!IsWindow(hWnd) ||
        0 == (dwOriginalStyle = GetWindowLongW(hWnd, GWL_STYLE)) ||
        !GetClientRect(hWnd, &rcWindow) ||
        !(window_text_length = GetWindowTextLengthW(hWnd))) {
      throw ToolException("错误描述：获取前台窗口信息失败。\r\n"

                          "错误代码：%lu。\r\n",

                          GetLastError());
    }
    window_text.resize(window_text_length);
    if (!GetWindowTextW(hWnd, &window_text[0], window_text_length)) // 不在表中
    {
      throw ToolException("错误描述：无法读取窗口名称。\r\n"
                          "错误代码：%lu。\r\n",
                          GetLastError());
    }
    if (g_OriginalWindowStyle.find(window_text) ==
        g_OriginalWindowStyle.end()) {
      g_OriginalWindowStyle[std::move(window_text)] = dwOriginalStyle;
    }
    dwModifiedStyle =
        dwOriginalStyle &
        ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX |
          WS_MINIMIZEBOX | RDW_INTERNALPAINT); // 去除边框后的窗口样式
  } catch (ToolException e) {
    e.notify();
    return;
  }

  RedrawWindow(hWnd, NULL, NULL,
               RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_INTERNALPAINT);
  Sleep(500);
  AdjustWindowRect(&rcWindow, dwModifiedStyle, FALSE);
  AdjustWindowRectToMiddleOfScreen(rcWindow);
  SetWindowLongPtrW(hWnd, GWL_STYLE, dwModifiedStyle);
  Sleep(500);
  SetWindowPos(hWnd, 0, rcWindow.left, rcWindow.top,
               rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top,
               SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED |
                   SWP_NOSENDCHANGING);
  Sleep(500);
  RedrawWindow(hWnd, NULL, NULL,
               RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_INTERNALPAINT);
}

__declspec(dllexport) VOID MakeForegroundWindowBordered() noexcept {
  HWND hWnd = GetForegroundWindow();
  std::wstring window_text;
  int window_text_length;
  DWORD dwCurrentStyle;
  DWORD dwOriginalStyle;
  RECT rcWindow;
  try {
    if (!IsWindow(hWnd) ||
        0 == (dwCurrentStyle = GetWindowLongW(hWnd, GWL_STYLE)) ||
        !GetClientRect(hWnd, &rcWindow) ||
        !(window_text_length = GetWindowTextLengthW(hWnd))) {
      throw ToolException("错误描述：获取前台窗口信息失败。\r\n"

                          "错误代码：%lu。\r\n",

                          GetLastError());
    }
    window_text.resize(window_text_length);
    if (!GetWindowTextW(hWnd, &window_text[0], window_text_length)) {
      throw ToolException("错误描述：无法读取窗口名称。\r\n"
                          "错误代码：%lu。\r\n",
                          GetLastError());
    }
    if (g_OriginalWindowStyle.find(window_text) ==
        g_OriginalWindowStyle.end()) {
      throw ToolException(
          "错误描述：找不到有关此窗口的信息（尚未去除过此窗口边框）。\r\n"
          "错误代码：%lu。\r\n",
          GetLastError());
    }
    dwOriginalStyle = g_OriginalWindowStyle[window_text]; // 原来的样式
  } catch (ToolException e) {
    e.notify();
    return;
  }

  if (dwOriginalStyle != WS_OVERLAPPED) {
    AdjustWindowRect(&rcWindow, dwOriginalStyle | dwCurrentStyle, FALSE);
  }
  RedrawWindow(hWnd, NULL, NULL,
               RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_INTERNALPAINT);
  Sleep(500);
  AdjustWindowRectToMiddleOfScreen(rcWindow);
  SetWindowLongPtrW(hWnd, GWL_STYLE, dwOriginalStyle | dwCurrentStyle);
  Sleep(500);
  SetWindowPos(hWnd, 0, rcWindow.left, rcWindow.top,
               rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top,
               SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSENDCHANGING);
  Sleep(500);
  InvalidateRect(hWnd, NULL, TRUE);
  RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW);
}

__declspec(dllexport) VOID ToggleTopmostForegroundWindow() noexcept {
  HWND hForeWnd = GetForegroundWindow();
  if (!hForeWnd)
    return;
  LONG_PTR lWindow = GetWindowLongPtrW(hForeWnd, GWL_EXSTYLE);
  if ((lWindow & WS_EX_TOPMOST) == 0) {
    SetWindowPos(hForeWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    AsyncPlaySoundW(L"C:\\Windows\\Media\\Speech On.wav");
  } else /* 已经置顶 */
  {
    SetWindowPos(hForeWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    AsyncPlaySoundW(L"C:\\Windows\\Media\\Speech Off.wav");
  }
}

__declspec(dllexport) VOID TopmostWindow(HWND hWnd) noexcept {
  if (!IsWindow(hWnd))
    return;
  LONG_PTR lWindow = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
  SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  AsyncPlaySoundW(L"C:\\Windows\\Media\\Speech On.wav");
}

__declspec(dllexport) VOID UntopmostWindow(HWND hWnd) noexcept {
  if (!IsWindow(hWnd))
    return;
  LONG_PTR lWindow = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
  SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  AsyncPlaySoundW(L"C:\\Windows\\Media\\Speech Off.wav");
}

__declspec(dllexport) VOID ChangeForegroundWindowInputLanguage() noexcept {
  HWND hForeWnd = GetForegroundWindow();
  if (!hForeWnd)
    return;
  DWORD dwTId = GetWindowThreadProcessId(hForeWnd, NULL);
  HKL current_hkl = GetKeyboardLayout(dwTId);
  int nBuff = GetKeyboardLayoutList(0, NULL);
  LPHKL hkl_list = new HKL[nBuff];
  int nCount = GetKeyboardLayoutList(nBuff, hkl_list);
  /* 在列表中查找 current_hkl */
  int i;
  for (i = 0; i < nCount; i++) {
    if (hkl_list[i] == current_hkl) {
      break;
    }
  }
  i = (i + 1) % nCount;
  PostMessageW(hForeWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)hkl_list[i]);
  delete[] hkl_list;
}

VOID AsyncPlaySoundW(LPCWSTR lpszSound) noexcept {
  HMODULE hMod = LoadLibraryW(L"Winmm.dll");
  if (!hMod)
    return;
  auto PlaySoundW = (BOOL (*)(_In_opt_ LPCWSTR, _In_opt_ HMODULE,
                              _In_ DWORD))GetProcAddress(hMod, "PlaySoundW");
  PlaySoundW(lpszSound, NULL, SND_ASYNC);
  FreeLibrary(hMod);
}

VOID AdjustWindowRectToMiddleOfScreen(RECT &rect) noexcept {
  RECT rcScreen = {.left = 0, .top = 0};
  rcScreen.right = GetSystemMetrics(SM_CXSCREEN);
  rcScreen.bottom = GetSystemMetrics(SM_CYSCREEN);
  LONG lDeltaX = (rcScreen.right - (rect.left + rect.right)) / 2;
  LONG lDeltaY = (rcScreen.bottom - (rect.top + rect.bottom)) / 2;
  rect.left += lDeltaX;
  rect.right += lDeltaX;
  rect.top += lDeltaY;
  rect.bottom += lDeltaY;
}

__declspec(dllexport) VOID CenterWindowClientArea(HWND hWnd) noexcept {
  if (!hWnd)
    return;

  WINDOWPLACEMENT wp{sizeof(wp)};
  if (!GetWindowPlacement(hWnd, &wp))
    return;

  if (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMAXIMIZED)
    return;

  // Determine target monitor work area.
  HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO monitor_info{sizeof(monitor_info)};
  if (!GetMonitorInfo(hMon, &monitor_info))
    return;
  const RECT &rcScreen = monitor_info.rcMonitor;

  const POINT ptScreenCenter{
      // screen center
      .x = rcScreen.left + (rcScreen.right - rcScreen.left) / 2,
      .y = rcScreen.top + (rcScreen.bottom - rcScreen.top) / 2};

  // Get window and client rects in screen coordinates.
  RECT rcWindow{};
  if (!GetWindowRect(hWnd, &rcWindow))
    return;

  RECT rcClient{};
  if (!GetClientRect(hWnd, &rcClient))
    return;

  // Map client rect to screen coordinates to compute current non-client
  // margins.
  POINT left_top{rcClient.left, rcClient.top};
  POINT right_bottom{rcClient.right, rcClient.bottom};
  if (!ClientToScreen(hWnd, &left_top) || !ClientToScreen(hWnd, &right_bottom))
    return;

  // Reconstruct client rect in screen coordinates.
  rcClient.left = left_top.x;
  rcClient.top = left_top.y;
  rcClient.right = right_bottom.x;
  rcClient.bottom = right_bottom.y;

  const POINT ptClientCenter{
      // client center
      .x = rcClient.left + (rcClient.right - rcClient.left) / 2,
      .y = rcClient.top + (rcClient.bottom - rcClient.top) / 2};

  // Offset from client center to screen center.
  auto dx = ptScreenCenter.x - ptClientCenter.x;
  auto dy = ptScreenCenter.y - ptClientCenter.y;

  // New window position.
  const POINT ptNewWindowTopLeft{.x = rcWindow.left + dx,
                                 .y = rcWindow.top + dy};

  SetWindowPos(hWnd, nullptr, ptNewWindowTopLeft.x, ptNewWindowTopLeft.y, 0, 0,
               SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

#ifdef __cplusplus
}
#endif
