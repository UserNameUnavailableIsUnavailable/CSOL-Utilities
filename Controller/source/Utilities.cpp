﻿#include "pch.hpp"

#include "Utilities.hpp"
#include "Exception.hpp"
#include "Global.hpp"

namespace CSOL_Utilities
{
	namespace Global
	{
		std::unordered_map<std::string, std::string> g_LanguagePackage;
	}

	std::filesystem::path GetModulePath(uintptr_t hMod)
	{
		static std::atomic_bool this_path_initialized{ false };
		static std::mutex this_path_write_lock;
		static std::wstring this_path(64, L'0');

		auto GetModulePathImpl = [] (uintptr_t hMod, std::wstring& path) {
			while (true)
			{
				HMODULE hModule = reinterpret_cast<HMODULE>(hMod);
				auto length = GetModuleFileNameW(hModule, path.data(), path.capacity());
				if (length == 0)
				{
					throw Exception(Translate("Utilities::ERROR_GetModuleFileNameW@1", GetLastError()));
				}
				else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				{
					/* If the function succeeds, the return value is the length of the string that is copied to the buffer, in characters, not including the terminating null character. */
					path.resize(length); /* 设置字符串长度，length 是不包含末尾空字符的字符串实际长度 */
					break;
				}
				else /* length != 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER */
				{
					path.resize(path.capacity() + path.capacity() / 2); /* 扩容为当前容量的 1.5 倍 */
				}
			}
		};

		if (hMod == 0) /* 查询当前可执行文件的路径 */
		{
			if (this_path_initialized.load(std::memory_order_acquire)) /* 已经查询过 */
			{
				return this_path;
			}
			else
			{
				std::lock_guard lk(this_path_write_lock);
				if (!this_path_initialized.load(std::memory_order_acquire))
				{
					GetModulePathImpl(hMod, this_path);
					this_path_initialized.store(true, std::memory_order_release);
				}
				return this_path;
			}
		}
		else
		{
			std::wstring ret(64, L'\0');
			GetModulePathImpl(hMod, ret);
			return ret;
		}
	}

	void CaptureWindowAsBmp(HWND hWnd, std::vector<uint8_t>& buffer)
	{
		if (!hWnd)
		{
			hWnd = GetDesktopWindow();
		}
		if (!IsWindow(hWnd))
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::ERROR_InvalidWindow"));
		}
		auto release_dc = [](HDC hdc) { ReleaseDC(NULL, hdc); };

		std::unique_ptr<std::remove_pointer_t<HDC>, decltype(release_dc)> hdcScreen(GetDC(NULL), release_dc);

		auto release_hwnd_dc = [&hWnd](HDC hdc) { ReleaseDC(NULL, hdc); };

		std::unique_ptr<std::remove_pointer_t<HDC>, decltype(release_hwnd_dc)> hdcWindow(GetDC(hWnd), release_hwnd_dc);

