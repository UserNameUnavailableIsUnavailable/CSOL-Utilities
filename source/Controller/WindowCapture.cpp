#include <WindowCapture.hpp>
#include <cinttypes>
#include <algorithm>
#include <Exception.hpp>
#include <CSOL_Utilities.hpp>
#include <cassert>

namespace CSOL_Utilities
{
void swap(WindowCapture& wc1, WindowCapture& wc2) noexcept
{
	BITMAPFILEHEADER bfh(wc1.m_BitmapFileHeader);
	memcpy(&wc1.m_BitmapFileHeader, &wc2.m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));
	memcpy(&wc2.m_BitmapFileHeader, &bfh, sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER bih(wc1.m_BitmapInfoHeader);
	memcpy(&wc1.m_BitmapInfoHeader, &wc2.m_BitmapInfoHeader, sizeof(BITMAPINFOHEADER));
	memcpy(&wc2.m_BitmapInfoHeader, &bih, sizeof(BITMAPINFOHEADER));
	void* pData;
	pData = wc1.m_pData;
	wc1.m_pData = wc2.m_pData;
	wc2.m_pData = pData;
	std::size_t size = wc1.m_BufferCapacity;
	wc1.m_BufferCapacity = wc2.m_BufferCapacity;
	wc2.m_BufferCapacity = size;
}

WindowCapture::WindowCapture(const WindowCapture& wc)
{
	memcpy(&m_BitmapFileHeader, &wc.m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));
	memcpy(&m_BitmapInfoHeader, &wc.m_BitmapInfoHeader, sizeof(BITMAPINFOHEADER));
	m_BufferCapacity = wc.m_BufferCapacity;
	m_pData = new std::uint8_t[m_BufferCapacity];
	memcpy(m_pData, wc.m_pData, m_BufferCapacity);
}

WindowCapture::WindowCapture(WindowCapture&& wc) noexcept : WindowCapture()
{
	swap(*this, wc);
}

WindowCapture& WindowCapture::operator=(WindowCapture wc)
{
	swap(*this, wc);
	return *this;
}
/*
 *@brief 截取指定窗口的图像。
 *@param `hWnd` 窗口句柄
 *@return `WindowCapture` 对象
 *@remark `hWnd` 可以为 `nullptr`，此时本函数将截取桌面窗口（覆盖整个屏幕）的图像。
 */
