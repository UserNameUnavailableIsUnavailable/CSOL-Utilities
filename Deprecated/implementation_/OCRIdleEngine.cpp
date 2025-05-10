#include "OCRIdleEngine.hpp"
#include <Windows.h>
#include <cassert>
#include <errhandlingapi.h>
#include <memory>
#include "CSOL_Utilities.hpp"
#include "Command.hpp"
#include "Exception.hpp"
#include "MultiLingual.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"

namespace CSOL_Utilities
{
	void IdleEngine::work()
	{
		analyze();
		dispatch();
	}

	IdleEngine::IdleEngine(std::string json_keywords, std::filesystem::path ocr_detection_model_path,
								 std::filesystem::path ocr_recognition_model_path,
								 std::filesystem::path ocr_key_file_path, int n_threads, std::shared_ptr<ExecutorCommand> cmd) :
		Module(Opt::g_IdleEngineName, 2000),
		m_ocr(ocr_detection_model_path, ocr_recognition_model_path, ocr_key_file_path, n_threads), m_cmd(cmd)
	{
		if (!m_cmd) /* 命令对象不能为空 */
		{
			throw Exception(Translate("OCRIdleEngine::EXECUTOR_COMMAND_CANNOT_BE_NULL"));
		}
		m_keywords = nlohmann::json::parse(json_keywords);
		/* 导入关键词 */
		for (auto i : m_keywords[KEYWORD_CATEGORY_LOBBY])
		{
			if (i.type() == nlohmann::json::value_t::string)
			{
				throw Exception(Translate("ODRIdleEngine::EXECUTOR_COMMAND_CANNOT_BE_NULL"));
			}
			trie.insert(i.get<std::string>());
		}
		for (auto i : m_keywords[KEYWORD_CATEGORY_ROOM])
		{
			if (i.type() == nlohmann::json::value_t::string)
			{
				throw Exception(Translate("ODRIdleEngine::EXECUTOR_COMMAND_CANNOT_BE_NULL"));
			}
			trie.insert(i.get<std::string>());
		}
		for (auto i : m_keywords[KEYWORD_CATEGORY_LOADING])
		{
			if (i.type() == nlohmann::json::value_t::string)
			{
				throw Exception(Translate("ODRIdleEngine::EXECUTOR_COMMAND_CANNOT_BE_NULL"));
			}
			trie.insert(i.get<std::string>());
		}
		for (auto i : m_keywords[KEYWORD_CATEGORY_IN_GAME])
		{
			if (i.type() == nlohmann::json::value_t::string)
			{
				throw Exception(Translate("ODRIdleEngine::EXECUTOR_COMMAND_CANNOT_BE_NULL"));
			}
			trie.insert(i.get<std::string>());
		}
		m_hGamingTool = LoadLibraryW(L"GamingTool.dll");
		if (!m_hGamingTool)
		{
			throw Exception(Translate("OCRIdleEngine::FAILED_TO_LOAD_GAMING_TOOL@1", GetLastError()));
		}
	}

	IdleEngine::~IdleEngine()
	{
		FreeLibrary(m_hGamingTool);
	}

	std::string IdleEngine::recognize()
	{
		if (!m_hGameWindow || !IsWindow(m_hGameWindow))
		{
			m_hGameWindow = FindWindowW(nullptr, L"Counter-Strike Online");
			throw Exception(Translate("OCRIdleEngine::FAILED_TO_FIND_GAME_WINDOW"));
		}
		thread_local auto error_count = 0;
		try
		{
			CaptureWindowAsBmp(m_hGameWindow, m_image_buffer);
		}
		catch (const Exception& e)
		{
			Console::Log(EXCEPTION_LEVEL::EL_ERROR, e.what());
			error_count++;
			if (error_count > 9)
			{
				throw Exception(Translate("OCRIdleEngine::TOO_MANY_ERRORS_WHEN_TRYING_TO_CAPTURE_GAME_WINDOW@1", GetLastError()));
			}
		}
		m_image = cv::imdecode(m_image_buffer, cv::IMREAD_COLOR, &m_image);
		if (m_image.empty())
		{
			Console::Log(EXCEPTION_LEVEL::EL_ERROR, Translate("OCRIdleEngine::FAILED_TO_DECODE_IMAGE"));
			error_count++;
			if (error_count > 9)
			{
				throw Exception(Translate("OCRIdleEngine::TOO_MANY_ERRORS_WHEN_TRYING_TO_DECODE_IMAGE"));
			}
		}
		error_count = 0; /* 本次识别未出现错误，则将错误计数置为 0 */
		auto ocr_result = m_ocr.detect(m_image, m_ocr_param);
		return ocr_result.content;
	}

