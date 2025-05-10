#include <CSOL_Utilities.hpp>
#include <Windows.h>
#include <cassert>
#include <errhandlingapi.h>
#include "Console.hpp"
#include "Controller.hpp"

using namespace CSOL_Utilities;

void Controller::HandleHotKeyEvent() noexcept
{
	BOOL bSuccess = TRUE;
	thread_local int64_t mode = '0'; /* 当前控制器的模式 */
	const char* conflict_hotkey{nullptr};
	uint32_t error_code{0};
	BOOL bObjInfo{FALSE};
	const int TOTAL_HOTKEY_COUNT = 6; /* 0 ~ 5 */
	const int idHotKeyStart = '0';
	int idHotKeyEnd = idHotKeyStart;
	UINT_PTR idTimer = 0;
	do
	{
		for (idHotKeyEnd = idHotKeyStart; idHotKeyEnd < idHotKeyStart + TOTAL_HOTKEY_COUNT; idHotKeyEnd++)
		{
			if (!RegisterHotKey(nullptr, idHotKeyEnd, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, idHotKeyEnd))
			{
				Console::Log(EXCEPTION_LEVEL::EL_ERROR,
							 "注册热键 Ctrl Alt Shift {} 失败，请检查是否存在热键冲突，错误代码 {}。", idHotKeyEnd,
							 GetLastError());
				break;
			}
			Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "成功注册热键 Ctrl Alt Shift {}。", idHotKeyEnd);
		}
		SetUserObjectInformationW(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &bObjInfo, sizeof(BOOL));
		idTimer = SetTimer(nullptr, 0, 5, nullptr);
		if (idTimer == 0)
		{
			Console::Log(EXCEPTION_LEVEL::EL_ERROR, "创建定时器失败。错误代码：%lu。", GetLastError());
			break;
		}
		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			if (msg.message == WM_TIMER)
			{
				/* 检测 CSOBanner 是否在运行 */
				HWND hCSOBannerWnd = FindWindowW(nullptr, L"CSOBanner");
				DWORD dwCSOBannerPId{0};
				HANDLE hCSOBannerProcess{nullptr};
				if (hCSOBannerWnd)
				{
					GetWindowThreadProcessId(hCSOBannerWnd, &dwCSOBannerPId);
				}
				if (dwCSOBannerPId)
				{
					hCSOBannerProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwCSOBannerPId);
				}
				if (hCSOBannerProcess)
				{
					TerminateProcess(hCSOBannerProcess, 0);
					Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "检测到 CSOBanner 正在运行，结束 CSOBanner。");
					CloseHandle(hCSOBannerProcess);
				}
			}
			else if (msg.message == WM_HOTKEY && mode != msg.wParam /* 需要进行模式切换 */ && msg.wParam >= '0' &&
					 msg.wParam <= '5')
			{
				if (mode == '1')
				{
				}
				else if (mode == '2')
				{
				}
				else if (mode == '3')
				{
				}
				else if (mode == '4')
				{
				}
				else if (mode == '5')
				{
				}
				/* 切换状态 */
				if (msg.wParam == '0')
				{
					Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "切换为 0 号模式。");
				}
				else if (msg.wParam == '1')
				{
					if (mode != '2') /* 如果是从 2 模式切换到 1 模式，则不对状态机进行重置 */
					{
					}
					Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "切换为 1 号模式。");
				}
				else if (msg.wParam == '2')
				{
					if (mode != '1') /* 如果是从 1 模式切换到 2 模式，则不对状态机进行重置 */
					{
					}
					Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "切换为 2 号模式。");
				}
				else if (msg.wParam == '3')
				{ /* 合成配件 */
					Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "切换为 3 号模式。");
				}
				else if (msg.wParam == '4')
				{ /* 购买物品 */
					Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "切换为 4 号模式。");
				}
				else if (msg.wParam == '5')
				{ /* 光标定位 */
					Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "切换为 5 号模式。");
				}
				mode = msg.wParam;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	while (false);
	while (idHotKeyEnd > idHotKeyStart)
	{
		UnregisterHotKey(nullptr, --idHotKeyEnd);
	}
	if (idTimer != 0)
	{
		KillTimer(nullptr, idTimer);
	}
	Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "线程 m_HotKeyEventHandler 退出。");
}
