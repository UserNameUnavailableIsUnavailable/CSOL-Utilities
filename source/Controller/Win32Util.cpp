#include "Exception.hpp"
#include "CSOL_Utilities.hpp"
#include <Windows.h>
#include <errhandlingapi.h>
#include <memory>
#include <winerror.h>

namespace CSOL_Utilities
{
BOOL CaptureWindowAsBmpW(const wchar_t* filename, HWND hWnd)
{
    std::unique_ptr<typename std::remove_pointer<HDC>::type, decltype(&DeleteDC)> hdc(CreateDCW(L"DISPLAY", NULL, NULL, NULL), &DeleteDC);
    int32_t ScrWidth = 0, ScrHeight = 0;
    RECT rect{};
    BOOL bMinimized = FALSE;
    if (hWnd == NULL)
    {
        ScrWidth = GetDeviceCaps(hdc.get(), HORZRES);
        ScrHeight = GetDeviceCaps(hdc.get(), VERTRES);
    }
    else
    {
        GetWindowRect(hWnd, &rect);
        ScrWidth = rect.right - rect.left;
        ScrHeight = rect.bottom - rect.top;
        if (IsIconic(hWnd))
        {
            bMinimized = TRUE;
            ShowWindow(hWnd, SW_NORMAL);
            SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
    }
    std::unique_ptr<typename std::remove_pointer<HDC>::type, decltype(&DeleteDC)> hMemDC(CreateCompatibleDC(hdc.get()), &DeleteDC);
    if (!hMemDC)
    {
        return FALSE;
    }
    std::unique_ptr<typename std::remove_pointer<HBITMAP>::type, decltype(&DeleteObject)> hBitmap(CreateCompatibleBitmap(hdc.get(), ScrWidth, ScrHeight), &DeleteObject);
    if (!hBitmap)
    {
        return FALSE;
    }
    SelectObject(hMemDC.get(), hBitmap.get());
    if (!BitBlt(hMemDC.get(), 0, 0, ScrWidth, ScrHeight, hdc.get(), rect.left, rect.top, SRCCOPY))
    {
        return FALSE;
    }
    /* 保存 BMP 到文件 */
    BITMAPFILEHEADER bmp_file_hdr{};
    BITMAPINFOHEADER bmp_info_hdr{};

    BITMAP bmp{};
    GetObjectW(hBitmap.get(), sizeof(bmp), &bmp);

    bmp_info_hdr.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info_hdr.biWidth = bmp.bmWidth;
    bmp_info_hdr.biHeight = bmp.bmHeight;
    bmp_info_hdr.biPlanes = bmp.bmPlanes;
    bmp_info_hdr.biBitCount = bmp.bmBitsPixel;
    bmp_info_hdr.biCompression = BI_RGB;
    bmp_info_hdr.biSizeImage = 0;
    bmp_info_hdr.biXPelsPerMeter = 0;
    bmp_info_hdr.biYPelsPerMeter = 0;
    bmp_info_hdr.biClrUsed = 0;
    bmp_info_hdr.biClrImportant = 0;
    
    DWORD dwBmpSize = bmp.bmWidth * bmp.bmHeight * 4;

    bmp_file_hdr.bfType = ((WORD)('M' << 8) | 'B');
    bmp_file_hdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    bmp_file_hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    std::size_t bufsz = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    std::unique_ptr<char[]> buf(new char[bufsz]);
    memcpy_s(buf.get(), bufsz, &bmp_file_hdr, sizeof(bmp_file_hdr));
    memcpy_s(buf.get() + sizeof(bmp_file_hdr), bufsz - sizeof(bmp_file_hdr), &bmp_info_hdr, sizeof(bmp_info_hdr));
    GetDIBits(hMemDC.get(), hBitmap.get(), 0L, (DWORD)ScrHeight, buf.get() + sizeof(bmp_file_hdr) + sizeof(bmp_info_hdr), (LPBITMAPINFO)&bmp_info_hdr, DIB_RGB_COLORS);

    std::unique_ptr<typename std::remove_pointer<HANDLE>::type, decltype(&CloseHandle)> hFile(
        CreateFileW(filename, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN, 0),
        &CloseHandle);
    if (!hFile || hFile.get() == INVALID_HANDLE_VALUE)
    {
        hFile.release(); /* 针对 INVALID_HANDLE_VALUE 情形将句柄设置为 NULL */
        return FALSE;
    }
    OVERLAPPED overlapped{};
    if (!WriteFileEx(hFile.get(), buf.get(), bufsz, &overlapped, [](DWORD dwErrorCode, DWORD dwBytesWritten, LPOVERLAPPED lpOverlapped) {}))
    {
        return FALSE;
    }
    return SleepEx(INFINITE, TRUE) == WAIT_IO_COMPLETION;
}


std::shared_ptr<wchar_t[]> ConvertUtf8ToUtf16(const char *byte_string)
{
    auto length = MultiByteToWideChar(CP_UTF8, 0, (char *)(byte_string), -1, nullptr, 0);
    std::shared_ptr<wchar_t[]> ret(new wchar_t[length]);
    MultiByteToWideChar(CP_UTF8, 0, (char *)byte_string, -1, ret.get(), length);
    return ret;
}

std::shared_ptr<char[]> ConvertUtf16ToUtf8(const wchar_t *byte_string)
{
    auto length = WideCharToMultiByte(CP_UTF8, 0, (wchar_t *)byte_string, -1, nullptr, 0, nullptr, nullptr);
    std::shared_ptr<char[]> ret(new char[length]);
    WideCharToMultiByte(CP_UTF8, 0, (wchar_t *)byte_string, -1, ret.get(), length, nullptr, nullptr);
    return ret;
}

/*
 * @brief 获取字符串型注册表项。
 * @param `lpPredefinedTopItem` 注册表预定义的项，如 `HKEY_CURRENT_USER`。
 * @param `lpSubItem` 注册表子目录项。
 * @param `lpItemName` 目录中的项名。
 * @return 项的值。
 * @note 例如，CSOL 安装路径保存在注册表项 HKEY_CURRENT_USER\Software\TCGame\csol\gamepath 中，则 `hPredefinedTopDir =
 * HKEY_CURRENT_USER`，`lpSubDir = Software\TCGame`，`lpItemName = gamepath`。
 */
std::shared_ptr<wchar_t[]> QueryRegistryStringItem(HKEY hPredefinedTopDir, LPCWSTR lpSubDir,
                                                                   LPCWSTR lpItemName)
{
    DWORD cbRequiredBufferSize{ 0 };
    const char *lpszPredefinedTopDirName = "";
    if (hPredefinedTopDir == HKEY_CLASSES_ROOT)
    {
        lpszPredefinedTopDirName = "HKEY_CLASSES_ROOT";
    }
    else if (hPredefinedTopDir == HKEY_CURRENT_CONFIG)
    {
        lpszPredefinedTopDirName = "HKEU_CURRENT_CONFIG";
    }
    else if (hPredefinedTopDir == HKEY_CURRENT_USER)
    {
        lpszPredefinedTopDirName = "HKEY_CURRENT_USER";
    }
    else if (hPredefinedTopDir == HKEY_LOCAL_MACHINE)
    {
        lpszPredefinedTopDirName = "HKEY_LOCAL_MACHINE";
    }
    else if (hPredefinedTopDir == HKEY_USERS)
    {
        lpszPredefinedTopDirName = "HKEY_USERS";
    }
    else
    {
        throw Exception("QueryRegistryStringItem 参数错误。");
    }
    LSTATUS ret{ERROR_SUCCESS};
    ret = RegGetValueW(hPredefinedTopDir, lpSubDir, lpItemName, RRF_RT_REG_SZ, nullptr, nullptr,
                       &cbRequiredBufferSize); // 获取需要的缓冲区长度
    if (ret != ERROR_SUCCESS)
    {
        throw Exception("查找注册表项失败。错误代码：%ld。", ret);
    }
    std::shared_ptr<wchar_t[]> lpwszResult(new wchar_t[(cbRequiredBufferSize - 1) / 2 + 1]{0});
    ret = RegGetValueW(hPredefinedTopDir, lpSubDir, lpItemName, RRF_RT_REG_SZ, nullptr, lpwszResult.get(),
                       &cbRequiredBufferSize); // 读取字符串
    if (ret != ERROR_SUCCESS)
    {
        throw Exception("查找注册表项失败。错误代码：%ld。", ret);
    }
    return lpwszResult;
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
}