	void IdleEngine::analyze()
	{
		std::chrono::system_clock::time_point tp; /* 记录状态识别开始时间 */
		auto ocr_result = recognize();
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
				if (i->get_index() < m_keywords[KEYWORD_CATEGORY_LOBBY].size())
				{
					++hall;
				}
				else if (i->get_index() <
						 m_keywords[KEYWORD_CATEGORY_LOBBY].size() + m_keywords[KEYWORD_CATEGORY_ROOM].size())
				{
					++room;
				}
				else if (i->get_index() < m_keywords[KEYWORD_CATEGORY_LOBBY].size() +
							 m_keywords[KEYWORD_CATEGORY_ROOM].size() + m_keywords[KEYWORD_CATEGORY_LOADING].size())
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
				igs = (*result)->state;
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
			Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::LOGIN_SUCCESS"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOBBY && igs == IN_GAME_STATE::IGS_ROOM) /* 创建房间 */
		{
			update_state(igs, tp);
			Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::ENTER_GAME_ROOM"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_ROOM && igs == IN_GAME_STATE::IGS_LOADING) /* 开始游戏 */
		{
			update_state(igs, tp);
			Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::START_GAME_ROOM"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOADING && igs == IN_GAME_STATE::IGS_GAMING) /* 场景加载完成 */
		{
			update_state(igs, tp);
			Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::GAME_MAP_LOADED"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_GAMING && igs == IN_GAME_STATE::IGS_ROOM) /* 结算确认 */
		{
			update_state(igs, tp);
			Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::RESULTS_CONFIRMED"));
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
				Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::LOBBY"));
				break;
			case IN_GAME_STATE::IGS_GAMING:
				Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::MAP"));
				break;
			case IN_GAME_STATE::IGS_LOADING:
				Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::LOADING"));
				break;
			case IN_GAME_STATE::IGS_ROOM:
				Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::ROOM"));
				break;
			default:
				break;
			}
		}
		/* 非法状态迁移，4 种 */
		else if (m_igs == IN_GAME_STATE::IGS_LOADING &&
				 igs == IN_GAME_STATE::IGS_ROOM) /* 加载失败，现象一般为重连 1 2 3，可能是网络问题 */
		{
			abnormal_back_to_room++;
			if (abnormal_back_to_room == 3)
			{
				update_state(IN_GAME_STATE::IGS_LOBBY, tp); /* 直接离开房间回到大厅 */
				Console::Log(EXCEPTION_LEVEL::EL_WARNING, Translate("OCRIdleEngine::TOO_MANY_TIMES_RETURN_TO_ROOM"));
				abnormal_back_to_room = 0;
			}
			Console::Log(EXCEPTION_LEVEL::EL_WARNING, Translate("OCRIdleEngine::FAILED_TO_LOAD_GAME_MAP"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_GAMING &&
				 igs == IN_GAME_STATE::IGS_LOBBY) /* 从游戏场景返回到大厅，原因一般为：强制踢出、长时间没有有效操作 */
		{
			update_state(IN_GAME_STATE::IGS_LOBBY, tp);
			Console::Log(EXCEPTION_LEVEL::EL_WARNING, Translate("OCRIdleEngine::RETURN_FROM_GAMING_TO_LOBBY"));
		}
		else if (m_igs == IN_GAME_STATE::IGS_ROOM &&
				 igs == IN_GAME_STATE::IGS_LOBBY) /* 从游戏房间返回到大厅，原因可能是：强制踢出、房间等待时间过长 */
		{
			update_state(IN_GAME_STATE::IGS_LOBBY, tp);
			Console::Log(EXCEPTION_LEVEL::EL_WARNING, Translate("OCRIdleEngine::RETURN_FROM_ROOM_TO_LOBBY"));
		}
		else /* 其他情形，除非手动操作，否则应该不会出现 */
		{
			update_state(igs, tp);
			switch (igs)
			{
			case IN_GAME_STATE::IGS_LOBBY:
				Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::LOBBY"));
				break;
			case IN_GAME_STATE::IGS_GAMING:
				Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::GAMING"));
				break;
			case IN_GAME_STATE::IGS_LOADING:
				Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::LOADING"));
				break;
			case IN_GAME_STATE::IGS_ROOM:
				Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, Translate("OCRIdleEngine::ROOM"));
				break;
			default:
				break;
			}
		}

		/* 超时检查 */
		if (m_igs == IN_GAME_STATE::IGS_ROOM && tp - m_tp > std::chrono::seconds(Opt::g_MaxWaitTimeStartGameRoom))
		{
			update_state(IN_GAME_STATE::IGS_LOBBY, tp);
			Console::Log(EXCEPTION_LEVEL::EL_WARNING, Translate("OCRIdleEngine::WAIT_START_GAME_ROOM_TIMEOUT@1"),
						 Opt::g_MaxWaitTimeStartGameRoom);
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOGIN && tp - m_tp > std::chrono::seconds(60)) /* 登录时间超过 60 秒 */
		{
			update_state(IN_GAME_STATE::IGS_UNKNOWN, tp);
			Console::Log(EXCEPTION_LEVEL::EL_WARNING, Translate("OCRIdleEngine::WAIT_LOGIN_TIMEOUT@1"),
						 Opt::g_MaxWaitTimeLogin);
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOADING && tp - m_tp > std::chrono::seconds(150)) /* 游戏加载超时 */
		{
			Console::Log(EXCEPTION_LEVEL::EL_WARNING, Translate("OCRIdleEngine::WAIT_LOADING_TIMEOUT@1"),
						 Opt::g_MaxWaitTimeLoading);
			DWORD dwPId = 0;
			GetWindowThreadProcessId(m_hGameWindow, &dwPId);
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

	void IdleEngine::remove_game_window_border() noexcept
	{
		auto MakeWindowBorderless =
			reinterpret_cast<void (*)(HWND)>(GetProcAddress(m_hGamingTool, "MakeWindowBorderless"));
		if (MakeWindowBorderless)
		{
			MakeWindowBorderless(m_hGameWindow);
		}
	}

	void IdleEngine::dispatch()
	{
		thread_local EXECUTOR_COMMAND_TYPE last_dispatched_command = EXECUTOR_COMMAND_TYPE::CMD_NOP;
		EXECUTOR_COMMAND_TYPE command;

		std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
		if (m_igs == IN_GAME_STATE::IGS_UNKNOWN)
		{
			command = EXECUTOR_COMMAND_TYPE::CMD_CLEAR_POPUPS;
			m_cmd->Set(EXECUTOR_COMMAND_TYPE::CMD_CLEAR_POPUPS, tp, false);
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOGIN) /* 正在登陆 */
		{
			command = EXECUTOR_COMMAND_TYPE::CMD_NOP;
			m_cmd->NOP();
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOBBY) /* 大厅内 */
		{
			command = EXECUTOR_COMMAND_TYPE::CMD_CREATE_GAME_ROOM;
			thread_local std::chrono::time_point<std::chrono::system_clock> last_tp;
		}
		else if (m_igs == IN_GAME_STATE::IGS_ROOM) /* 房间内（正常） */
		{
			command = EXECUTOR_COMMAND_TYPE::CMD_START_GAME_ROOM;
		}
		else if (m_igs == IN_GAME_STATE::IGS_LOADING) /* 加载 */
		{
			command = EXECUTOR_COMMAND_TYPE::CMD_NOP;
		}
		else if (m_igs == IN_GAME_STATE::IGS_GAMING) /* 游戏中 */
		{
			if (m_extended.load(std::memory_order_acquire) && tp - m_tp > std::chrono::milliseconds(60))
			{
				command = EXECUTOR_COMMAND_TYPE::CMD_EXTENDED_IDLE;
			}
			else if (tp - m_tp > std::chrono::seconds(5))
			{
				command = EXECUTOR_COMMAND_TYPE::CMD_DEFAULT_IDLE;
			}
			else
			{
				command = EXECUTOR_COMMAND_TYPE::CMD_CHOOSE_CHARACTER;
			}
		}
		remove_game_window_border();
		if (command == last_dispatched_command)
		{
			return;
		}
		last_dispatched_command = command;
		switch (command)
		{
		case EXECUTOR_COMMAND_TYPE::CMD_NOP:
			m_cmd->NOP();
			break;
		case EXECUTOR_COMMAND_TYPE::CMD_CREATE_GAME_ROOM:
			m_cmd->Set(command, tp, false);
			break;
		case EXECUTOR_COMMAND_TYPE::CMD_CLEAR_POPUPS:
			m_cmd->Set(command, tp, false);
			break;
		default:
			m_cmd->Set(command, tp);
		}
	}
} // namespace CSOL_Utilities
