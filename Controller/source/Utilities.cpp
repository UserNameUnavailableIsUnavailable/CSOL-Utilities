#include "Utilities.hpp"
#include "Exception.hpp"
#include "Global.hpp"
// #include "Console.hpp"

namespace CSOL_Utilities
{
namespace Global
{
std::unordered_map<std::string, std::string> g_LanguagePackage;
}

std::filesystem::path GetProcessImagePath(uintptr_t hMod)
{
    // 获取当前进程的路径不能简单地用
    // QueryFullProcessImageNameW，因为传入的句柄必须是
    // PROCESS_QUERY_LIMITED_INFORMATION 或 PROCESS_QUERY_INFORMATION 权限 而
    // GetCurrentProcess() 返回的伪句柄并不具备该权限 需要使用 GetModuleFileNameW
    // 来获取当前进程的路径
    auto get_self_image_path = [](std::wstring &path) {
        if (path.size() == 0)
        {
            path.resize(128); // 初始缓冲区
        }
        while (true)
        {
            auto length = GetModuleFileNameW(nullptr, path.data(), path.size());
            if (length > 0)
            {
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) // 缓冲区不足
                {
                    path.resize(path.size() + path.size() / 2); // 扩容
                    SetLastError(ERROR_SUCCESS);                // 重置错误码
                    continue;
                }
                else
                {
                    path.resize(length); // 调整为实际长度
                    return;
                }
            }
            else
            {
                auto err = GetLastError();
                throw Exception(Translate("ERROR_Win32_API@2", "GetModuleFileNameW", err));
            }
        }
    };

    auto get_other_image_path = [](uintptr_t hMod, std::wstring &path) {
        if (path.size() == 0)
        {
            path.resize(128); // 初始缓冲区
        }
        while (true)
        {
            HMODULE hModule = reinterpret_cast<HMODULE>(hMod);
            DWORD dwLength = path.size();
            auto ok = QueryFullProcessImageNameW(hModule, 0, path.data(), &dwLength);
            if (ok)
            {
                // MS Docs: On success, dwLength receives the number of characters
                // written to the buffer, not including the null-terminating
                // character.
                path.resize(dwLength); // 调整为实际长度
                return;
            }
            else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) // 缓冲区不足，返回 FALSE
                                                                  // 并设置错误码（官方文档并未对此给出说明，已经反馈）
            {
                path.resize(path.capacity() + path.capacity() / 2); // 扩容
                SetLastError(ERROR_SUCCESS);                        // 重置错误码
                continue;
            }
            else
            {
                auto err = GetLastError();
                throw Exception(Translate("ERROR_Win32_API@2", "QueryFullProcessImageNameW", err));
            }
        }
    };

    // 单独优化对当前可执行文件路径的查询
    static std::atomic_bool this_path_initialized{false};
    static std::mutex this_path_write_lock;
    static std::wstring this_path(128, L'0');
    if (hMod == 0 || hMod == reinterpret_cast<uintptr_t>(GetCurrentProcess())) // 查询当前可执行文件的路径
    {
        if (this_path_initialized.load(std::memory_order_acquire)) // 已经查询过
        {
            return this_path;
        }
        else
        {
            std::lock_guard lk(this_path_write_lock);
            if (!this_path_initialized.load(std::memory_order_acquire)) // 双重检查法
            {
                get_self_image_path(this_path);
                this_path_initialized.store(true, std::memory_order_release);
            }
            return this_path;
        }
    }
    else
    {
        std::wstring path(128, L'\0');
        get_other_image_path(hMod, path);
        return path;
    }
}

