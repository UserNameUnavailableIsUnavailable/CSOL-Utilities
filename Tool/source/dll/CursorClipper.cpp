#include "CursorClipper.hpp"
#include "ToolException.hpp"
#include <Windows.h>
#include <memory.h>
#include <cwchar>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>

DWORD CALLBACK CursorClipper::HandleMessage(LPVOID lpParam)
{
    /* 在当前线程创建事件钩子，监测前台窗口情况 */
    HWINEVENTHOOK event_hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_MOVESIZEEND,
        NULL,
        OnEvent,
        0,
        0,
        WINEVENT_OUTOFCONTEXT
    );
    if (!event_hook)
    {
        return GetLastError(); /* 带着错误码退出，以便上层获知 */
    }
    MSG msg;
    while (CursorClipper::is_alive()
        && 0 < GetMessageW(&msg, NULL, 0, 0))
    {
        /* 事件队列，使事件钩子生效 */
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (UnhookWinEvent(event_hook))
    {
        return ERROR_SUCCESS;
    }
    return GetLastError(); /* 将错误事件告知上层 */
}

DWORD CALLBACK CursorClipper::MouseCursorClipper(LPVOID lpParam)
{
    RECT window_rect;
    while (CursorClipper::is_alive())
    {
        Sleep(20);
        if (CursorClipper::is_clippable())
        {
            ClipCursor(CursorClipper::get_window_rect(&window_rect));
        }
    }
    ClipCursor(NULL);
    return ERROR_SUCCESS;
}

void CALLBACK CursorClipper::OnEvent(
    HWINEVENTHOOK hWinEventHook,
    DWORD dwEvent,
    HWND hWnd,
    LONG lObjectId,
    LONG lChildId,
    DWORD dwEventThreadId,
    DWORD dwEventTimeInMs
) noexcept
{
    if (dwEvent == EVENT_SYSTEM_FOREGROUND)
    {
        WCHAR foreWndTxt[MAX_WINDOW_TEXT_LENGTH];
        GetWindowTextW(hWnd, foreWndTxt, MAX_WINDOW_TEXT_LENGTH);
        if (!match_window_text(foreWndTxt)) /* 前台窗口改变，但不匹配 */
        {
            reset_clippable();
            ClipCursor(NULL);
            return;
        }
        set_clippable(); /* 前台窗口改变，且匹配 */
    }
    else if (
        dwEvent == EVENT_SYSTEM_MOVESIZESTART ||
        dwEvent == EVENT_SYSTEM_MOVESIZEEND ||
        dwEvent == EVENT_SYSTEM_CAPTURESTART ||
        dwEvent == EVENT_SYSTEM_CAPTUREEND
     )
    {
        WCHAR wndChanged[MAX_WINDOW_TEXT_LENGTH];
        GetWindowTextW(hWnd, wndChanged, MAX_WINDOW_TEXT_LENGTH);
        if (!match_window_text(wndChanged))
        {
            return;
        }
        RECT rect;
        GetWindowRect(hWnd, &rect);
        CursorClipper::set_window_rect(rect);
    }
}

LPCWSTR CursorClipper::WINDOW_RECT_MUTEX_NAME_STRING{ L"g_ToolWindowRectMutex"};

CursorClipper& CursorClipper::get_instance() noexcept
{
    static CursorClipper s_instance;
    return s_instance;
}

void CursorClipper::open()
{
    if (get_instance().alive)
    {
        return;
    }
    get_instance().mutex = CreateMutexW(
    NULL,
    FALSE,
    WINDOW_RECT_MUTEX_NAME_STRING
    );
    /* 创建互斥量 */
    if (!get_instance().mutex)
    {
        throw ToolException(
            "错误描述：创建互斥量失败\r\n"

            "错误语句：%s\r\n"
            "错误代码：%lu\r\n",

            "get_instance().mutex = CreateMutexW(...);",
            GetLastError()
        );
    }
    HWND hForeWnd = GetForegroundWindow();
    GetWindowTextW(hForeWnd, get_instance().window_text, MAX_WINDOW_TEXT_LENGTH);
    GetWindowRect(hForeWnd, &get_instance().window_rect);
    set_window_rect(get_instance().window_rect);
    /* 创建事件 */ 
    get_instance().event = CreateEventW(
        NULL,
        TRUE,
        FALSE, /* nonsignaled */
        L"CursorIsClippable"
    );
    if (!get_instance().event)
    {
        throw ToolException(
            "错误描述：创建事件失败\r\n"
            
            "错误语句：%s\r\n"
            "错误代码：%lu\r\n",
            
            "get_instance().event = CreateEventW(...)",
            GetLastError()
        );
    }
    get_instance().alive = true;
    /* 创建 clipper 线程 */
    get_instance().cursor_clipper = CreateThread(
        NULL,
        8192,
        MouseCursorClipper,
        &get_instance().window_rect,
        0,
        NULL
    );
    if (!get_instance().cursor_clipper)
    {
        throw ToolException(
            "错误描述：创建 cursor_clipper 线程失败\r\n"

            "错误语句：%s\r\n"
            "错误代码：%lu\r\n",

            "get_instance().cursor_clipper = CreateThread(...);",
            GetLastError()
        );
    }
    get_instance().message_handler = CreateThread(
        NULL,
        0,
        HandleMessage,
        NULL,
        0,
        NULL
    );
    if (!get_instance().message_handler)
    {
        throw ToolException(
            "错误描述：创建 message_handler 线程失败\r\n"

            "错误语句：%s\r\n"
            "错误代码：%lu\r\n",

            "get_instance().message_handler = CreateThread(...);",
            GetLastError()
        );
    }
    if (WAIT_OBJECT_0 == WaitForSingleObject(get_instance().message_handler, 10))
    {
        DWORD dwLastError;
        if (!GetExitCodeThread(get_instance().message_handler, &dwLastError))
        {
            dwLastError = GetLastError();
        }
        throw ToolException(
            "错误描述：message_handler 创建后发生错误\r\n"
            
            "错误代码：%lu\r\n",

            dwLastError
        );
    }
    set_clippable();
}

