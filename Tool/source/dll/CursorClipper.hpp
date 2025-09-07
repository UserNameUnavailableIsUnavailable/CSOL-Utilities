#pragma once
#include <Windows.h>

#define MAX_WINDOW_TEXT_LENGTH (512)

class CursorClipper
{
public:
    static CursorClipper& get_instance() noexcept;
    static void open();
    static void close() noexcept;
    static LPRECT get_window_rect(LPRECT lpRect) noexcept;
    static void set_window_rect(const RECT& rect) noexcept;
    static LPWSTR get_window_text(LPWSTR lpDst, SIZE_T cchWideChar) noexcept;
    static void set_window_text(LPCWSTR lpszWndTxt) noexcept;
    static bool match_window_text(LPCWSTR lpszPattern) noexcept;
    static inline bool is_alive() { return get_instance().alive; };
    static bool is_clippable() noexcept;
    static void set_clippable() noexcept;
    static void reset_clippable() noexcept;
private: /* 单例模式 */
    CursorClipper() = default;
    ~CursorClipper() = default;
    CursorClipper(const CursorClipper&) = delete;
    CursorClipper(const CursorClipper&&) = delete;
    CursorClipper& operator=(const CursorClipper&) = delete;
private:
    bool alive;
    HANDLE event;
    HANDLE mutex;
    HANDLE message_handler;
    HANDLE cursor_clipper;
    WCHAR window_text[MAX_WINDOW_TEXT_LENGTH];
    RECT window_rect;
    static LPCWSTR WINDOW_RECT_MUTEX_NAME_STRING;
    static void CALLBACK OnEvent(
        HWINEVENTHOOK hWinEventHook,
        DWORD dwEvent,
        HWND hWnd,
        LONG lObjectId,
        LONG lChildId,
        DWORD dwEventThreadId,
        DWORD dwEventTimeInMs
    ) noexcept;
    static DWORD CALLBACK MouseCursorClipper(LPVOID lpParam);
    static DWORD CALLBACK HandleMessage(LPVOID lpParam);
};

