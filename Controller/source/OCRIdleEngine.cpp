#include "pch.hpp"

#include "OCRIdleEngine.hpp"
#include "Command.hpp"
#include "IdleEngine.hpp"
#include "Console.hpp"
#include "Global.hpp"
#include "Exception.hpp"
#include "OCR.hpp"
#include "Utilities.hpp"


namespace CSOL_Utilities
{
    OCRIdleEngine::OCRIdleEngine(GameProcessInformation game_process_information, OCRBackboneInformation ocr_backbone_information) :
        IdleEngine(std::move(game_process_information))
    {
		auto dbnet_setting = std::make_unique<DBNet_Setting>();
		dbnet_setting->m_ModelPath = ocr_backbone_information.DBNetPath;
		dbnet_setting->m_BinarizationProbabilityThreshold = 0.8f;
		dbnet_setting->m_DilationOffsetRatio = 1.6f;

		auto crnn_setting = std::make_unique<CRNN_Setting>();
		crnn_setting->m_ModelPath = ocr_backbone_information.CRNNPath;
		crnn_setting->m_DictionaryPath = ocr_backbone_information.DictPath;

		auto ocr_setting = std::make_unique<OCR_Setting>();
		ocr_setting->m_Padding = 16;
		ocr_setting->m_MaxSideLength = 1920;
		ocr_setting->m_DetectionConfidenceThreshold = 0.8f;

		m_OCR = std::make_unique<OCR>(
			std::move(ocr_setting),
			std::move(dbnet_setting),
			std::move(crnn_setting)
		);

        std::ifstream ifs(ocr_backbone_information.KeywordsPath, std::ios::ate | std::ios::binary);
        if (!ifs.is_open())
        {
			std::string reason;
			if (ifs.fail()) {
				reason = Translate("IdleEngine::OCR::FAIL_BIT");
			} else if (ifs.bad()) {
				reason = Translate("IdleEngine::OCR::BAD_BIT");
			} else {
				reason = Translate("IdleEngine::OCR::UNKNOWN");
			}
			throw Exception(Translate("IdleEngine::OCR::ERROR_OpenKeywordsFile@1", reason));
        }
        std::string json_string;
        auto size = ifs.tellg();
        json_string.resize(size);
        ifs.seekg(0);
        ifs.read(json_string.data(), size);
		auto keywords_json = nlohmann::json::parse(json_string);
		/* 导入关键词 */
        for (auto i : { "LOBBY", "ROOM", "LOADING", "GAMING" })
        {
            for (auto j : keywords_json[i])
            {
                if (j.type() != nlohmann::json::value_t::string)
                {
                    throw Exception(Translate("IdleEngine::OCR::ERROR_InvalidKeywordType"));
                }
				m_Keywords[i].emplace_back(j.get<std::string>());
            }
        }
    }

    void OCRIdleEngine::RecognizeGameState(std::stop_token st)
    {
        while (true)
        {
            {
				std::unique_lock lk(m_StateLock);
				m_RecognizerRunnable.wait(lk, [this, &st] {
					return st.stop_requested() || (m_bDetectorRunnable && m_bRecognizerRunnable);
				});
				if (st.stop_requested())
				{
					break;
				}
			}
            m_bRecognizerFinished = false;
            Analyze();
            Dispatch();
			{
				std::lock_guard lk(m_StateLock);
				m_bRecognizerFinished = true;
			}
			m_RecognizerFinished.notify_one();
			SleepEx(5000, true);
        }
    }

