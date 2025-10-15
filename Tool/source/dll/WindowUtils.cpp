#include <Windows.h>
#include <stack>
#include <cassert>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "WindowUtils.h"

constexpr auto TOPMOST_AUDIO_FILE = L"C:\\Windows\\Media\\Speech On.wav";
constexpr auto UNTOPMOST_AUDIO_FILE = L"C:\\Windows\\Media\\Speech Off.wav";

static std::stack<HWND> g_minimized_windows;
std::unordered_map<HWND, std::pair<LONG_PTR, LONG_PTR>> g_window_styles; // {  窗口句柄, { 原始样式, 原始扩展样式 }  }

// TODO: 在初始化函数中创建消息处理线程，添加事件钩子监视窗口销毁事件，清理与窗口句柄关联的资源

TOOL_API void SetupWindowUtils()
{
}

TOOL_API void CleanupWindowUtils()
{
    StopCursorClipper();
}

TOOL_API void MinimizeForegroundWindow()
{
    HWND hWnd = GetForegroundWindow();
    if (hWnd && IsWindow(hWnd))
    {
        ShowWindow(hWnd, SW_MINIMIZE);
        g_minimized_windows.push(hWnd);
    }
}

TOOL_API void RestoreMinimizedWindow()
{
    while (!g_minimized_windows.empty())
    {
        HWND hWnd = g_minimized_windows.top();
        g_minimized_windows.pop();
        if (hWnd && IsWindow(hWnd))
        {
            ShowWindow(hWnd, SW_RESTORE);
            break;
        }
    }
}

TOOL_API void ChangeForegroundWindowInputLanguage()
{
    HWND hWnd = GetForegroundWindow();
    if (!hWnd || !IsWindow(hWnd))
    {
        return;
    }
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    HKL current_hkl = GetKeyboardLayout(dwThreadId);
    int nBuff = GetKeyboardLayoutList(0, nullptr);
    if (nBuff <= 0)
    {
        return;
    }
    HKL* hkl_list = new HKL[nBuff];
    int nCount = GetKeyboardLayoutList(nBuff, hkl_list);
    /* 在列表中查找 current_hkl */
    int i;
    for (i = 0; i < nCount; i++)
    {
        if (hkl_list[i] == current_hkl)
        {
            break;
        }
    }
    i = (i + 1) % nCount;
    PostMessageW(hWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)hkl_list[i]);
    delete[] hkl_list;
}

TOOL_API int IsTopmost(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))
    {
        return 0;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    return (exStyle & WS_EX_TOPMOST) != 0;
}

TOOL_API void TopmostWindow(HWND hWnd)
{
    if (hWnd && IsWindow(hWnd))
    {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        PlaySoundW(TOPMOST_AUDIO_FILE, nullptr, SND_FILENAME | SND_ASYNC);
    }
}

TOOL_API void UntopmostWindow(HWND hWnd)
{
    if (hWnd && IsWindow(hWnd))
    {
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        PlaySoundW(UNTOPMOST_AUDIO_FILE, nullptr, SND_FILENAME | SND_ASYNC);
    }
}

TOOL_API void ToggleTopmostWindow(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))
    {
        return;
    }
    if (IsTopmost(hWnd))
    {
        UntopmostWindow(hWnd);
    }
    else
    {
        TopmostWindow(hWnd);
    }
}

TOOL_API void ToggleTray()
{
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (IsWindowVisible(hTray))
    {
        ShowWindow(hTray, SW_HIDE);
    }
    else
    {
        ShowWindow(hTray, SW_SHOWNORMAL);
    }
}

TOOL_API void RemoveWindowBorder(HWND hWnd)
{
    if (!hWnd) return;
    if (!IsWindow(hWnd)) return;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    g_window_styles[hWnd] = { style, exStyle }; // 保存原始样式
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    SetWindowLongPtrW(hWnd, GWL_STYLE, style); // 设置新样式
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle); // 设置新扩展样式
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE); // 应用更改
}

TOOL_API void RestoreWindowBorder(HWND hWnd)
{
    if (!hWnd) return;
    if (!IsWindow(hWnd)) return;
    if (!g_window_styles.contains(hWnd)) return;
    auto [style, exStyle] = g_window_styles[hWnd];
    SetWindowLongPtrW(hWnd, GWL_STYLE, style); // 恢复原始样式
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle); // 恢复原始扩展样式
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE); // 应用更改
}

