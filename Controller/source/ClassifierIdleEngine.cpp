#include "ClassifierIdleEngine.hpp"

#include "Command.hpp"
#include "Console.hpp"
#include "Exception.hpp"
#include "Global.hpp"
#include "Utilities.hpp"

using namespace CSOL_Utilities;

ClassifierIdleEngine::ClassifierIdleEngine(std::unique_ptr<GameProcessInformation> game_process_information,
                                           std::filesystem::path classifier_model_json_path)
    : IdleEngine(std::move(game_process_information))
{
    SetDiscriminationInterval(3500);
    classifier_ = std::make_unique<Classifier>(classifier_model_json_path);
    interface_type_ = GAME_INTERFACE_TYPE::UNKNOWN; /* 游戏内状态 */
}

void ClassifierIdleEngine::Discriminate()
{
    thread_local std::vector<uint8_t> buffer;
    thread_local auto capture_error_count = 0;
    thread_local uint64_t session_identifier = 0;
    std::chrono::system_clock::time_point recognize_start = std::chrono::system_clock::now();
    auto interface_type = interface_type_.load(std::memory_order_acquire);
    auto game_window_handle = game_process_info_->get_window_handle();
    if (!IsWindow(game_window_handle))
    {
        return;
    }
    if (IsIconic(game_window_handle))
    {
        // 尝试恢复窗口
        ShowWindow(game_window_handle, SW_NORMAL);
    }
    if (GetForegroundWindow() != game_window_handle)
    {
        // 尝试将窗口置于最前，并激活
        SetWindowPos(game_window_handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    CenterWindowClientArea(game_window_handle); // 将窗口客户区居中
    auto current_tp = std::chrono::system_clock::now();
    try
    {
        CaptureWindowAsBmp(game_window_handle, buffer);
    }
    catch (const std::exception &e)
    {
        Console::Warn(e.what());
        capture_error_count++;
        if (capture_error_count > 9)
        {
            throw Exception(Translate("ClassifierIdleEngine::ERROR_CaptureFailedTooManyTimes"));
        }
        return;
    }
    capture_error_count = 0;
    auto game_screenshot = cv::imdecode(buffer, cv::IMREAD_COLOR);
    thread_local auto decode_error_count{0};
    if (game_screenshot.empty())
    {
        decode_error_count++;
        if (decode_error_count > 9)
        {
            throw Exception("ClassifierIdleEngine::ERROR_DecodeFailedTooManyTimes");
        }
        Console::Warn(Translate("ClassifierIdleEngine::WARN_Decode"));
        return;
    }
#ifdef _DEBUG
    auto classify_start = std::chrono::system_clock::now();
#endif
    auto classification_result = classifier_->Run(game_screenshot);
#ifdef _DEBUG
    auto classify_end = std::chrono::system_clock::now();
    Console::Debug(
        std::format("本轮推理耗时 {} 毫秒",
                    std::chrono::duration_cast<std::chrono::milliseconds>(classify_end - classify_start).count()));
#endif
    auto current_interface_type = StringToInterfaceType(classification_result);
    if (current_interface_type == GAME_INTERFACE_TYPE::UNKNOWN)
    {
        if (interface_type == GAME_INTERFACE_TYPE::IN_GAME || interface_type == GAME_INTERFACE_TYPE::LOGIN ||
            current_interface_type == GAME_INTERFACE_TYPE::LOBBY)
        // 大厅/游戏内/登录状态下，若判定出未知状态则视为判定错误，仍然保持原状态
        {
            current_interface_type = interface_type;
        }
    }
    // 状态机
    // 分支数量庞大，但也没有更好的办法了……
    // 从 LOGIN、LOBBY、RESULTS 状态进入 ROOM
    // 状态时，视为新一轮游戏会话，更新会话标识符
    switch (interface_type)
    {
    case GAME_INTERFACE_TYPE::LOGIN: {
        switch (current_interface_type)
        {
        case GAME_INTERFACE_TYPE::LOGIN: // LOGIN -> LOGIN
        {
            auto elapsed = current_tp - interface_timepoint_;
            command_type_ = Command::TYPE::CMD_NOP;
            if (elapsed > std::chrono::seconds(120)) // 等待登陆达到 120 秒
            {
                Console::Warn(Translate("IdleEngine::WARN_LoginTimeout@1", 120));
                current_interface_type = GAME_INTERFACE_TYPE::UNKNOWN; // 超时后视为未知状态，等待下一次判定
            }
        }
        break;                           // esac LOGIN
        case GAME_INTERFACE_TYPE::LOBBY: // LOGIN -> LOBBY
            command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM;
            Console::Info(Translate("IdleEngine::INFO_LoginToLobby"));
            break;                      // esac LOBBY
        case GAME_INTERFACE_TYPE::ROOM: // LOGIN -> ROOM
            session_identifier++;
            command_type_ = Command::TYPE::CMD_START_GAME_ROOM;
            Console::Info(Translate("IdleEngine::INFO_Room"));
            break;                         // esac ROOM
        case GAME_INTERFACE_TYPE::LOADING: // LOGIN -> LOADING
            command_type_ = Command::TYPE::CMD_WAIT_FOR_LOADING;
            Console::Info(Translate("IdleEngine::INFO_Loading"));
            break;                         // esac LOADING
        case GAME_INTERFACE_TYPE::IN_GAME: // LOGIN -> IN_GAME
            command_type_ = Command::TYPE::CMD_DEFAULT_IDLE_2;
            Console::Info(Translate("IdleEngine::INFO_InGame"));
            break;                         // esac IN_GAME
        case GAME_INTERFACE_TYPE::RESULTS: // LOGIN -> RESULTS
            command_type_ = Command::TYPE::CMD_CONFIRM_RESULTS;
            Console::Info(Translate("IdleEngine::INFO_Results"));
            break;                         // esac RESULTS
        case GAME_INTERFACE_TYPE::UNKNOWN: // LOGIN -> UNKNOWN
            command_type_ = Command::TYPE::CMD_CLEAR_POPUPS;
            break; // esac UNKNOWN
        }
    }
    break; // esac LOGIN

    case GAME_INTERFACE_TYPE::LOBBY: {
        switch (current_interface_type)
        {
        case GAME_INTERFACE_TYPE::LOGIN: // LOBBY -> LOGIN
        {
            auto msg = std::format("Unexpected transition from {} to GAME_INTERFACE_{}. File "
                                   "{}, "
                                   "line {}.",
                                   InterfaceTypeToString(interface_type), InterfaceTypeToString(current_interface_type),
                                   __FILE__, __LINE__);
            throw Exception(Translate("ERROR_Unexpected@1", msg));
        }
        break;                           // esac LOGIN
        case GAME_INTERFACE_TYPE::LOBBY: // LOBBY -> LOBBY
            command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM;
            break;                      // esac LOBBY
        case GAME_INTERFACE_TYPE::ROOM: // LOBBY -> ROOM
            session_identifier++;
            command_type_ = Command::TYPE::CMD_START_GAME_ROOM;
            Console::Info(Translate("IdleEngine::INFO_LobbyToRoom"));
            break;                         // esac ROOM
        case GAME_INTERFACE_TYPE::LOADING: // LOBBY -> LOADING
            command_type_ = Command::TYPE::CMD_WAIT_FOR_LOADING;
            Console::Info(Translate("IdleEngine::INFO_Loading"));
            break;                         // esac LOADING
        case GAME_INTERFACE_TYPE::IN_GAME: // LOBBY -> IN_GAME
            command_type_ = Command::TYPE::CMD_DEFAULT_IDLE_2;
            Console::Info(Translate("IdleEngine::INFO_InGame"));
            break;                         // esac IN_GAME
        case GAME_INTERFACE_TYPE::RESULTS: // LOBBY -> RESULTS
            command_type_ = Command::TYPE::CMD_CONFIRM_RESULTS;
            Console::Info(Translate("IdleEngine::INFO_Results"));
            break;                         // esac RESULTS
        case GAME_INTERFACE_TYPE::UNKNOWN: // LOBBY -> UNKNOWN
            command_type_ = Command::TYPE::CMD_CLEAR_POPUPS;
            break; // esac UNKNOWN
        }
    }
    break; // esac LOBBY

    case GAME_INTERFACE_TYPE::ROOM: {
        switch (current_interface_type)
        {
        case GAME_INTERFACE_TYPE::LOGIN: // ROOM -> LOGIN
        {
            auto msg = std::format("Unexpected transition from {} to GAME_INTERFACE_{}. File "
                                   "{}, "
                                   "line {}.",
                                   InterfaceTypeToString(interface_type), InterfaceTypeToString(current_interface_type),
                                   __FILE__, __LINE__);
            throw Exception(Translate("ERROR_Unexpected@1", msg));
        }
        break;                           // esac LOGIN
        case GAME_INTERFACE_TYPE::LOBBY: // ROOM -> LOBBY
            command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM;
            Console::Warn(Translate("IdleEngine::WARN_RoomToLobby"));
            break;                      // esac LOBBY
        case GAME_INTERFACE_TYPE::ROOM: // ROOM -> ROOM
        {
            auto elapsed = current_tp - interface_timepoint_;
            command_type_ = Command::TYPE::CMD_START_GAME_ROOM;               // 尝试开始游戏
            if (elapsed > std::chrono::seconds(Global::StartGameRoomTimeout)) // 等待开始游戏超时
            {
                Console::Warn(Translate("IdleEngine::WARN_RoomTimeout@1", Global::StartGameRoomTimeout));
                command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM; // 超时，回到大厅重新创建房间
            }
        }
        break;                             // esac ROOM
        case GAME_INTERFACE_TYPE::LOADING: // ROOM -> LOADING
            command_type_ = Command::TYPE::CMD_WAIT_FOR_LOADING;
            Console::Info(Translate("IdleEngine::INFO_RoomToLoading"));
            break;                         // esac LOADING
        case GAME_INTERFACE_TYPE::IN_GAME: // ROOM -> IN_GAME
            command_type_ = Command::TYPE::CMD_CHOOSE_CHARACTER;
            Console::Info(Translate("IdleEngine::INFO_InGame"));
            // 由房间直接转入游戏中，一般是由于用户手动接管导致，直接视为新一轮游戏会话
            break;                         // esac IN_GAME
        case GAME_INTERFACE_TYPE::RESULTS: // ROOM -> RESULTS
            // 由房间直接转入结果界面，一般是由于用户手动接管导致，直接视为本轮游戏会话结束
            command_type_ = Command::TYPE::CMD_CONFIRM_RESULTS;
            Console::Info(Translate("IdleEngine::INFO_Results"));
            break;                         // esac RESULTS
        case GAME_INTERFACE_TYPE::UNKNOWN: // ROOM -> UNKNOWN
            command_type_ = Command::TYPE::CMD_CLEAR_POPUPS;
            break; // esac UNKNOWN
        }
    }
    break; // esac ROOM

    case GAME_INTERFACE_TYPE::LOADING: {
        switch (current_interface_type)
        {
        case GAME_INTERFACE_TYPE::LOGIN: // LOADING -> LOGIN
        {
            auto msg = std::format("Unexpected transition from {} to GAME_INTERFACE_{}. File "
                                   "{}, "
                                   "line {}.",
                                   InterfaceTypeToString(interface_type), InterfaceTypeToString(current_interface_type),
                                   __FILE__, __LINE__);
            throw Exception(Translate("ERROR_Unexpected@1", msg));
        }
        break;                           // esac LOGIN
        case GAME_INTERFACE_TYPE::LOBBY: // LOADING -> LOBBY
            command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM;
            Console::Info(Translate("IdleEngine::INFO_Lobby"));
            break;                      // esac LOBBY
        case GAME_INTERFACE_TYPE::ROOM: // LOADING -> ROOM
        {
            // 加载失败
            thread_local int count = 0;
            thread_local uint64_t last_session_identifier = 0; // 记录最近异常的会话标识符
            if (last_session_identifier != session_identifier) // 新会话
            {
                count = 0;                                    // 加载失败次数先置为 0
                last_session_identifier = session_identifier; // 记录当前会话标识符
            }
            command_type_ = Command::TYPE::CMD_START_GAME_ROOM;
            if (count + 1 == 3) // 连续三次加载失败
            {
                command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM; // 离开当前房间重新创建
                Console::Info(Translate("IdleEngine::INFO_LoadingToRoomTooManyTimes"));
            }
            else // 未达三次加载失败
            {
                count++; // 加载失败次数加 1
            }
            Console::Warn(Translate("IdleEngine::WARN_LoadingToRoom"));
        }
        break;                             // esac ROOM
        case GAME_INTERFACE_TYPE::LOADING: // LOADING -> LOADING
        {
            command_type_ = Command::TYPE::CMD_WAIT_FOR_LOADING;
            auto elapsed = current_tp - interface_timepoint_;
            if (elapsed > std::chrono::seconds(120)) // 游戏加载超过 120 秒
            {
                Console::Warn(Translate("IdleEngine::WARN_LoadingTimeout@1", 120));
                if (Global::RestartGameOnLoadingTimeout)
                {
                    auto hProcess = game_process_info_->get_process_handle();
                    if (hProcess)
                    {
                        Console::Warn(Translate("IdleEngine::INFO_TryKillGameProcess"));
                        if (!SafeTerminateProcess(hProcess, 3000))
                        {
                            TerminateProcess(hProcess, -1);
                        }
                    }
                }
            }
        }
        break;                             // esac LOADING
        case GAME_INTERFACE_TYPE::IN_GAME: // LOADING -> IN_GAME
            command_type_ = Command::TYPE::CMD_CHOOSE_CHARACTER;
            Console::Info(Translate("IdleEngine::INFO_LoadingToInGame"));
            break;                         // esac IN_GAME
        case GAME_INTERFACE_TYPE::RESULTS: // LOADING -> RESULTS
            command_type_ = Command::TYPE::CMD_CONFIRM_RESULTS;
            Console::Info(Translate("IdleEngine::INFO_Results"));
            break;                         // esac RESULTS
        case GAME_INTERFACE_TYPE::UNKNOWN: // LOADING -> UNKNOWN
            command_type_ = Command::TYPE::CMD_CLEAR_POPUPS;
            break; // esac UNKNOWN
        }
    }
    break; // esac LOADING

    case GAME_INTERFACE_TYPE::IN_GAME: {
        switch (current_interface_type)
        {
        case GAME_INTERFACE_TYPE::LOGIN: // IN_GAME -> LOGIN
        {
            auto msg = std::format("Unexpected transition from {} to GAME_INTERFACE_{}. File "
                                   "{}, "
                                   "line {}.",
                                   InterfaceTypeToString(interface_type), InterfaceTypeToString(current_interface_type),
                                   __FILE__, __LINE__);
            throw Exception(Translate("ERROR_Unexpected@1", msg));
        }
        break;                           // esac LOGIN
        case GAME_INTERFACE_TYPE::LOBBY: // IN_GAME -> LOBBY
            command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM;
            Console::Warn(Translate("IdleEngine::WARN_InGameToLobby"));
            break;                      // esac LOBBY
        case GAME_INTERFACE_TYPE::ROOM: // IN_GAME -> ROOM
        {
            thread_local int count = 0;
            thread_local uint64_t last_session_identifier = 0; // 记录最近一次异常的会话标识符
            if (last_session_identifier != session_identifier)
            { // 根据状态标识符判断是否为新的房间
                count = 0;
                last_session_identifier = session_identifier;
            }
            command_type_ = Command::TYPE::CMD_START_GAME_ROOM; // 重新开始游戏
            Console::Warn(Translate("IdleEngine::WARN_InGameToRoom"));
            if (count + 1 == 3)
            {
                command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM; // 离开当前房间重新创建
                Console::Info(Translate("IdleEngine::INFO_InGameToRoomTooManyTimes"));
            }
            else
            {
                count++;
            }
        }
        break;                             // esac ROOM
        case GAME_INTERFACE_TYPE::LOADING: // IN_GAME -> LOADING
            command_type_ = Command::TYPE::CMD_WAIT_FOR_LOADING;
            Console::Info(Translate("IdleEngine::INFO_Loading"));
            break;                         // esac LOADING
        case GAME_INTERFACE_TYPE::IN_GAME: // IN_GAME -> IN_GAME
        {
            auto elapsed = current_tp - interface_timepoint_;
            if (elapsed < std::chrono::seconds(7)) // 加载完毕后的 7 秒内选择角色
            {
                command_type_ = Command::TYPE::CMD_CHOOSE_CHARACTER;
            }
            else if (elapsed < std::chrono::seconds(120 + 7)) // 进入游戏后的 120 秒内用默认模式挂机
            {
                command_type_ = Command::TYPE::CMD_DEFAULT_IDLE_2;
            }
            else if (elapsed < std::chrono::seconds(Global::MaxInGameTime)) // 超过 120
                                                                            // 秒且为扩展模式，执行扩展挂机
            {
                command_type_ = GetIdleMode() == IDLE_MODE::EXTENDED ? Command::TYPE::CMD_EXTENDED_IDLE_2
                                                                     : Command::TYPE::CMD_DEFAULT_IDLE_2;
            }
            else // 超过最长游戏内时间
            {
                Console::Warn(Translate("IdleEngine::WARN_MaxInGameTimeReached@1", Global::MaxInGameTime));
                // 强制结束游戏进程
                auto hProcess = game_process_info_->get_process_handle();
                if (hProcess)
                {
                    Console::Warn(Translate("IdleEngine::INFO_TryKillGameProcess"));
                    if (!SafeTerminateProcess(hProcess, 3000))
                    {
                        TerminateProcess(hProcess, -1);
                    }
                }
            }
        }
        break;                             // esac IN_GAME
        case GAME_INTERFACE_TYPE::RESULTS: // IN_GAME -> RESULTS
            command_type_ = Command::TYPE::CMD_CONFIRM_RESULTS;
            Console::Info(Translate("IdleEngine::INFO_InGameToResults"));
            break;                         // esac RESULTS
        case GAME_INTERFACE_TYPE::UNKNOWN: // IN_GAME -> UNKNOWN
            command_type_ = Command::TYPE::CMD_CLEAR_POPUPS;
            break; // esac UNKNOWN
        }
        break; // esac IN_GAME
    case GAME_INTERFACE_TYPE::RESULTS:
        switch (current_interface_type)
        {
        case GAME_INTERFACE_TYPE::LOGIN: // RESULTS -> LOGIN
        {
            auto msg = std::format("Unexpected transition from {} to GAME_INTERFACE_{}. File "
                                   "{}, "
                                   "line {}.",
                                   InterfaceTypeToString(interface_type), InterfaceTypeToString(current_interface_type),
                                   __FILE__, __LINE__);
            throw Exception(Translate("ERROR_Unexpected@1", msg));
        }
        break;                           // esac LOGIN
        case GAME_INTERFACE_TYPE::LOBBY: // RESULTS -> LOBBY
            command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM;
            Console::Info(Translate("IdleEngine::INFO_Lobby"));
            break;                      // esac LOBBY
        case GAME_INTERFACE_TYPE::ROOM: // RESULTS -> ROOM
            session_identifier++;
            command_type_ = Command::TYPE::CMD_START_GAME_ROOM;
            Console::Info(Translate("IdleEngine::INFO_ResultsToRoom"));
            break;                         // esac ROOM
        case GAME_INTERFACE_TYPE::LOADING: // RESULTS -> LOADING
            command_type_ = Command::TYPE::CMD_WAIT_FOR_LOADING;
            Console::Info(Translate("IdleEngine::INFO_Loading"));
            break;                         // esac LOADING
        case GAME_INTERFACE_TYPE::IN_GAME: // RESULTS -> IN_GAME
            command_type_ = Command::TYPE::CMD_DEFAULT_IDLE_2;
            Console::Info(Translate("IdleEngine::INFO_InGame"));
            break;                         // esac IN_GAME
        case GAME_INTERFACE_TYPE::RESULTS: // RESULTS -> RESULTS
            command_type_ = Command::TYPE::CMD_CONFIRM_RESULTS;
            break;                         // esac RESULTS
        case GAME_INTERFACE_TYPE::UNKNOWN: // RESULTS -> UNKNOWN
            command_type_ = Command::TYPE::CMD_CLEAR_POPUPS;
            break; // esac UNKNOWN
        }
        break; // esac RESULTS

    case GAME_INTERFACE_TYPE::UNKNOWN: {
        switch (current_interface_type)
        {
        case GAME_INTERFACE_TYPE::LOGIN: // UNKNOWN -> LOGIN
        {
            auto msg = std::format("Unexpected transition from {} to GAME_INTERFACE_{}. File "
                                   "{}, "
                                   "line {}.",
                                   InterfaceTypeToString(interface_type), InterfaceTypeToString(current_interface_type),
                                   __FILE__, __LINE__);
            throw Exception(Translate("ERROR_Unexpected@1", msg));
        }
        break;                           // esac LOGIN
        case GAME_INTERFACE_TYPE::LOBBY: // UNKNOWN -> LOBBY
            Console::Info(Translate("IdleEngine::INFO_Lobby"));
            command_type_ = Command::TYPE::CMD_CREATE_GAME_ROOM;
            break;                      // esac LOBBY
        case GAME_INTERFACE_TYPE::ROOM: // UNKNOWN -> ROOM
            session_identifier++;
            Console::Info(Translate("IdleEngine::INFO_Room"));
            command_type_ = Command::TYPE::CMD_START_GAME_ROOM;
            break;                         // esac ROOM
        case GAME_INTERFACE_TYPE::LOADING: // UNKNOWN -> LOADING
            command_type_ = Command::TYPE::CMD_WAIT_FOR_LOADING;
            Console::Info(Translate("IdleEngine::INFO_Loading"));
            break;                         // esac LOADING
        case GAME_INTERFACE_TYPE::IN_GAME: // UNKNOWN -> IN_GAME
            Console::Info(Translate("IdleEngine::INFO_InGame"));
            command_type_ = Command::TYPE::CMD_CHOOSE_CHARACTER;
            break;                         // esac IN_GAME
        case GAME_INTERFACE_TYPE::RESULTS: // UNKNOWN -> RESULTS
            Console::Info(Translate("IdleEngine::INFO_Results"));
            command_type_ = Command::TYPE::CMD_CONFIRM_RESULTS;
            break;                         // esac RESULTS
        case GAME_INTERFACE_TYPE::UNKNOWN: // 已经处理过状态转移后仍为自身的情形
            command_type_ = Command::TYPE::CMD_CLEAR_POPUPS;
            break; // esac UNKNOWN
        }
    }
    break; // esac UNKNOWN
    }
    }
    if (current_interface_type != interface_type) // 状态变更，更新状态和时刻
    {
        interface_timepoint_ = std::chrono::system_clock::now();
        interface_type_.store(current_interface_type, std::memory_order_release);
    }
    switch (command_type_)
    {
    case Command::TYPE::CMD_NOP:
        Command::Set(command_type_, Command::CMD_ZERO_TIMESTAMP);
        break; // esac NOP
    case Command::TYPE::CMD_CONFIRM_RESULTS:
        Command::Set(command_type_, Command::CMD_DEFAULT);
        break; // esac CONFIRM_RESULTS
    case Command::TYPE::CMD_CREATE_GAME_ROOM:
        Command::Set(command_type_, Command::CMD_DEFAULT);
        break; // esac CREATE_GAME_ROOM
    case Command::TYPE::CMD_CLEAR_POPUPS:
        Command::Set(command_type_, Command::CMD_DEFAULT);
        break; // esac CLEAR_POPUPS
    default:
        Command::Set(command_type_, Command::CMD_REPEATABLE); // 其他命令均为可重复
    }
}
