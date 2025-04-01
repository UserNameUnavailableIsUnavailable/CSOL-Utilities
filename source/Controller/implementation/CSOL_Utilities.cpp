#include "CSOL_Utilities.hpp"
#include <vector>
#include "Exception.hpp"

namespace CSOL_Utilities
{
    void CaptureWindowAsBmp(HWND hWnd, std::vector<char>& buffer)
    {
        if (!hWnd)
        {
            hWnd = GetDesktopWindow();
        }
        if (!IsWindow(hWnd))
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：不是有效的窗口。错误代码：%lu。", GetLastError());
        }
        auto release_dc = [](HDC hdc) { ReleaseDC(NULL, hdc); };

        std::unique_ptr<std::remove_pointer_t<HDC>, decltype(release_dc)> hdcScreen(
            GetDC(NULL),
            release_dc
        );

        auto release_hwnd_dc = [&hWnd](HDC hdc) { ReleaseDC(NULL, hdc); };

        std::unique_ptr<std::remove_pointer_t<HDC>, decltype(release_hwnd_dc)> hdcWindow(GetDC(hWnd), release_hwnd_dc);

        if (!hdcWindow)
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：GetDC 失败。错误代码：%lu。", GetLastError());
        }
        DWORD dwBmpSize = 0;
        DWORD dwSizeofDIB = 0;
        HANDLE hDIB = NULL;

        std::unique_ptr<std::remove_pointer_t<HDC>, BOOL (*)(HGDIOBJ)> hdcMemDC(CreateCompatibleDC(hdcWindow.get()), &DeleteObject);

        if (!hdcMemDC)
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：GetDC 失败。错误代码：%lu。", GetLastError());
        }
        if (IsIconic(hWnd))
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：窗口处于最小化状态，无法捕获。");
        }

        /* 获取窗口的相对区域 */
        RECT rcClient;
        if (!GetClientRect(hWnd, &rcClient))
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：GetClientRect 失败。错误代码：%lu。", GetLastError());
        }

        /* 获取窗口左上角绝对坐标 */
        POINT ptLeftTopOfClient{ 0, 0 }; /* 需要将各字段坐标初始化为 0，表示左上角坐标 */
        ClientToScreen(hWnd, &ptLeftTopOfClient);

        /* 从整个屏幕截图中裁剪出客户端窗口部分 */
        SetStretchBltMode(hdcWindow.get(), HALFTONE);
        if (!StretchBlt(hdcWindow.get(), /* 窗口部分 */
            0, 0,
            rcClient.right, rcClient.bottom,
            hdcScreen.get(), /* 整个屏幕 */
            ptLeftTopOfClient.x, ptLeftTopOfClient.y,
            rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
            SRCCOPY))
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：StretchBltMode 失败。错误代码：%lu。", GetLastError());
        }

        std::unique_ptr<std::remove_pointer_t<HBITMAP>, BOOL (*)(HGDIOBJ)> hbmScreen(CreateCompatibleBitmap(hdcWindow.get(), rcClient.right - rcClient.left, rcClient.bottom - rcClient.top), &DeleteObject);

        if (!hbmScreen)
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：CreateCompatibleBitmap 失败。错误代码：%lu。", GetLastError());
        }

        SelectObject(hdcMemDC.get(), hbmScreen.get());

        if (!BitBlt(hdcMemDC.get(),
            0, 0,
            rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
            hdcWindow.get(),
            0, 0,
            SRCCOPY))
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：BitBlt 失败。错误代码：%lu。", GetLastError());
        }

        BITMAP bmpScreen{};
        GetObjectW(hbmScreen.get(), sizeof(BITMAP), &bmpScreen); /* 获取位图信息 */
        BITMAPINFOHEADER BitmapInfoHeader;
        BITMAPFILEHEADER BitmapFileHeader;

        BitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
        BitmapInfoHeader.biWidth = bmpScreen.bmWidth;
        BitmapInfoHeader.biHeight = bmpScreen.bmHeight;
        BitmapInfoHeader.biPlanes = 1;
        BitmapInfoHeader.biBitCount = 32; /* 每个像素 32 位 */
        BitmapInfoHeader.biCompression = BI_RGB;
        BitmapInfoHeader.biSizeImage = 0;
        BitmapInfoHeader.biXPelsPerMeter = 0;
        BitmapInfoHeader.biYPelsPerMeter = 0;
        BitmapInfoHeader.biClrUsed = 0;
        BitmapInfoHeader.biClrImportant = 0;

        dwBmpSize = ((bmpScreen.bmWidth * BitmapInfoHeader.biBitCount + 31) / 32) /* 宽度向上取整 */ * 4 * bmpScreen.bmHeight;

        auto required_size = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + dwBmpSize;

        buffer.resize(required_size);
        
        dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        BitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        BitmapFileHeader.bfSize = dwSizeofDIB;
        BitmapFileHeader.bfType = 0x4D42; // BM.
        BitmapFileHeader.bfReserved1 = 0;
        BitmapFileHeader.bfReserved2 = 0;

        auto file_header_offset = 0;
        auto info_header_offset = sizeof(BitmapFileHeader);
        auto bmp_body_offset = info_header_offset + sizeof(BitmapInfoHeader);

        memcpy_s(
            buffer.data() + file_header_offset, buffer.size() - file_header_offset,
            &BitmapFileHeader, sizeof(BitmapFileHeader)
        );

        memcpy_s(
            buffer.data() + info_header_offset, buffer.size() - info_header_offset,
            &BitmapInfoHeader, sizeof(BitmapInfoHeader)
        );
        if (!GetDIBits(hdcWindow.get(), hbmScreen.get(), 0,
            static_cast<UINT>(bmpScreen.bmHeight),
            buffer.data() + bmp_body_offset,
            reinterpret_cast<BITMAPINFO*>(&BitmapInfoHeader), DIB_RGB_COLORS))
        {
            throw Exception("CaptureWindowAsBmp(HWND, std::vector<char>&)：GetDIBits 失败。错误代码：%lu。", GetLastError());
        }
    }
}