/// 将指定窗口捕获为 BMP 格式的位图数据。
/// @param window_handle 窗口句柄。如果为 `NULL`，则捕获整个屏幕。
/// @param buffer 用于接收 BMP 数据的缓冲区。
/// @throw `Exception` 捕获失败时抛出异常。
/// @note 对于高分辨率屏幕，需要在清单中配置高分辨率适配的选项。参见：https://learn.microsoft.com/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process。
void CaptureWindowAsBmp(HWND window_handle, std::vector<uint8_t> &buffer)
{
    DWORD dwScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    DWORD dwScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Console::Info(std::format("Screen Size: {}x{}", dwScreenWidth, dwScreenHeight));

    if (!window_handle)
    {
        window_handle = GetDesktopWindow();
    }
    if (!IsWindow(window_handle))
    {
        throw Exception(Translate("CaptureWindowAsBmp::ERROR_InvalidWindow"));
    }
    if (IsIconic(window_handle))
    {
        throw Exception(Translate("CaptureWindowAsBmp::ERROR_WindowIsMinimized"));
    }

    RECT rcLogicalClient;
    RECT rcPhysicalClient;
    POINT pt;

    if (!GetClientRect(window_handle, &rcLogicalClient))
    {
        throw Exception(Translate("ERROR_Win32_API@2", "GetClientRect", GetLastError()));
    }
    
    assert(rcLogicalClient.left == 0 && rcLogicalClient.top == 0);

    DWORD dwLogicalWidth = rcLogicalClient.right - rcLogicalClient.left;
    DWORD dwLogicalHeight = rcLogicalClient.bottom - rcLogicalClient.top;

    // Console::Info(std::format("Logical Client Size: {}x{}", dwLogicalWidth, dwLogicalHeight));

    pt.x = rcLogicalClient.left;
    pt.y = rcLogicalClient.top;
    ClientToScreen(window_handle, &pt);
    rcPhysicalClient.left = pt.x;
    rcPhysicalClient.top = pt.y;
    pt.x = rcLogicalClient.right;
    pt.y = rcLogicalClient.bottom;
    ClientToScreen(window_handle, &pt);
    rcPhysicalClient.right = pt.x;
    rcPhysicalClient.bottom = pt.y;
    
    DWORD dwPhysicalWidth = rcPhysicalClient.right - rcPhysicalClient.left;
    DWORD dwPhysicalHeight = rcPhysicalClient.bottom - rcPhysicalClient.top;
    // Console::Info(std::format("Physical Client Rect: left={}, top={}, right={}, bottom={}", rcPhysicalClient.left, rcPhysicalClient.top, rcPhysicalClient.right, rcPhysicalClient.bottom));
    // Console::Info(std::format("Physical Client Size: {}x{}", dwPhysicalWidth, dwPhysicalHeight));

    auto release_screen_dc = [](HDC hdc) { ReleaseDC(nullptr, hdc); };
    std::unique_ptr<std::remove_pointer_t<HDC>, decltype(release_screen_dc)> hdcScreen(GetDC(nullptr), release_screen_dc);
    if (!hdcScreen)
    {
        throw Exception(Translate("ERROR_Win32_API@2", "GetDC", GetLastError()));
    }
    // auto release_hwnd_dc = [&window_handle](HDC hdc) { ReleaseDC(window_handle, hdc); };
    // std::unique_ptr<std::remove_pointer_t<HDC>, decltype(release_hwnd_dc)> hdcWindow(GetDC(window_handle), release_hwnd_dc);
    // if (!hdcWindow)
    // {
    //     throw Exception(Translate("ERROR_Win32_API@2", "GetDC", GetLastError()));
    // }
    std::unique_ptr<std::remove_pointer_t<HDC>, BOOL (*)(HGDIOBJ)> hdcMemDC(CreateCompatibleDC(hdcScreen.get()), &DeleteObject);
    if (!hdcMemDC)
    {
        throw Exception(Translate("ERROR_Win32_API@2", "CreateCompatibleDC", GetLastError()));
    }

    // SetStretchBltMode(hdcWindow.get(), HALFTONE); // 设置拉伸模式为高质量
    // 从屏幕 DC 复制到窗口的 DC
    // if (!StretchBlt(hdcWindow.get(), 0, 0, dwPhysicalWidth, dwPhysicalHeight, hdcScreen.get(), rcPhysicalClient.left, rcPhysicalClient.top, dwPhysicalWidth, dwPhysicalHeight, SRCCOPY))
    // {
    //     throw Exception(Translate("ERROR_Win32_API@2", "StretchBlt", GetLastError()));
    // }

    // 创建兼容位图
    std::unique_ptr<std::remove_pointer_t<HBITMAP>, BOOL (*)(HGDIOBJ)> hdcBitmap(
        CreateCompatibleBitmap(hdcScreen.get(), dwPhysicalWidth, dwPhysicalHeight), &DeleteObject);
    if (!hdcBitmap)
    {
        throw Exception(Translate("ERROR_Win32_API@2", "CreateCompatibleBitmap", GetLastError()));
    }

    SelectObject(hdcMemDC.get(), hdcBitmap.get()); // 将位图选入内存 DC

    // 从窗口的 DC 复制到内存 DC
    if (!BitBlt(hdcMemDC.get(), 0, 0, dwPhysicalWidth, dwPhysicalHeight, hdcScreen.get(), rcPhysicalClient.left, rcPhysicalClient.top, SRCCOPY))
    {
        throw Exception(Translate("ERROR_Win32_API@2", "BitBlt", GetLastError()));
    }

    BITMAP bmp;
    GetObjectW(hdcBitmap.get(), sizeof(BITMAP), &bmp); /* 获取位图信息 */

    BITMAPFILEHEADER bfh{
        .bfType = 0x4D42, // BM
        .bfSize = 0,      // 之后会重新计算
        .bfReserved1 = 0,
        .bfReserved2 = 0,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
    };
    BITMAPINFOHEADER bih{
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = bmp.bmWidth,
        .biHeight = bmp.bmHeight,
        .biPlanes = 1,
        .biBitCount = 32, /* 每个像素 32 位 */
        .biCompression = BI_RGB,
        .biSizeImage = 0,
        .biXPelsPerMeter = 0,
        .biYPelsPerMeter = 0,
        .biClrUsed = 0,
        .biClrImportant = 0,
    };
    DWORD dwBmpSize = ((bmp.bmWidth * bih.biBitCount + 31) / 32) /* 宽度向上取整 */
                       * 4 * bmp.bmHeight;                     /* 每行字节数 * 高度 */
    DWORD dwTotalSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    bfh.bfSize = dwTotalSize;
    buffer.resize(dwTotalSize);
    memcpy(buffer.data(), &bfh, sizeof(BITMAPFILEHEADER));
    memcpy(buffer.data() + sizeof(BITMAPFILEHEADER), &bih, sizeof(BITMAPINFOHEADER));
    BITMAPINFO bmi;
    bmi.bmiHeader = bih;
    bmi.bmiColors[0] = {0, 0, 0, 0};
    if (!GetDIBits(hdcScreen.get(), hdcBitmap.get(), 0, (UINT)bmp.bmHeight,
                   buffer.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), &bmi,
                   DIB_RGB_COLORS))
    {
        throw Exception(Translate("ERROR_Win32_API@2", "GetDIBits", GetLastError()));
    }
    std::ofstream ofs("debug.bmp", std::ios::binary);
    ofs.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
    ofs.close();
}