TOOL_API void CenterWindow(HWND hWnd)
{
    if (!hWnd) return;
    if (!IsWindow(hWnd)) return;
    RECT rcWindow;
    if (!GetWindowRect(hWnd, &rcWindow)) return;
    auto screen_width = GetSystemMetrics(SM_CXSCREEN);
    auto screen_height = GetSystemMetrics(SM_CYSCREEN);
    auto window_width = rcWindow.right - rcWindow.left;
    auto window_height = rcWindow.bottom - rcWindow.top;
    auto center_x = rcWindow.left + window_width / 2;
    auto center_y = rcWindow.top + window_height / 2;
    auto dx = screen_width / 2 - center_x;
    auto dy = screen_height / 2 - center_y;
    auto new_left = rcWindow.left + dx;
    auto new_top = rcWindow.top + dy;
    SetWindowPos(hWnd, nullptr, new_left, new_top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

TOOL_API void CenterClient(HWND hWnd)
{
    if (!hWnd) return;
    if (!IsWindow(hWnd)) return;
    RECT rcClient;
    if (!GetClientRect(hWnd, &rcClient)) return;
    RECT rcWindow;
    if (!GetWindowRect(hWnd, &rcWindow)) return;
    assert(rcClient.left == 0 && rcClient.top == 0);
    POINT ptClientTopLeft{rcClient.left, rcClient.top};
    ClientToScreen(hWnd, &ptClientTopLeft);
    auto window_width = rcClient.right - rcClient.left;
    auto window_height = rcClient.bottom - rcClient.top;
    auto screen_width = GetSystemMetrics(SM_CXSCREEN);
    auto screen_height = GetSystemMetrics(SM_CYSCREEN);
    auto client_center_x = ptClientTopLeft.x + window_width / 2;
    auto client_center_y = ptClientTopLeft.y + window_height / 2;
    auto dx = screen_width / 2 - client_center_x;
    auto dy = screen_height / 2 - client_center_y;
    auto new_left = rcWindow.left + dx;
    auto new_top = rcWindow.top + dy;
    SetWindowPos(hWnd, nullptr, new_left, new_top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static HWND g_hClipWnd{ nullptr };
static std::mutex g_clipper_mtx;
static std::thread g_clipper_thread;
static std::stop_source g_clipper_thread_stop_source;

static void StartCursorClipperImpl()
{
    // 创建线程
    g_clipper_thread_stop_source = std::stop_source(); // 重置 stop source
    g_hClipWnd = GetForegroundWindow(); // 获取当前前台窗口
    auto fn = [] (std::stop_token stop_token) {
        while (!stop_token.stop_requested())
        {
            do
            {
                if (!IsWindow(g_hClipWnd)) return; // 窗口无效，退出线程
                if (g_hClipWnd == GetForegroundWindow() && IsWindowVisible(g_hClipWnd))
                {
                    RECT rect;
                    GetClientRect(g_hClipWnd, &rect);
                    assert(rect.left == 0 && rect.top == 0);
                    POINT pt{ 0, 0 };
                    ClientToScreen(g_hClipWnd, &pt);
                    rect.left = pt.x;
                    rect.top = pt.y;
                    rect.right += pt.x;
                    rect.bottom += pt.y;
                    ClipCursor(&rect);
                }
            } while (false);
            Sleep(20);
        }
        ClipCursor(nullptr);
    };
    g_clipper_thread = std::thread(fn, g_clipper_thread_stop_source.get_token());
}

TOOL_API void StartCursorClipper()
{
    std::lock_guard lk(g_clipper_mtx);
    if (g_clipper_thread.joinable()) // 已经在运行
    {
        return;
    }
    StartCursorClipperImpl();
}

static void StopCursorClipperImpl()
{
    g_clipper_thread_stop_source.request_stop(); // 请求停止
    g_clipper_thread.join(); // 等待线程结束
    g_hClipWnd = nullptr;
}

TOOL_API void StopCursorClipper()
{
    std::lock_guard lk(g_clipper_mtx);
    if (g_clipper_thread.joinable()) // 未在运行
    {
        StopCursorClipperImpl();
    }
}

TOOL_API void ToggleCursorClipper()
{
    std::lock_guard lk(g_clipper_mtx);
    if (g_clipper_thread.joinable())
    {
        StopCursorClipperImpl();
    }
    else
    {
        StartCursorClipperImpl();
    }
}