    void OCRIdleEngine::Analyze()
    {
		auto recognize_start = std::chrono::system_clock::now();
		thread_local std::vector<std::string> results; /* 复用容器，节省空间 */
		Recognize(results);
		std::chrono::system_clock::time_point recognize_end = std::chrono::system_clock::now();
		#ifdef _DEBUG
		Console::Debug(std::format("本次识别耗时 {} 毫秒。", std::chrono::duration_cast<std::chrono::milliseconds>(recognize_end - recognize_start).count()));
		#endif
		if (results.empty()) /* 未识别到任何文本，剔除 */
		{
			return;
		}
		thread_local auto analyzed_in_game_state = IN_GAME_STATE::IGS_UNKNOWN;
		std::array<int, 4> scenarios{ 0, 0, 0, 0 };
		constexpr auto lobby = 0;
		constexpr auto room = 1;
		constexpr auto loading = 2;
		constexpr auto gaming = 3;
		for (const auto& s : results)
		{
			for (std::string_view keyword : m_Keywords["LOBBY"])
			{
				if (s.find(keyword) != std::string::npos)
				{
					scenarios[lobby]++;
				}
			}
			for (std::string_view keyword : m_Keywords["ROOM"])
			{
				if (s.find(keyword) != std::string::npos)
				{
					scenarios[room]++;
				}
			}
			for (std::string_view keyword : m_Keywords["LOADING"])
			{
				if (s.find(keyword) != std::string::npos)
				{
					scenarios[loading]++;
				}
			}
			for (std::string_view keyword : m_Keywords["GAMING"])
			{
				if (s.find(keyword) != std::string::npos)
				{
					scenarios[gaming]++;
				}
			}
		}
		auto greatest = std::max_element(scenarios.begin(), scenarios.end());
		if (*greatest < 1)
		{
			analyzed_in_game_state = IN_GAME_STATE::IGS_UNKNOWN;
		}
		else
		{
			switch (greatest - scenarios.begin())
			{
			case lobby: analyzed_in_game_state = IN_GAME_STATE::IGS_LOBBY; break;
			case room: analyzed_in_game_state = IN_GAME_STATE::IGS_ROOM; break;
			case loading: analyzed_in_game_state = IN_GAME_STATE::IGS_LOADING; break;
			case gaming: analyzed_in_game_state = IN_GAME_STATE::IGS_GAMING; break;
			}
			if (*greatest == scenarios[gaming])
			{
				analyzed_in_game_state = IN_GAME_STATE::IGS_GAMING;
			}
		}
		// thread_local int abnormal_back_to_room = 0;
		/* 合法状态迁移 */
		/* 手写状态机，考虑到后续维护难度，故引入了更多的代码冗余降低理解难度 */
		if (m_InGameState == analyzed_in_game_state)
		{
			/* 自上一次起状态未发生变化 */
		}
		/* 5 种顺序迁移 */
		else if (m_InGameState == IN_GAME_STATE::IGS_LOGIN && analyzed_in_game_state == IN_GAME_STATE::IGS_LOBBY) /* 登陆成功，进入大厅 */
		{
			Console::Info(Translate("IdleEngine::OCR::INFO_LoginSuccess"));
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_LOBBY && analyzed_in_game_state == IN_GAME_STATE::IGS_ROOM) /* 创建房间 */
		{
			Console::Info(Translate("IdleEngine::OCR::INFO_EnterGameRoom"));
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_ROOM && analyzed_in_game_state == IN_GAME_STATE::IGS_LOADING) /* 开始游戏 */
		{
			Console::Info(Translate("IdleEngine::OCR::INFO_StartGameRoom"));
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_LOADING && analyzed_in_game_state == IN_GAME_STATE::IGS_GAMING) /* 场景加载完成 */
		{
			Console::Info(Translate("IdleEngine::OCR::INFO_GameMapLoaded"));
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_GAMING && analyzed_in_game_state == IN_GAME_STATE::IGS_ROOM) /* 一局游戏完成 */
		{
			Console::Info(Translate("IdleEngine::OCR::INFO_RoundCompleted"));
		}
		/* 消除未知状态迁移，不计入迁移种类 */
		else if (analyzed_in_game_state == IN_GAME_STATE::IGS_UNKNOWN)
		{
			if (m_InGameState == IN_GAME_STATE::IGS_GAMING || m_InGameState == IN_GAME_STATE::IGS_LOADING)
			{
				/* 正在游戏或正在加载，保持原有状态不变 */
				analyzed_in_game_state = GetInGameState();
			}
			else
			{
				analyzed_in_game_state = IN_GAME_STATE::IGS_UNKNOWN;
			}
		}
		else if (analyzed_in_game_state != IN_GAME_STATE::IGS_UNKNOWN && m_InGameState != IN_GAME_STATE::IGS_UNKNOWN)
		{
			switch (analyzed_in_game_state)
			{
			case IN_GAME_STATE::IGS_LOBBY:
				Console::Info(Translate("IdleEngine::OCR::INFO_Lobby"));
				break;
			case IN_GAME_STATE::IGS_GAMING:
				Console::Info(Translate("IdleEngine::OCR::INFO_Gaming"));
				break;
			case IN_GAME_STATE::IGS_LOADING:
				Console::Info(Translate("IdleEngine::OCR::INFO_Loading"));
				break;
			case IN_GAME_STATE::IGS_ROOM:
				Console::Info(Translate("IdleEngine::OCR::INFO_Room"));
				break;
			default:
				break;
			}
		}
		/* 非法状态迁移，4 种 */
		else if (m_InGameState == IN_GAME_STATE::IGS_LOADING && analyzed_in_game_state == IN_GAME_STATE::IGS_ROOM)
		/* 加载失败，现象一般为重连 1 2 3，可能是网络问题 */
		{
			analyzed_in_game_state = IN_GAME_STATE::IGS_LOBBY; /* 直接离开房间回到大厅 */
			Console::Warn(Translate("IdleEngine::OCR::WARN_LoadGameMap"));
		}
		// else if (m_InGameState == IN_GAME_STATE::IGS_GAMING && analyzed_in_game_state == IN_GAME_STATE::IGS_ROOM)
		// {
		// 	abnormal_back_to_room++;
		// 	if (abnormal_back_to_room == 3)
		// 	{
		// 		analyzed_in_game_state = IN_GAME_STATE::IGS_LOBBY; /* 直接离开房间回到大厅 */
		// 		Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromGamingToRoom"));
		// 		abnormal_back_to_room = 0;
		// 	}
		// 	Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromGamingToRoomTooManyTimes"));
		// }
		else if (m_InGameState == IN_GAME_STATE::IGS_GAMING && analyzed_in_game_state == IN_GAME_STATE::IGS_LOBBY)
		/* 从游戏场景返回到大厅，原因一般为：强制踢出、长时间没有有效操作 */
		{
			analyzed_in_game_state = IN_GAME_STATE::IGS_LOBBY;
			Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromGamingToLobby"));
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_ROOM && analyzed_in_game_state == IN_GAME_STATE::IGS_LOBBY)
		/* 从游戏房间返回到大厅，原因可能是：强制踢出、房间等待时间过长 */
		{
			analyzed_in_game_state = IN_GAME_STATE::IGS_LOBBY;
			Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromRoomToLobby"));
		}
		else /* 其他情形，除非手动操作，否则不会出现 */
		{
			switch (analyzed_in_game_state)
			{
			case IN_GAME_STATE::IGS_LOBBY:
				Console::Info(Translate("IdleEngine::OCR::INFO_Lobby"));
				break;
			case IN_GAME_STATE::IGS_GAMING:
				Console::Info(Translate("IdleEngine::OCR::INFO_Gaming"));
				break;
			case IN_GAME_STATE::IGS_LOADING:
				Console::Info(Translate("IdleEngine::OCR::INFO_Loading"));
				break;
			case IN_GAME_STATE::IGS_ROOM:
				Console::Info(Translate("IdleEngine::OCR::INFO_Room"));
				break;
			default:
				break;
			}
		}

		/* 超时检查 */
		if (m_InGameState == IN_GAME_STATE::IGS_ROOM && recognize_start - m_tp > std::chrono::seconds(Global::StartGameRoomTimeout))
		{
			analyzed_in_game_state = IN_GAME_STATE::IGS_LOBBY;
			Console::Warn(Translate("IdleEngine::OCR::WARN_WaitStartGameRoomTimeout@1", Global::StartGameRoomTimeout));
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_LOGIN && recognize_start - m_tp > std::chrono::seconds(Global::LoginTimeout))
		/* 登录超时 */
		{
			analyzed_in_game_state = IN_GAME_STATE::IGS_UNKNOWN; /* 置为未知状态 */
			Console::Warn(Translate("IdleEngine::OCR::WARN_WaitLoginTimeout@1",
						 Global::LoginTimeout));
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_LOADING && recognize_start - m_tp > std::chrono::seconds(Global::LoadMapTimeout))
		/* 游戏加载超时 */
		{
			Console::Warn(Translate("IdleEngine::OCR::WaitLoadGameMapTimeout@1", Global::LoadMapTimeout));
			DWORD dwPId = 0;
			if (Global::RestartGameOnLoadingTimeout)
			{
				GetWindowThreadProcessId(m_GameProcessInfo.hGameWindow.load(std::memory_order_acquire), &dwPId);
				if (dwPId)
				{
					Console::Warn(Translate("IdleEngine::OCR::INFO_TryKillGameProcess"));
					HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, NULL, dwPId);
					if (hProcess)
					{
						if (!SafeTerminateProcess(hProcess, 3000))
						{
							TerminateProcess(hProcess, -1);
						}
						CloseHandle(hProcess);
					}
				}
				analyzed_in_game_state = IN_GAME_STATE::IGS_UNKNOWN;
			}
			else
			{
				analyzed_in_game_state = IN_GAME_STATE::IGS_GAMING;
			}
		}
		/* 更新状态 */
		SetInGameState(analyzed_in_game_state);
    }

    void OCRIdleEngine::Dispatch()
    {
		Command::TYPE command;

		std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
		if (m_InGameState == IN_GAME_STATE::IGS_UNKNOWN)
		{
			command = Command::TYPE::CMD_CLEAR_POPUPS;
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_LOGIN) /* 正在登陆 */
		{
			command = Command::TYPE::CMD_NOP;
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_LOBBY) /* 大厅内 */
		{
			command = Command::TYPE::CMD_CREATE_GAME_ROOM;
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_ROOM) /* 房间内（正常） */
		{
			command = Command::TYPE::CMD_START_GAME_ROOM;
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_LOADING) /* 加载 */
		{
			command = Command::TYPE::CMD_NOP;
		}
		else if (m_InGameState == IN_GAME_STATE::IGS_GAMING) /* 游戏中 */
		{
			if (tp - m_tp > std::chrono::seconds(60) && GetIdleMode() == IDLE_MODE::IM_EXTENDED) /* 扩展模式启用，且进入游戏达到 60 秒 */
			{
				command = Command::TYPE::CMD_EXTENDED_IDLE;
			#ifdef _DEBUG
				Console::Debug("扩展挂机模式。");
			#endif
			}
			else if (tp - m_tp > std::chrono::seconds(7)) /* 角色选择完成，进入挂机模式 */
			{
				command = Command::TYPE::CMD_DEFAULT_IDLE;
			#ifdef _DEBUG
				Console::Debug("默认挂机模式。");
			#endif
			}
			else
			{
				command = Command::TYPE::CMD_CHOOSE_CHARACTER;
			#ifdef _DEBUG
				Console::Info("选择游戏角色。");
			#endif
			}
		}
		switch (command)
		{
		case Command::TYPE::CMD_NOP:
			Command::Set(command, Command::CMD_ZERO_TIMESTAMP);
			break;
		case Command::TYPE::CMD_CREATE_GAME_ROOM:
			Command::Set(command, Command::CMD_DEFAULT);
			break;
		case Command::TYPE::CMD_CLEAR_POPUPS:
			Command::Set(command, Command::CMD_DEFAULT);
			break;
		default:
			Command::Set(command, Command::CMD_REPEATABLE);
		}
    }

	void OCRIdleEngine::Recognize(std::vector<std::string>& results)
	{
		thread_local std::vector<uint8_t> buffer;
		thread_local auto capture_error_count{ 0 };
		results.clear(); /* 清空结果 */
		auto hGameWindow = m_GameProcessInfo.hGameWindow.load(std::memory_order_acquire);
		if (!IsWindow(hGameWindow))
		{
			struct ParamPack
			{
				const DWORD dwProcessId;
				const std::wstring windowText;
				HWND hWnd;
			};
			auto dwGameProcessId = m_GameProcessInfo.dwGameProcessId.load(std::memory_order_acquire);
			if (dwGameProcessId != 0)
			{
				ParamPack params {
					.dwProcessId = dwGameProcessId,
					.windowText = m_GameProcessInfo.GameWindowTitle,
					.hWnd = nullptr
				};
				auto search_game_window = [] (HWND hWnd, LPARAM lParam) -> BOOL {
					auto params = reinterpret_cast<ParamPack*>(lParam);
					std::wstring window_title;

                    DWORD_PTR length = 0;
                    /* 向窗口发送 WM_GETTEXTLENGTH 消息，获取窗口标题长度，需要设置超时时间，防止有部分窗口不响应该消息 */
					auto ret = SendMessageTimeoutW(hWnd, WM_GETTEXTLENGTH, 0, 0, SMTO_ABORTIFHUNG, 200, &length);
					if (ret == 0) /* 失败或超时 */
					{
					#ifdef _DEBUG
						Console::Debug(std::format("枚举窗口（句柄：0x{:X}）失败或超时。", reinterpret_cast<std::uintptr_t>(hWnd)));
					#endif
						return true; /* 继续枚举下一个窗口 */
					}
					window_title.resize(length + 1); /* length 不包含 '\0' */
					auto iSize = GetWindowTextW(hWnd, window_title.data(), static_cast<int>(window_title.size()));
					window_title.resize(iSize); /* iSize 为不含末尾空字符的实际长度 */
					#ifdef _DEBUG
					Console::Debug(std::format("窗口句柄：0x{:X}，窗口标题：{}", reinterpret_cast<std::uintptr_t>(hWnd), ConvertUtf16ToUtf8(window_title)));
					#endif
					DWORD dwProcessId;
					GetWindowThreadProcessId(hWnd, &dwProcessId);
					if (dwProcessId != params->dwProcessId || window_title != params->windowText)
					{
						return true; /* 继续枚举 */
					}
					params->hWnd = hWnd;
					return false;
				};
				EnumWindows(search_game_window, reinterpret_cast<LPARAM>(&params));
				if (params.hWnd) /* 成功找到指定的游戏窗口 */
				{
					hGameWindow = params.hWnd;
					m_GameProcessInfo.hGameWindow.store(params.hWnd);
					if (IsIconic(hGameWindow))
					{
						ShowWindow(hGameWindow, SW_NORMAL);
					}
					SetWindowPos(hGameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					RemoveWindowBorder(hGameWindow);
					CenterWindow(hGameWindow);
					Console::Info(Translate("IdleEngine::OCR::INFO_GameWindowUpdated"));
				}
				else
				{
					return;
				}
			}
			else
			{
				return;
			}
		}
		try
		{
			CaptureWindowAsBmp(hGameWindow, buffer);
		#ifdef _DEBUG
			std::ofstream bmp_file("capture.bmp", std::ios::binary | std::ios::out | std::ios::trunc);
			bmp_file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
		#endif
		}
		catch (std::exception& e)
		{
			Console::Warn(e.what());
			capture_error_count++;
			if (capture_error_count > 9)
			{
				throw Exception(Translate("IdleEngine::OCR::ERROR_CaptureWindowAsBmpFailedTooManyTimes"));
			}
			return;
		}
		capture_error_count = 0;
		auto mat = cv::imdecode(buffer, cv::IMREAD_COLOR);

		thread_local auto decode_error_count{ 0 };
		if (mat.empty())
		{
			decode_error_count++;
			if (decode_error_count > 9)
			{
				throw Exception("IdleEngine::OCR::ERROR_imdecodeFailedTooManyTimes");
			}
			Console::Warn(Translate("IdleEngine::OCR::WARN_imdecode"));
			return;
		}
		auto ocr_results = m_OCR->Detect(mat);
		results.clear();
		for (auto& r : ocr_results)
		{
			results.emplace_back(std::move(r.m_ReognitionResult.m_Text));
		}
	}
}
