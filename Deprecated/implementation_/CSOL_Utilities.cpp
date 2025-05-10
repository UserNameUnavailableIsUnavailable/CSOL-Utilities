#include "CSOL_Utilities.hpp"
#include <TlHelp32.h>
#include <codecvt>
#include <errhandlingapi.h>
#include <filesystem>
#include <libloaderapi.h>
#include <locale>
#include <memory>
#include <minwindef.h>
#include <opencv2/core.hpp>
#include <string>
#include <stringapiset.h>
#include <vector>
#include <winerror.h>
#include <winnls.h>
#include "Exception.hpp"
#include "MultiLingual.hpp"

namespace CSOL_Utilities
{

	namespace Opt
	{
		std::string g_DetectMode = "classification";
		std::string g_OCRDetectionModelPath;
		std::string g_OCRRecognitionModelPath;
		std::string g_OCRDictionaryPath;
		std::string g_OCRKeywordsPath;
		unsigned int g_MaxWaitTimeInGameRoom = 60 * 15;
		bool g_bSwitchToNormalModeAfterReconnection = true;
		extern bool g_bTerminateGameProcessIfLoadingTimeout;
	} // namespace Opt

	std::filesystem::path GetModulePath(uintptr_t hMod)
	{
		std::wstring module_path(128, L'\0');
		while (true)
		{
			HMODULE hModule = reinterpret_cast<HMODULE>(hMod);
			auto length = GetModuleFileNameW(hModule, module_path.data(), module_path.capacity());
			if (length == 0)
			{
				throw Exception(Translate("Utilities::FAILED_TO_GET_MODULE_PATH", GetLastError()));
			}
			else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				module_path.resize(length + 1); /* If the function succeeds, the return value is the length of the string that is copied to the buffer, in characters, not including the terminating null character. */
				return module_path;
			}
			else
			{
				module_path.resize(module_path.capacity() + module_path.capacity() / 2, L'\0');
			}
		}
	}

	std::string RecognizeCharactersInImage(std::vector<uint8_t> buffer, int padding, int maxSideLen,
										   float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle,
										   bool mostAngle)
	{
		return "";
	}

	void CaptureWindowAsBmp(HWND hWnd, std::vector<uint8_t>& buffer)
	{
		if (!hWnd)
		{
			hWnd = GetDesktopWindow();
		}
		if (!IsWindow(hWnd))
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::FAILED_TO_CAPTURE_WINDOW"));
		}
		auto release_dc = [](HDC hdc) { ReleaseDC(NULL, hdc); };

		std::unique_ptr<std::remove_pointer_t<HDC>, decltype(release_dc)> hdcScreen(GetDC(NULL), release_dc);

		auto release_hwnd_dc = [&hWnd](HDC hdc) { ReleaseDC(NULL, hdc); };

		std::unique_ptr<std::remove_pointer_t<HDC>, decltype(release_hwnd_dc)> hdcWindow(GetDC(hWnd), release_hwnd_dc);

		if (!hdcWindow)
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::FAILKED_TO_GET_WINDOW_DC", GetLastError()));
		}
		DWORD dwBmpSize = 0;
		DWORD dwSizeofDIB = 0;
		HANDLE hDIB = NULL;

		std::unique_ptr<std::remove_pointer_t<HDC>, BOOL (*)(HGDIOBJ)> hdcMemDC(CreateCompatibleDC(hdcWindow.get()),
																				&DeleteObject);

		if (!hdcMemDC)
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::FAILED_TO_CREATE_COMPATIBLE_DC", GetLastError()));
		}
		if (IsIconic(hWnd))
		{
			throw Exception("Utilities::CaptureWindowAsBmp::WINDOW_TO_BE_CAPTURED_IS_MINIMIZED");
		}

		/* 获取窗口的相对区域 */
		RECT rcClient;
		if (!GetClientRect(hWnd, &rcClient))
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::FAILED_TO_GET_CLIENT_RECT", GetLastError()));
		}

		/* 获取窗口左上角绝对坐标 */
		POINT ptLeftTopOfClient{0, 0}; /* 需要将各字段坐标初始化为 0，表示左上角坐标 */
		ClientToScreen(hWnd, &ptLeftTopOfClient);

		/* 从整个屏幕截图中裁剪出客户端窗口部分 */
		SetStretchBltMode(hdcWindow.get(), HALFTONE);
		if (!StretchBlt(hdcWindow.get(), /* 窗口部分 */
						0, 0, rcClient.right, rcClient.bottom, hdcScreen.get(), /* 整个屏幕 */
						ptLeftTopOfClient.x, ptLeftTopOfClient.y, rcClient.right - rcClient.left,
						rcClient.bottom - rcClient.top, SRCCOPY))
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::FAILED_TO_STRETCH_BLT", GetLastError()));
		}

		std::unique_ptr<std::remove_pointer_t<HBITMAP>, BOOL (*)(HGDIOBJ)> hbmScreen(
			CreateCompatibleBitmap(hdcWindow.get(), rcClient.right - rcClient.left, rcClient.bottom - rcClient.top),
			&DeleteObject);

		if (!hbmScreen)
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::FAILED_TO_CREATE_COMPATIBLE_DC", GetLastError()));
		}

		SelectObject(hdcMemDC.get(), hbmScreen.get());

		if (!BitBlt(hdcMemDC.get(), 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
					hdcWindow.get(), 0, 0, SRCCOPY))
		{
			throw Exception(Translate("Utilties::FAILED_TO_BIT_BLT", GetLastError()));
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

		dwBmpSize =
			((bmpScreen.bmWidth * BitmapInfoHeader.biBitCount + 31) / 32) /* 宽度向上取整 */ * 4 * bmpScreen.bmHeight;

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

		memcpy_s(buffer.data() + file_header_offset, buffer.size() - file_header_offset, &BitmapFileHeader,
				 sizeof(BitmapFileHeader));

		memcpy_s(buffer.data() + info_header_offset, buffer.size() - info_header_offset, &BitmapInfoHeader,
				 sizeof(BitmapInfoHeader));
		if (!GetDIBits(hdcWindow.get(), hbmScreen.get(), 0, static_cast<UINT>(bmpScreen.bmHeight),
					   buffer.data() + bmp_body_offset, reinterpret_cast<BITMAPINFO*>(&BitmapInfoHeader),
					   DIB_RGB_COLORS))
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::FAILED_TO_GET_DI_BITS", GetLastError()));
		}
	}

	std::wstring ConvertUtf8ToUtf16(const std::string& u8)
	{
		auto cchRequiredBufferSizeInChars = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, nullptr, 0);
		std::wstring u16(cchRequiredBufferSizeInChars, L'0');
		MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, u16.data(), cchRequiredBufferSizeInChars);
		return u16;
	}

	std::string ConvertUtf16ToUtf8(const std::wstring& u16)
	{
		auto length = WideCharToMultiByte(CP_UTF8, 0, u16.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string u8;
		WideCharToMultiByte(CP_UTF8, 0, u16.c_str(), -1, u8.data(), length, nullptr, nullptr);
		return u8;
	}

	BOOL IsRunningAsAdmin() noexcept
	{
		BOOL bRet = FALSE;
		PSID admin_group = NULL;

		SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
		if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0,
									 0, 0, &admin_group))
		{
			CheckTokenMembership(NULL, admin_group, &bRet);
			FreeSid(admin_group);
		}
		return bRet;
	}

	DWORD GetProcessIdByName(const std::wstring& process_name)
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
				}
				while (Process32NextW(hSnapshot, &process_entry));
			}
			CloseHandle(hSnapshot);
		}
		return dwPId;
	}
} // namespace CSOL_Utilities