std::wstring ConvertUtf8ToUtf16(const std::string &u8)
{
    auto cchRequiredBufferSizeInChars = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, nullptr, 0);
    std::wstring u16(cchRequiredBufferSizeInChars, L'0');
    u16.resize(cchRequiredBufferSizeInChars);
    auto cchWritten = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, u16.data(), cchRequiredBufferSizeInChars);
    u16.resize(cchWritten - 1); /* cchWritten 是写入缓冲区的字符数，包含空字符 */
    return u16;
}

std::string ConvertUtf16ToUtf8(const std::wstring &u16)
{
    auto cchRequiredBufferSizeInChars = WideCharToMultiByte(CP_UTF8, 0, u16.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string u8;
    u8.resize(cchRequiredBufferSizeInChars);
    auto cchWritten =
        WideCharToMultiByte(CP_UTF8, 0, u16.c_str(), -1, u8.data(), cchRequiredBufferSizeInChars, nullptr, nullptr);
    u8.resize(cchWritten - 1); /* cchWritten 是写入缓冲区的字符数，包含空字符 */
    return u8;
}

void RemoveWindowBorder(HWND hWnd) noexcept
{
    WINDOWINFO windowInfo;
    if (IsWindow(hWnd))
    {
        GetWindowInfo(hWnd, &windowInfo);
        DWORD dwStyle = windowInfo.dwStyle & ~WS_CAPTION;
        ShowWindow(hWnd, SW_SHOW);
        SetWindowLongPtrW(hWnd, GWL_STYLE, dwStyle);
        UpdateWindow(hWnd);
    }
}

void CenterWindow(HWND hWnd) noexcept
{
    WINDOWINFO windowInfo;
    if (IsWindow(hWnd))
    {
        RECT rcScreen{};
        GetWindowInfo(hWnd, &windowInfo);
        RECT &rcClient = windowInfo.rcClient;
        rcScreen.right = GetSystemMetrics(SM_CXSCREEN);
        rcScreen.bottom = GetSystemMetrics(SM_CYSCREEN);
        LONG lDeltaX = (rcScreen.right - (rcClient.left + rcClient.right)) / 2;
        LONG lDeltaY = (rcScreen.bottom - (rcClient.top + rcClient.bottom)) / 2;
        rcClient.left += lDeltaX;
        rcClient.right += lDeltaX;
        rcClient.top += lDeltaY;
        rcClient.bottom += lDeltaY;
        MoveWindow(hWnd, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
                   TRUE);
    }
}

BOOL IsRunningAsAdmin() noexcept
{
    BOOL bRet = FALSE;
    PSID admin_group = NULL;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0,
                                 0, &admin_group))
    {
        CheckTokenMembership(NULL, admin_group, &bRet);
        FreeSid(admin_group);
    }
    return bRet;
}

