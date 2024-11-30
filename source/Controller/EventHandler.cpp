#include "Console.hpp"
#include "Controller.hpp"
#include <CSOL_Utilities.hpp>
#include <Windows.h>
#include <cassert>
#include <errhandlingapi.h>

using namespace CSOL_Utilities;

void Controller::HandleHotKeyEvent() noexcept
{
    BOOL bSuccess = TRUE;
    thread_local int64_t mode = '0';
    const char* conflict_hotkey{ nullptr };
    uint32_t error_code{ 0 };
    BOOL bObjInfo{ FALSE };
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
                Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_ERROR, "注册热键 {Ctrl Alt Shift %c} 失败，请检查是否存在热键冲突。错误代码：%lu。", idHotKeyEnd, GetLastError());
                break;
            }
			Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "成功注册热键 {Ctrl Alt Shift %c}。", idHotKeyEnd);
        }
		SetUserObjectInformationW(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &bObjInfo, sizeof(BOOL));
        idTimer = SetTimer(nullptr, 0, 5, nullptr);
        if (idTimer == 0)
        {
            Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_ERROR, "创建定时器失败。错误代码：%lu。", GetLastError());
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
					Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "检测到 CSOBanner 正在运行，结束 CSOBanner。");
					CloseHandle(hCSOBannerProcess);
				}
			}
			else if (msg.message == WM_HOTKEY && mode != msg.wParam /* 需要进行模式切换 */ && msg.wParam >= '0' &&
				msg.wParam <= '5')
			{
				if (mode == '1')
				{
					s_Instance->m_InGameStateWatcherSwitch.Reset();
					s_Instance->m_InGameStateWatcherFinished.Wait();
					s_Instance->m_GameProcessWatcherSwitch.Reset();
					s_Instance->m_GameProcessWatcherFinished.Wait();
				}
				else if (mode == '2')
				{
					s_Instance->m_InGameStateWatcherSwitch.Reset();
					s_Instance->m_InGameStateWatcherFinished.Wait();
					s_Instance->m_GameProcessWatcherSwitch.Reset();
					s_Instance->m_GameProcessWatcherFinished.Wait();
				}
				else if (mode == '3')
				{
					s_Instance->m_FixedCommandDispatcherSwitch.Reset();
				}
				else if (mode == '4')
				{
					s_Instance->m_FixedCommandDispatcherSwitch.Reset();
				}
				else if (mode == '5')
				{
					s_Instance->m_FixedCommandDispatcherSwitch.Reset();
				}
				/* 切换状态 */
				if (msg.wParam == '0')
				{
					Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "切换为 0 号模式。");
					s_Instance->m_Messenger.DispatchNOP();
				}
				else if (msg.wParam == '1')
				{
					s_Instance->m_CurrentState.reset();
					s_Instance->m_InGameStateWatcherSwitch.Set();
					s_Instance->m_GameProcessWatcherSwitch.Set();
					s_Instance->ToggleExtendedAutoPlayMode(false);
					Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "切换为 1 号模式。");
				}
				else if (msg.wParam == '2')
				{
					s_Instance->m_CurrentState.reset();
					s_Instance->m_InGameStateWatcherSwitch.Set();
					s_Instance->m_GameProcessWatcherSwitch.Set();
					s_Instance->ToggleExtendedAutoPlayMode(true);
					Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "切换为 2 号模式。");
				}
				else if (msg.wParam == '3')
				{ /* 合成配件 */
					s_Instance->m_PeriodicCommand = EXECUTOR_COMMAND::CMD_COMBINE_PARTS;
					s_Instance->m_FixedCommandDispatcherSwitch.Set();
					Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "切换为 3 号模式。");
				}
				else if (msg.wParam == '4')
				{ /* 购买物品 */
					s_Instance->m_PeriodicCommand = EXECUTOR_COMMAND::CMD_PURCHASE_ITEM;
					s_Instance->m_FixedCommandDispatcherSwitch.Set();
					Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "切换为 4 号模式。");
				}
				else if (msg.wParam == '5')
				{ /* 光标定位 */
					s_Instance->m_PeriodicCommand = EXECUTOR_COMMAND::CMD_LOCATE_CURSOR;
					s_Instance->m_FixedCommandDispatcherSwitch.Set();
					Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "切换为 5 号模式。");
				}
				mode = msg.wParam;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
    } while (false);
	while (idHotKeyEnd > idHotKeyStart)
	{
		UnregisterHotKey(nullptr, --idHotKeyEnd);
	}
	if (idTimer != 0)
	{
		KillTimer(nullptr, idTimer);
	}
    Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "线程 m_HotKeyEventHandler 退出。");
    s_Instance->m_ThreadExitEvent.Set();
}