		if (!hdcWindow)
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::ERROR_GetDC@1", GetLastError()));
		}
		DWORD dwBmpSize = 0;
		DWORD dwSizeofDIB = 0;
		HANDLE hDIB = NULL;

		std::unique_ptr<std::remove_pointer_t<HDC>, BOOL (*)(HGDIOBJ)> hdcMemDC(CreateCompatibleDC(hdcWindow.get()),
																				&DeleteObject);

		if (!hdcMemDC)
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::ERROR_CreateCompatibleDC@1", GetLastError()));
		}
		if (IsIconic(hWnd))
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::ERROR_WindowIsMinimized"));
		}

		/* 获取窗口的相对区域 */
		RECT rcClient;
		if (!GetClientRect(hWnd, &rcClient))
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::GetClientRect@1", GetLastError()));
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
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::ERROR_StretchBlt@1", GetLastError()));
		}

		std::unique_ptr<std::remove_pointer_t<HBITMAP>, BOOL (*)(HGDIOBJ)> hbmScreen(
			CreateCompatibleBitmap(hdcWindow.get(), rcClient.right - rcClient.left, rcClient.bottom - rcClient.top),
			&DeleteObject);

		if (!hbmScreen)
		{
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::ERROR_CreateCompatibleDC@1", GetLastError()));
		}

		SelectObject(hdcMemDC.get(), hbmScreen.get());

		if (!BitBlt(hdcMemDC.get(), 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
					hdcWindow.get(), 0, 0, SRCCOPY))
		{
			throw Exception(Translate("Utilties::ERROR_BltBlt@1", GetLastError()));
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
			throw Exception(Translate("Utilities::CaptureWindowAsBmp::ERROR_GetDIBits@1", GetLastError()));
		}
	}

	std::wstring ConvertUtf8ToUtf16(const std::string& u8)
	{
		auto cchRequiredBufferSizeInChars = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, nullptr, 0);
		std::wstring u16(cchRequiredBufferSizeInChars, L'0');
        u16.resize(cchRequiredBufferSizeInChars);
		auto cchWritten = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, u16.data(), cchRequiredBufferSizeInChars);
		u16.resize(cchWritten - 1); /* cchWritten 是写入缓冲区的字符数，包含空字符 */
		return u16;
	}

	std::string ConvertUtf16ToUtf8(const std::wstring& u16)
	{
		auto cchRequiredBufferSizeInChars = WideCharToMultiByte(CP_UTF8, 0, u16.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string u8;
		u8.resize(cchRequiredBufferSizeInChars);
		auto cchWritten = WideCharToMultiByte(CP_UTF8, 0, u16.c_str(), -1, u8.data(), cchRequiredBufferSizeInChars, nullptr, nullptr);
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
		RECT rcScreen = {
			.left = 0,
			.top = 0
		};
		if (IsWindow(hWnd))
		{
			GetWindowInfo(hWnd, &windowInfo);
			RECT& rcClient = windowInfo.rcClient;
			rcScreen.right = GetSystemMetrics(SM_CXSCREEN);
			rcScreen.bottom = GetSystemMetrics(SM_CYSCREEN);
			LONG lDeltaX = (rcScreen.right - (rcClient.left + rcClient.right)) / 2;
			LONG lDeltaY = (rcScreen.bottom - (rcClient.top + rcClient.bottom)) / 2;
			rcClient.left += lDeltaX;
			rcClient.right += lDeltaX;
			rcClient.top += lDeltaY;
			rcClient.bottom += lDeltaY;
			MoveWindow(hWnd	, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, TRUE);
		}
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

	void EnumerateProcesses(std::function<bool (const PROCESSENTRY32W& process_entry)> callback)
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
					if (!continue_enum) break;
				}
				while (Process32NextW(hSnapshot, &process_entry));
			}
			CloseHandle(hSnapshot);
		}
	}

	/// 根据全局区域格式加载对应的语言包。
	void LoadLanguagePackage(std::unordered_map<std::string, std::string>& lang_pack)
	{
		auto locale = std::locale();
		auto locale_name = locale.name(); // e.g., zh-CN.UTF-8 (Windows)
		auto encoding_dot_index = locale_name.find('.');
		if (encoding_dot_index != std::string::npos)
		{
			locale_name.erase(encoding_dot_index);
		}
		std::filesystem::path package_path = std::filesystem::path(Global::LocaleResourcesDirectory) /
			(locale_name + ".json");
		if (package_path.is_relative())
		{
			package_path = GetModulePath().parent_path() / package_path;
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
		std::function<void(nlohmann::json&, std::string)> resolve =
			[&resolve](nlohmann::json& json_object, std::string name_space)
		{
			for (auto& [key, value] : json_object.items())
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
	/// @note `hProcess` 应具有 `SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION` 权限，否则函数将返回 `false`。若 `dwMilliseconds` 为 `0`，则不会检查进程是否结束并直接返回 `false`。
	bool SafeTerminateProcess(HANDLE hProcess, DWORD dwMilliseconds) noexcept
	{
		DWORD dwProcessId = GetProcessId(hProcess);

		if (dwProcessId == 0)
		{
			return false;
		}

		EnumWindows(/* 优先检索是否存在窗口，如果存在窗口则尝试通过向窗口发送 WM_CLOSE 结束进程 */
					[](HWND hWnd, LPARAM lParam) -> BOOL
					{
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
			}
			while (Thread32Next(hThreadSnap, &te32));
		}
		CloseHandle(hThreadSnap);

		if (dwMilliseconds != 0 && WAIT_OBJECT_0 == WaitForSingleObject(hProcess, dwMilliseconds))
		{
			return true;
		}

		return false;
	}
} // namespace CSOL_Utilities

#if defined(Test) && defined(MultiLingual)
int main()
{
	std::locale locale("zh-CN.UTF-8");
	std::locale::global(locale);
	CSOL_Utilities::LoadLanguagePackage();
	for (auto& [key, value] : lang_pack)
	{
		std::cout << key << ": " << value << std::endl;
	}
	return 0;
}
#endif