DWORD
GetProcessIdByName(const std::wstring &process_name)
{
    DWORD dwPId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W process_entry;
        process_entry.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnapshot, &process_entry))
        {
            do
            {
                if (_wcsicmp(process_entry.szExeFile, process_name.c_str()) == 0)
                {
                    dwPId = process_entry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(hSnapshot, &process_entry));
        }
        CloseHandle(hSnapshot);
    }
    return dwPId;
}

void EnumerateProcesses(std::function<bool(const PROCESSENTRY32W &process_entry)> callback)
{
    DWORD dwPId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W process_entry;
        process_entry.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnapshot, &process_entry))
        {
            do
            {
                auto continue_enum = callback(process_entry);
                if (!continue_enum)
                    break;
            } while (Process32NextW(hSnapshot, &process_entry));
        }
        CloseHandle(hSnapshot);
    }
}

/// 根据全局区域格式加载对应的语言包。
void LoadLanguagePackage(std::unordered_map<std::string, std::string> &lang_pack)
{
    auto locale = std::locale();
    auto locale_name = locale.name(); // e.g., zh-CN.UTF-8 (Windows)
    auto encoding_dot_index = locale_name.find('.');
    if (encoding_dot_index != std::string::npos)
    {
        locale_name.erase(encoding_dot_index);
    }
    std::filesystem::path package_path = std::filesystem::path(Global::LocaleResourcesDir) / (locale_name + ".json");
    if (package_path.is_relative())
    {
        package_path = GetProcessImagePath().parent_path() / package_path;
    }
    if (!std::filesystem::is_regular_file(package_path))
        throw Exception("Language package not found or is not an regular file.");
    std::ifstream ifs(package_path, std::ios::binary | std::ios::in | std::ios::ate);
    if (!ifs.is_open())
        throw Exception("Unable to open language package file.");
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    assert(size != -1);
    std::string json_string;
    json_string.resize(size); /* json 文件大小为 size 字节 */
    ifs.read(json_string.data(), size);
    nlohmann::json json_obj = nlohmann::json::parse(json_string);

    /* 递归解析 JSON */
    std::function<void(nlohmann::json &, std::string)> resolve = [&resolve](nlohmann::json &json_object,
                                                                            std::string name_space) {
        for (auto &[key, value] : json_object.items())
        {
            std::string name = name_space.empty() ? key : name_space + "::" + key;
            if (value.is_object())
            {
                resolve(value, name);
            }
            else if (value.is_string())
            {
                Global::g_LanguagePackage[name] = value.get<std::string>();
            }
            else
            {
                throw Exception("Invalid language package format.");
            }
        }
    };

    resolve(json_obj, "");
}

