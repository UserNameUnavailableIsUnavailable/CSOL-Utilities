#include <Windows.h>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <exception>
#include <fstream>
#include <minwindef.h>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <opencv2/imgcodecs.hpp>
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
        IdleEngine(std::move(game_process_information)),
        m_OCR(ocr_backbone_information.DBNetPath, ocr_backbone_information.CRNNPath, ocr_backbone_information.DictPath, 0)
    {
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
		m_Keywords = nlohmann::json::parse(json_string);
		/* 导入关键词 */
        for (auto i : { "LOBBY", "ROOM", "LOADING", "IN_GAME" })
        {
            for (auto j : m_Keywords[i])
            {
                if (j.type() != nlohmann::json::value_t::string)
                {
                    throw Exception(Translate("IdleEngine::OCR::ERROR_InvalidKeywordType"));
                }
                trie.insert(j.get<std::string>());
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
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        }
    }

    void OCRIdleEngine::Analyze()
    {
		std::chrono::system_clock::time_point tp; /* 记录状态识别开始时间 */
		auto ocr_result = Recognize();
		if (ocr_result.empty()) // 未检测到任何结果
		{
			update_state(IN_GAME_STATE::IGS_UNKNOWN, std::chrono::system_clock::now());
		}
		thread_local auto igs = IN_GAME_STATE::IGS_UNKNOWN;
		auto parse_result = trie.parse_text(ocr_result);
		if (parse_result.empty())
		{
			igs = IN_GAME_STATE::IGS_UNKNOWN;
		}
		else /* 根据出现频次确定当前状态 */
		{
			struct InGameStatistics
			{
				int occurrences = 0;
				IN_GAME_STATE state = IN_GAME_STATE::IGS_UNKNOWN;
				InGameStatistics& operator++() noexcept
				{
					++occurrences;
					return *this;
				}
				InGameStatistics operator++(int) noexcept
				{
					auto tmp = *this;
					operator++();
					return tmp;
				}
			};
			InGameStatistics hall{0, IN_GAME_STATE::IGS_LOBBY};
			InGameStatistics room{0, IN_GAME_STATE::IGS_ROOM};
			InGameStatistics loading{0, IN_GAME_STATE::IGS_LOADING};
			InGameStatistics map{0, IN_GAME_STATE::IGS_GAMING};
			for (auto i = parse_result.begin(); i != parse_result.end(); i++)
			{
				if (i->get_index() < m_Keywords[KEYWORD_CATEGORY_LOBBY].size())
				{
					++hall;
				}
				else if (i->get_index() <
						 m_Keywords[KEYWORD_CATEGORY_LOBBY].size() + m_Keywords[KEYWORD_CATEGORY_ROOM].size())
				{
					++room;
				}
				else if (i->get_index() < m_Keywords[KEYWORD_CATEGORY_LOBBY].size() +
							 m_Keywords[KEYWORD_CATEGORY_ROOM].size() + m_Keywords[KEYWORD_CATEGORY_LOADING].size())
				{
					++loading;
				}
				else
				{
					++map;
				}
				std::array<InGameStatistics*, 4> list{&hall, &room, &loading, &map};
				auto result =
					std::max_element(list.begin(), list.end(), [](const InGameStatistics* a, const InGameStatistics* b)
									 { return a->occurrences < b->occurrences; });
				igs = (*result)->occurrences > 0 ? (*result)->state : IN_GAME_STATE::IGS_UNKNOWN; /* 出现次数要大于 0，否则仍然设定为未知状态 */
			}
		}
		thread_local int abnormal_back_to_room = 0;
		/* 合法状态迁移 */
		/* 手写状态机，考虑到后续维护难度，故引入了更多的代码冗余降低理解难度 */
		/* 5 种自迁移 */
		if (m_igs == igs)
		{
		}
		/* 5 种顺序迁移 */
		else if (m_igs == IN_GAME_STATE::IGS_LOGIN && igs == IN_GAME_STATE::IGS_LOBBY) /* 登陆成功，进入大厅 */
		{
			update_state(igs, tp);
			Console::Info(Translate("IdleEngine::OCR::INFO_LoginSuccess"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOBBY && igs == IN_GAME_STATE::IGS_ROOM) /* 创建房间 */
		{
			update_state(igs, tp);
			Console::Info(Translate("IdleEngine::OCR::INFO_EnterGameRoom"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_ROOM && igs == IN_GAME_STATE::IGS_LOADING) /* 开始游戏 */
		{
			update_state(igs, tp);
			Console::Info(Translate("IdleEngine::OCR::INFO_StartGameRoom"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOADING && igs == IN_GAME_STATE::IGS_GAMING) /* 场景加载完成 */
		{
			update_state(igs, tp);
			Console::Info(Translate("IdleEngine::OCR::INFO_GameMapLoaded"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_GAMING && igs == IN_GAME_STATE::IGS_ROOM) /* 结算确认 */
		{
			update_state(igs, tp);
			Console::Info(Translate("IdleEngine::OCR::INFO_ResultsConfirmed"));
		}
		/* 消除未知状态迁移，不计入迁移种类 */
		else if (igs == IN_GAME_STATE::IGS_UNKNOWN)
		{
			if (m_igs != IN_GAME_STATE::IGS_GAMING && m_igs != IN_GAME_STATE::IGS_LOADING)
			{
				update_state(IN_GAME_STATE::IGS_UNKNOWN, tp);
			}
			else
			{
				/* 正在游戏 */
			}
		}
		else if (m_igs == IN_GAME_STATE::IGS_UNKNOWN && igs != IN_GAME_STATE::IGS_UNKNOWN)
		{
			update_state(igs, tp);
			switch (igs)
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
		else if (m_igs == IN_GAME_STATE::IGS_LOADING &&
				 igs == IN_GAME_STATE::IGS_ROOM) /* 加载失败，现象一般为重连 1 2 3，可能是网络问题 */
		{
			update_state(IN_GAME_STATE::IGS_LOBBY, tp); /* 直接离开房间回到大厅 */
			Console::Warn(Translate("IdleEngine::OCR::WARN_LoadGameMap"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_GAMING && igs == IN_GAME_STATE::IGS_ROOM)
		{
			abnormal_back_to_room++;
			if (abnormal_back_to_room == 3)
			{
				update_state(IN_GAME_STATE::IGS_LOBBY, tp); /* 直接离开房间回到大厅 */
				Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromGamingToRoom"));
				abnormal_back_to_room = 0;
			}
			Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromGamingToRoomTooManyTimes"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_GAMING &&
				 igs == IN_GAME_STATE::IGS_LOBBY) /* 从游戏场景返回到大厅，原因一般为：强制踢出、长时间没有有效操作 */
		{
			update_state(IN_GAME_STATE::IGS_LOBBY, tp);
			Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromGamingToLobby"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_ROOM &&
				 igs == IN_GAME_STATE::IGS_LOBBY) /* 从游戏房间返回到大厅，原因可能是：强制踢出、房间等待时间过长 */
		{
			update_state(IN_GAME_STATE::IGS_LOBBY, tp);
			Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromRoomToLobby"));
		}
		else /* 其他情形，除非手动操作，否则应该不会出现 */
		{
			update_state(igs, tp);
			switch (igs)
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
		if (m_igs == IN_GAME_STATE::IGS_ROOM && tp - m_tp > std::chrono::seconds(Global::g_StartGameRoomTimeout))
		{
			update_state(IN_GAME_STATE::IGS_LOBBY, tp);
			Console::Warn(Translate("IdleEngine::OCR::WARN_WaitStartGameRoomTimeout@1", Global::g_StartGameRoomTimeout));
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOGIN && tp - m_tp > std::chrono::seconds(60)) /* 登录时间超过 60 秒 */
		{
			update_state(IN_GAME_STATE::IGS_UNKNOWN, tp);
			Console::Warn(Translate("IdleEngine::OCR::WaitLoginTimeout@1",
						 Global::g_LoginTimeout));
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOADING && tp - m_tp > std::chrono::seconds(150)) /* 游戏加载超时 */
		{
			Console::Warn(Translate("OCRIdleEngine::WaitLoadGameMapTimeout@1",
						 Global::g_LoadMapTimeout));
			DWORD dwPId = 0;
			GetWindowThreadProcessId(m_GameProcessInfo.hGameWindow.load(std::memory_order_acquire), &dwPId);
			if (dwPId)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, NULL, dwPId);
				if (hProcess)
				{
					TerminateProcess(hProcess, -1);
					CloseHandle(hProcess);
				}
			}
		}
    }

    void OCRIdleEngine::Dispatch()
    {
		Command::TYPE command;

		std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
		if (m_igs == IN_GAME_STATE::IGS_UNKNOWN)
		{
			command = Command::TYPE::CMD_CLEAR_POPUPS;
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOGIN) /* 正在登陆 */
		{
			command = Command::TYPE::CMD_NOP;
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOBBY) /* 大厅内 */
		{
			command = Command::TYPE::CMD_CREATE_GAME_ROOM;
		}
		else if (m_igs == IN_GAME_STATE::IGS_ROOM) /* 房间内（正常） */
		{
			command = Command::TYPE::CMD_START_GAME_ROOM;
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOADING) /* 加载 */
		{
			command = Command::TYPE::CMD_NOP;
		}
		else if (m_igs == IN_GAME_STATE::IGS_GAMING) /* 游戏中 */
		{
			if (m_extended.load(std::memory_order_acquire) && tp - m_tp > std::chrono::milliseconds(60))
			{
				command = Command::TYPE::CMD_EXTENDED_IDLE;
			}
			else if (tp - m_tp > std::chrono::seconds(5))
			{
				command = Command::TYPE::CMD_DEFAULT_IDLE;
			}
			else
			{
				command = Command::TYPE::CMD_CHOOSE_CHARACTER;
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
			Command::Set(command, Command::CMD_REPEATABLE | Command::CMD_AUTO_REFRESH);
		}
    }

	std::string OCRIdleEngine::Recognize()
	{
		thread_local std::vector<uint8_t> buffer;
		thread_local auto capture_error_count{ 0 };
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
					GetWindowTextW(hWnd, window_title.data(), static_cast<int>(window_title.size()));
					window_title.resize(length); /* 忽略末尾多余的 '\0' */
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
					Console::Info(Translate("IdleEngine::OCR::INFO_GameWindowUpdated"));
				}
				else
				{
					return "";
				}
			}
			else
			{
				return "";
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
			return "";
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
			return "";
		}
		OCRParam param{
			.nPadding = 50, .nMaxSideLength = 1024, .fBoxScoreThreshold = 0.6f, .fBoxThreshold = 0.3f, .fUnclipRatio = 2.0f,
		};
		auto result = m_OCR.Detect(mat, param);
		return result.content;
	}
}
