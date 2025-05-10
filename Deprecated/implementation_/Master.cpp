#include "Master.hpp"
#include <Windows.h>
#include <atomic>
#include <cstddef>
#include <errhandlingapi.h>
#include <format>
#include <stdexcept>
#include <winnt.h>
#include "CSOL_Utilities.hpp"
#include "Command.hpp"
#include "Module.hpp"
#include "Signal.hpp"

namespace CSOL_Utilities
{
	Master::Master(std::string name) : Module(name, 1000)
	{
	}
	void Master::work()
	{
		MSG msg;
		auto bRet = PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);
		thread_local DWORD dwCurrentMode = 0;
		if (Shared::g_ModuleExitEvent.signaled() || msg.message == WM_QUIT)
		{
			// TODO: 结束所有子模块
			m_alive = false;
			return;
		}
		else if (msg.message == WM_HOTKEY && dwCurrentMode != msg.wParam)
		{
			DWORD dwTargetMode = -1;
			for (int i = 0; i < m_KeyBindings.size(); i++)
			{
				if (m_KeyBindings[i].uKey == msg.wParam)
				{
					dwTargetMode = i;
					break;
				}
			}
			if (dwTargetMode > 0) /* 目标模式有效 */
			{
				switch (dwCurrentMode)
				{
				case 0: break;
				case 1:
					disable(Opt::g_GameProcessDetectorName);
					disable(Opt::g_IdleEngineName);
					break;
				case 2:
					disable(Opt::g_GameProcessDetectorName);
					disable(Opt::g_IdleEngineName);
					break;
				case 3:
				case 4:
				case 5:
					disable(Opt::g_CommandDispatcherName);
					break;
				default:
					break;
				}
				switch (dwTargetMode)
				{
				case 0: break;
				case 1:
					enable(Opt::g_GameProcessDetectorName);
					enable(Opt::g_IdleEngineName);
					break;
				case 2:
					enable(Opt::g_GameProcessDetectorName);
					enable(Opt::g_IdleEngineName);
					break;
				case 3:
					m_cmd->set(EXECUTOR_COMMAND::CMD_BATCH_COMBINE_PARTS, std::chrono::system_clock::now(), true);
					enable(Opt::g_CommandDispatcherName);
					break;
				case 4:
					m_cmd->set(EXECUTOR_COMMAND::CMD_BATCH_PURCHASE_ITEM, std::chrono::system_clock::now(), true);
					enable(Opt::g_CommandDispatcherName);
					break;
				case 5:
					m_cmd->set(EXECUTOR_COMMAND::CMD_LOCATE_CURSOR, std::chrono::system_clock::now(), true);
					enable(Opt::g_CommandDispatcherName);
					break;
				default:
					break;
				}
			}
		}
		if (bRet)
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
} // namespace CSOL_Utilities