void CursorClipper::close() noexcept
{
    if (!get_instance().alive)
    {
        return;
    }
    /* 停止 message_handler 线程 */
    try
    {
        /* 向线程发布退出消息 */
        BOOL bStatus = PostThreadMessageW(
            GetThreadId(get_instance().message_handler),
            WM_QUIT,
            NULL,
            NULL
        );
        if (!bStatus)
        {
            TerminateThread(get_instance().message_handler, -1);
            throw ToolException(
                "错误描述：无法向 message_handler 下达退出命令，强行将其终止\r\n"

                "错误语句：%s\r\n"
                "错误代码：%lu\r\n",

                "BOOL bStatus = PostThreadMessageW(...)",
                GetLastError()
            );
        }
        DWORD dwStatus = WaitForSingleObject(get_instance().message_handler, 1000);
        if (dwStatus != WAIT_OBJECT_0)
        {
            TerminateThread(get_instance().message_handler, -1);
            throw ToolException(
                "错误描述：message_handler 无法正常退出，这可能是等待超时，强行将其终止\r\n"

                "WaitForSingleObject 返回值：%lu\r\n",

                dwStatus
            );
        }
    }
    catch (ToolException e)
    {
        e.notify();
    }
    /* message_handler finally */
    CloseHandle(get_instance().message_handler);
    get_instance().message_handler = NULL;
    get_instance().alive = false;
    /* 停止 cursor_clipper 线程 */
    try
    {
        set_clippable();
        DWORD dwStatus = WaitForSingleObject(get_instance().cursor_clipper, 1000);
        if (WAIT_OBJECT_0 != dwStatus)
        {
            TerminateThread(get_instance().cursor_clipper, -1);
            throw ToolException(
                "错误描述：cursor_clipper 无法正常退出，这可能是等待超时，强行将其终止\r\n"

                "WaitForSingleObject 返回值：%lu",

                dwStatus
            );
        }
    }
    catch (ToolException e)
    {
        e.notify();
    }
    /* cursor_clipper finally */
    ClipCursor(NULL);
    CloseHandle(get_instance().cursor_clipper);
    get_instance().cursor_clipper = NULL;
    /* 关闭事件 */
    CloseHandle(get_instance().event);
    get_instance().event = NULL;
    /* 关闭互斥量 */
    CloseHandle(get_instance().mutex);
    get_instance().mutex = NULL;
}

LPRECT CursorClipper::get_window_rect(LPRECT lpRect) noexcept
{
    DWORD dwStatus = WaitForSingleObject(get_instance().mutex, 200);
    if (WAIT_OBJECT_0 != dwStatus)
    {
        HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, lpRect);
        return lpRect;
    }
    memcpy_s(lpRect, sizeof(RECT), &get_instance().window_rect, sizeof(RECT));
    ReleaseMutex(get_instance().mutex);
    return lpRect;
}

void CursorClipper::set_window_rect(const RECT& rect) noexcept
{
    DWORD dwStatus = WaitForSingleObject(get_instance().mutex, 200);
    if (WAIT_OBJECT_0 != dwStatus)
    {
        return; /* 等待超时，放弃更新 */   
    }
    get_instance().window_rect.left = rect.left + 20;
    get_instance().window_rect.top = rect.top + 20;
    get_instance().window_rect.right = rect.right - 20;
    get_instance().window_rect.bottom = rect.bottom - 20;
    ReleaseMutex(get_instance().mutex);
}

LPWSTR CursorClipper::get_window_text(LPWSTR lpDst, SIZE_T cchWideChar) noexcept
{
    DWORD dwStatus = WaitForSingleObject(get_instance().mutex, 200);
    if (WAIT_OBJECT_0 != dwStatus)
    {
        *lpDst = L'\0';
        return lpDst;
    }
    wcscpy_s(lpDst, cchWideChar, get_instance().window_text);
    ReleaseMutex(get_instance().mutex);
    return lpDst;
}

void CursorClipper::set_window_text(LPCWSTR lpszWndTxt) noexcept
{
    DWORD dwStatus = WaitForSingleObject(get_instance().mutex, 200);
    if (WAIT_OBJECT_0 != dwStatus)
    {
        return;
    }
    wcscpy_s(get_instance().window_text, sizeof(get_instance().window_text) / sizeof(get_instance().window_text[0]), lpszWndTxt);
    ReleaseMutex(get_instance().mutex);
}

bool CursorClipper::is_clippable() noexcept
{
    if (WAIT_OBJECT_0 == WaitForSingleObject(get_instance().event, INFINITE))
    {
        return true;
    }
    return false;
}

void CursorClipper::set_clippable() noexcept
{
    SetEvent(get_instance().event);
}

void CursorClipper::reset_clippable() noexcept
{
    ResetEvent(get_instance().event);
}

bool CursorClipper::match_window_text(LPCWSTR lpszPattern) noexcept
{
    DWORD dwStatus = WaitForSingleObject(get_instance().mutex, 200);
    if (dwStatus != WAIT_OBJECT_0)
    {
        return false;
    }
    bool matched = false;
    if (0 == wcsicmp(lpszPattern, get_instance().window_text))
    {
        matched = true;
    }
    ReleaseMutex(get_instance().mutex);
    return matched;
}