/// 安全地结束某个进程。
/// @param hProcess 进程句柄。
/// @param dwMilliseconds 等待进程结束的时延。
/// @return 进程是否在指定时间内结束。
/// @note `hProcess` 应具有 `SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION`
/// 权限，否则函数将返回 `false`。若 `dwMilliseconds` 为
/// `0`，则不会检查进程是否结束并直接返回 `false`。
bool SafeTerminateProcess(HANDLE hProcess, DWORD dwMilliseconds) noexcept
{
    DWORD dwProcessId = GetProcessId(hProcess);

    if (dwProcessId == 0)
    {
        return false;
    }

    EnumWindows(/* 优先检索是否存在窗口，如果存在窗口则尝试通过向窗口发送
                   WM_CLOSE 结束进程 */
                [](HWND hWnd, LPARAM lParam) -> BOOL {
                    DWORD dwOwnerProcessId;
                    GetWindowThreadProcessId(hWnd, &dwOwnerProcessId);
                    if (dwOwnerProcessId == static_cast<DWORD>(lParam))
                    {
                        PostMessageW(hWnd, WM_CLOSE, 0, 0);
                    }
                    return true;
                },
                static_cast<LPARAM>(dwProcessId));

    if (dwMilliseconds != 0 && WAIT_OBJECT_0 == WaitForSingleObject(hProcess, dwMilliseconds))
    {
        return true;
    }

    /* 尝试通过向各个线程发送 WM_QUIT 消息结束进程 */
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (!hThreadSnap)
        return false;
    THREADENTRY32 te32{.dwSize = sizeof(THREADENTRY32)};
    if (Thread32First(hThreadSnap, &te32))
    {
        do
        {
            if (te32.th32OwnerProcessID == dwProcessId)
            {
                DWORD dwThreadId = te32.th32ThreadID;
                PostThreadMessageW(dwThreadId, WM_QUIT, 0, 0); /* 向各个线程发送 WM_QUIT 消息 */
            }
        } while (Thread32Next(hThreadSnap, &te32));
    }
    CloseHandle(hThreadSnap);

    if (dwMilliseconds != 0 && WAIT_OBJECT_0 == WaitForSingleObject(hProcess, dwMilliseconds))
    {
        return true;
    }

    return false;
}

void CenterWindowClientArea(HWND hWnd) noexcept
{
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

    const POINT ptScreenCenter{// screen center
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

    const POINT ptClientCenter{// client center
                               .x = rcClient.left + (rcClient.right - rcClient.left) / 2,
                               .y = rcClient.top + (rcClient.bottom - rcClient.top) / 2};

    // Offset from client center to screen center.
    auto dx = ptScreenCenter.x - ptClientCenter.x;
    auto dy = ptScreenCenter.y - ptClientCenter.y;

    // New window position.
    const POINT ptNewWindowTopLeft{.x = rcWindow.left + dx, .y = rcWindow.top + dy};

    SetWindowPos(hWnd, nullptr, ptNewWindowTopLeft.x, ptNewWindowTopLeft.y, 0, 0,
                 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
} // namespace CSOL_Utilities