WindowCapture& WindowCapture::Capture(HWND hWnd)
{
    if (!hWnd)
    {
        hWnd = GetDesktopWindow();
    }
    if (!IsWindow(hWnd))
    {
		throw Exception("WindowCapture::Capture(HWND)：不是有效的窗口。错误代码：%lu。", GetLastError());
    }
    UniqueHandle hdcScreen(GetDC(NULL), [](HDC hdc) { ReleaseDC(NULL, hdc); });
    UniqueHandle hdcWindow(GetDC(hWnd), [hWnd](HDC hdc) { ReleaseDC(hWnd, hdc); });
    if (!hdcWindow)
    {
		throw Exception("WindowCapture::Capture(HWND)：GetDC 失败。错误代码：%lu。", GetLastError());
    }
    DWORD dwBmpSize = 0;
    DWORD dwSizeofDIB = 0;
    HANDLE hDIB = NULL;

    UniqueHandle hdcMemDC(CreateCompatibleDC(hdcWindow.get()), &DeleteObject);
    if (!hdcMemDC)
    {
		throw Exception("WindowCapture::Capture(HWND)：CreateCompatibleDC 失败。错误代码：%lu。", GetLastError());
    }
    if (IsIconic(hWnd))
    {
        throw Exception("WindowCapture::Capture(HWND)：窗口处于最小化状态，无法捕获。");
    }

    /* 获取窗口的相对区域 */
    RECT rcClient;
    if (!GetClientRect(hWnd, &rcClient))
    {
        throw Exception("WindowCapture::Capture(HWND)：GetClientRect 失败。错误代码：%lu。", GetLastError());
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
		throw Exception("WindowCapture::Capture(HWND)：SetStretchBltMode 失败。错误代码：%lu。", GetLastError());
    }

    UniqueHandle hbmScreen(CreateCompatibleBitmap(hdcWindow.get(), rcClient.right - rcClient.left, rcClient.bottom - rcClient.top), &DeleteObject);
    if (!hbmScreen)
    {
        throw Exception("WindowCapture::Capture(const RECT&)：CreateCompatibleBitmap 失败。错误代码：%lu。", GetLastError());
    }

    SelectObject(hdcMemDC.get(), hbmScreen.get());

    if (!BitBlt(hdcMemDC.get(),
        0, 0,
        rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
        hdcWindow.get(),
        0, 0,
        SRCCOPY))
    {
        throw Exception("WindowCapture::Capture(const RECT&)：BitBlt 失败。错误代码：%lu。", GetLastError());
    }

    BITMAP bmpScreen{};
    GetObjectW(hbmScreen.get(), sizeof(BITMAP), &bmpScreen); /* 获取位图信息 */

    m_BitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_BitmapInfoHeader.biWidth = bmpScreen.bmWidth;
    m_BitmapInfoHeader.biHeight = bmpScreen.bmHeight;
    m_BitmapInfoHeader.biPlanes = 1;
    m_BitmapInfoHeader.biBitCount = 32; /* 每个像素 32 位 */
    m_BitmapInfoHeader.biCompression = BI_RGB;
    m_BitmapInfoHeader.biSizeImage = 0;
    m_BitmapInfoHeader.biXPelsPerMeter = 0;
    m_BitmapInfoHeader.biYPelsPerMeter = 0;
    m_BitmapInfoHeader.biClrUsed = 0;
    m_BitmapInfoHeader.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * m_BitmapInfoHeader.biBitCount + 31) / 32) /* 宽度向上取整 */ * 4 * bmpScreen.bmHeight;
    
    if (dwBmpSize > m_BufferCapacity)
    {
        m_pData = new uint8_t[dwBmpSize];
        m_BufferCapacity = dwBmpSize;
    }
    
    if (!GetDIBits(hdcWindow.get(), hbmScreen.get(), 0,
        static_cast<UINT>(bmpScreen.bmHeight),
        m_pData,
        reinterpret_cast<BITMAPINFO*>(&m_BitmapInfoHeader), DIB_RGB_COLORS))
    {
        throw Exception("WindowCapture::Capture(const RECT&)：GetDIBits 失败。错误代码：%lu。", GetLastError());
    }

    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    m_BitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    m_BitmapFileHeader.bfSize = dwSizeofDIB;
    m_BitmapFileHeader.bfType = 0x4D42; // BM.
    m_BitmapFileHeader.bfReserved1 = 0;
    m_BitmapFileHeader.bfReserved2 = 0;
    return *this;
}

void WindowCapture::Save(std::filesystem::path p)
{
	UniqueHandle hFile = CreateFileW(p.wstring().c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		throw Exception("WindowCapture::Save(std::filesystem::path)：CreateFileW 失败。错误代码：%lu。", GetLastError());
	}
    Save(hFile.get());
}

void WindowCapture::Save(HANDLE hFile)
{
    if (!hFile || hFile == INVALID_HANDLE_VALUE)
    {
        throw Exception("WindowCapture::Save(HANDLE)：hFile 非法。");
    }
    /* mandatory in Windows 7 */
    DWORD dwBytesWritten1;
    DWORD dwBytesWritten2;
    DWORD dwBytesWritten3;
    if (
        !((WriteFile(hFile, &m_BitmapFileHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten1, NULL) || GetLastError() == ERROR_IO_PENDING)) ||
		!((WriteFile(hFile, &m_BitmapInfoHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten2, NULL) || GetLastError() == ERROR_IO_PENDING)) ||
		!((WriteFile(hFile, m_pData, m_BitmapFileHeader.bfSize, &dwBytesWritten3, NULL) || GetLastError() == ERROR_IO_PENDING))
	)
	{
		throw Exception("WindowCapture::Save(HANDLE)：WriteFile 失败。错误代码：%lu。", GetLastError());
	}
}

}

