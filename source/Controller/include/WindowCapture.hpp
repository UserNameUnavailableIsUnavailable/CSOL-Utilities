#pragma once
#include <Windows.h>
#include <cstddef>
#include <filesystem>

namespace CSOL_Utilities
{
class WindowCapture
{
	friend void swap(WindowCapture& wc1, WindowCapture& wc2) noexcept;
public:
	WindowCapture() = default;
	WindowCapture(const WindowCapture& wc);
	WindowCapture(WindowCapture&& wc) noexcept;
	~WindowCapture() noexcept
	{
		delete m_pData;
	}
	WindowCapture& Capture(HWND hWnd);
	//WindowCapture& Capture(const RECT& rect);
	void Save(std::filesystem::path p);
	void Save(HANDLE hFile);
	WindowCapture& operator=(WindowCapture wc);
	const void* GetData() const noexcept
	{
		return m_pData;
	}
	const BITMAPFILEHEADER& GetBitmapFileHeader() const noexcept
	{
		return m_BitmapFileHeader;
	}
	const BITMAPINFOHEADER& GetBitmapInfoHeader() const noexcept
	{
		return m_BitmapInfoHeader;
	}
	/* 当 Capture 抛出异常后，处理异常时应调用本函数清空成员 */
	void Clear() noexcept
	{
		memset(&m_BitmapFileHeader, 0, sizeof(BITMAPFILEHEADER));
		memset(&m_BitmapInfoHeader, 0, sizeof(BITMAPINFOHEADER));
		delete[] m_pData;
		m_pData = nullptr;
		m_BufferCapacity = 0;
	}
private:
	BITMAPFILEHEADER m_BitmapFileHeader{};
	BITMAPINFOHEADER m_BitmapInfoHeader{};
	void* m_pData{ nullptr };
	std::size_t m_BufferCapacity{ 0 };
};
}