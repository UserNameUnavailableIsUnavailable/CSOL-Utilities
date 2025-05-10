#pragma once

#include <atomic>
#include <condition_variable>
#include <stop_token>
#include <string>
#include <thread>
#include <Windows.h>
#include "Module.hpp"

namespace CSOL_Utilities
{
    struct GameProcessInformation
    {
        std::wstring GameProcessName;
        std::wstring GameWindowTitle;
        std::wstring GameExecutablePath;
        std::wstring GameProcessLaunchCommand;
        std::atomic<HWND> hGameWindow = nullptr;
        std::atomic<DWORD> dwGameProcessId = 0;
		GameProcessInformation(std::wstring game_process_name, std::wstring game_window_title, std::wstring game_executable_path, std::wstring game_process_launch_command) :
			GameProcessName(std::move(game_process_name)), GameWindowTitle(std::move(game_window_title)), GameExecutablePath(std::move(game_executable_path)), GameProcessLaunchCommand(std::move(game_process_launch_command))
		{
		}
		GameProcessInformation(const GameProcessInformation& game_process_information) = delete;
        GameProcessInformation(GameProcessInformation&& game_process_information)
        {
            std::swap(GameProcessName, game_process_information.GameProcessName);
            std::swap(GameWindowTitle, game_process_information.GameWindowTitle);
			std::swap(GameExecutablePath, game_process_information.GameExecutablePath);
            std::swap(GameProcessLaunchCommand, game_process_information.GameProcessLaunchCommand);
            hGameWindow.store(game_process_information.hGameWindow);
            dwGameProcessId.store(game_process_information.dwGameProcessId);
        }
    };

	/* 游戏状态 */
	enum class IN_GAME_STATE
	{
		IGS_LOGIN, /* 正在登陆 */
		IGS_LOBBY, /* 在大厅中 */
		IGS_ROOM, /* 在房间内 */
		IGS_LOADING, /* 游戏场景正在加载 */
		IGS_GAMING, /* 在游戏地图中 */
		IGS_UNKNOWN /* 未知状态 */
	};

    /* 游戏进程状态 */
	enum class GAME_PROCESS_STATE
	{
		GPS_BEING_CREATED, /* 游戏进程正在被创建 */
		GPS_RUNNING, /* 游戏进程正在运行 */
		GPS_EXITED, /* 游戏进程退出 */
		GPS_UNKNOWN, /* 尚未确认游戏进程状态 */
	};

    class IdleEngine : public Module
    {
    public:
        IdleEngine(GameProcessInformation game_process_info);
        ~IdleEngine() noexcept;
        virtual void Resume();
        virtual void Suspend();
        void ToggleExtendedMode(bool flag) noexcept
        {
            m_ExtendedMode.store(flag, std::memory_order_release);
        }
    protected:
        std::mutex m_StateLock;
        std::condition_variable m_DetectorRunnable;
        std::condition_variable m_DetectorFinished;
        std::condition_variable m_RecognizerRunnable;
        std::condition_variable m_RecognizerFinished;

        std::stop_source m_StopSource;
        bool m_bDetectorRunnable = false;
        bool m_bRecognizerRunnable = false;
        bool m_bDetectorFinished = true;
        bool m_bRecognizerFinished = true;
        virtual void DetectGameProcess(std::stop_token st);
        virtual void RecognizeGameState(std::stop_token st) = 0;
        GameProcessInformation m_GameProcessInfo;
        /* 考虑到 dangling resources 问题，这里使用 thread 而非 jthread */
        std::thread m_GameStateRecognizer;
        std::thread m_GameProcessDetector;
		HMODULE m_hGamingTool = nullptr; /* 游戏工具 */
        std::atomic_bool m_ExtendedMode;
    };
}
