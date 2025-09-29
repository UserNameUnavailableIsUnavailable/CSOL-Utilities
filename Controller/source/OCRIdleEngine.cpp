#include "OCRIdleEngine.hpp"

#include "Command.hpp"
#include "Console.hpp"
#include "Exception.hpp"
#include "Global.hpp"
#include "IdleEngine.hpp"
#include "OCR.hpp"
#include "Utilities.hpp"

using namespace CSOL_Utilities;
OCRIdleEngine::OCRIdleEngine(std::unique_ptr<GameProcessInformation> game_process_information,
                             std::unique_ptr<OCRBackboneInformation> ocr_backbone_information)
    : IdleEngine(std::move(game_process_information))
{
    ocr_ = std::make_unique<OCR>(OCR_Detector(ocr_backbone_information->deteor_model_json_path),
                                 OCR_Recognizer(ocr_backbone_information->recognizer_model_json_path));
    if (!std::filesystem::is_regular_file(ocr_backbone_information->keywords_json_path))
    {
        throw Exception(
            Translate("ERROR_FileNotFound@1", ConvertUtf16ToUtf8(ocr_backbone_information->keywords_json_path)));
    }
    std::ifstream ifs(ocr_backbone_information->keywords_json_path, std::ios::ate | std::ios::binary);
    if (!ifs)
    {
        auto iostate = static_cast<std::size_t>(ifs.rdstate());
        throw Exception(
            Translate("ERROR_FileStream@2", ConvertUtf16ToUtf8(ocr_backbone_information->keywords_json_path),
                      std::format("std::ifstream failed with IO state {:#x} ({}:{})", iostate, __FILE__, __LINE__)));
    }
    std::string json_string;
    auto size = ifs.tellg();
    json_string.resize(size);
    ifs.seekg(0);
    ifs.read(json_string.data(), size);
    auto keywords_json = nlohmann::json::parse(json_string);
    /* 导入关键词 */
    for (auto i : {"LOBBY", "ROOM", "LOADING", "GAMING"})
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

void OCRIdleEngine::Discriminate()
{
    Analyze();
    Dispatch();
}

void OCRIdleEngine::Analyze()
{
    auto recognize_start = std::chrono::system_clock::now();
    thread_local std::vector<std::string> results; /* 复用容器，节省空间 */
    Recognize(results);
    std::chrono::system_clock::time_point recognize_end = std::chrono::system_clock::now();
#ifdef _DEBUG
    Console::Debug(
        std::format("本次识别耗时 {} 毫秒。",
                    std::chrono::duration_cast<std::chrono::milliseconds>(recognize_end - recognize_start).count()));
#endif
    if (results.empty()) /* 未识别到任何文本，剔除 */
    {
        return;
    }
    thread_local auto analyzed_in_game_state = GAME_INTERFACE_TYPE::UNKNOWN;
    std::array<int, 4> scenarios{0, 0, 0, 0};
    constexpr auto lobby = 0;
    constexpr auto room = 1;
    constexpr auto loading = 2;
    constexpr auto gaming = 3;
    for (const auto &s : results)
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
        analyzed_in_game_state = GAME_INTERFACE_TYPE::UNKNOWN;
    }
    else
    {
        switch (greatest - scenarios.begin())
        {
        case lobby:
            analyzed_in_game_state = GAME_INTERFACE_TYPE::LOBBY;
            break;
        case room:
            analyzed_in_game_state = GAME_INTERFACE_TYPE::ROOM;
            break;
        case loading:
            analyzed_in_game_state = GAME_INTERFACE_TYPE::LOADING;
            break;
        case gaming:
            analyzed_in_game_state = GAME_INTERFACE_TYPE::IN_GAME;
            break;
        }
        if (*greatest == scenarios[gaming])
        {
            analyzed_in_game_state = GAME_INTERFACE_TYPE::IN_GAME;
        }
    }
    // thread_local int abnormal_back_to_room = 0;
    /* 合法状态迁移 */
    /* 手写状态机，考虑到后续维护难度，故引入了更多的代码冗余降低理解难度 */
    if (game_interface_type_ == analyzed_in_game_state)
    {
        /* 自上一次起状态未发生变化 */
    }
    /* 5 种顺序迁移 */
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOGIN &&
             analyzed_in_game_state == GAME_INTERFACE_TYPE::LOBBY) /* 登陆成功，进入大厅 */
    {
        Console::Info(Translate("IdleEngine::OCR::INFO_LoginSuccess"));
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOBBY &&
             analyzed_in_game_state == GAME_INTERFACE_TYPE::ROOM) /* 创建房间 */
    {
        Console::Info(Translate("IdleEngine::OCR::INFO_EnterGameRoom"));
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::ROOM &&
             analyzed_in_game_state == GAME_INTERFACE_TYPE::LOADING) /* 开始游戏 */
    {
        Console::Info(Translate("IdleEngine::OCR::INFO_StartGameRoom"));
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOADING &&
             analyzed_in_game_state == GAME_INTERFACE_TYPE::IN_GAME) /* 场景加载完成 */
    {
        Console::Info(Translate("IdleEngine::OCR::INFO_GameMapLoaded"));
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::IN_GAME &&
             analyzed_in_game_state == GAME_INTERFACE_TYPE::ROOM) /* 一局游戏完成 */
    {
        Console::Info(Translate("IdleEngine::OCR::INFO_RoundCompleted"));
    }
    /* 消除未知状态迁移，不计入迁移种类 */
    else if (analyzed_in_game_state == GAME_INTERFACE_TYPE::UNKNOWN)
    {
        if (game_interface_type_ == GAME_INTERFACE_TYPE::IN_GAME ||
            game_interface_type_ == GAME_INTERFACE_TYPE::LOADING)
        {
            /* 正在游戏或正在加载，保持原有状态不变 */
            analyzed_in_game_state = GetInGameState();
        }
        else
        {
            analyzed_in_game_state = GAME_INTERFACE_TYPE::UNKNOWN;
        }
    }
    else if (analyzed_in_game_state != GAME_INTERFACE_TYPE::UNKNOWN &&
             game_interface_type_ != GAME_INTERFACE_TYPE::UNKNOWN)
    {
        switch (analyzed_in_game_state)
        {
        case GAME_INTERFACE_TYPE::LOBBY:
            Console::Info(Translate("IdleEngine::OCR::INFO_Lobby"));
            break;
        case GAME_INTERFACE_TYPE::IN_GAME:
            Console::Info(Translate("IdleEngine::OCR::INFO_Gaming"));
            break;
        case GAME_INTERFACE_TYPE::LOADING:
            Console::Info(Translate("IdleEngine::OCR::INFO_Loading"));
            break;
        case GAME_INTERFACE_TYPE::ROOM:
            Console::Info(Translate("IdleEngine::OCR::INFO_Room"));
            break;
        default:
            break;
        }
    }
    /* 非法状态迁移，4 种 */
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOADING &&
             analyzed_in_game_state == GAME_INTERFACE_TYPE::ROOM)
    /* 加载失败，现象一般为重连 1 2 3，可能是网络问题 */
    {
        analyzed_in_game_state = GAME_INTERFACE_TYPE::LOBBY; /* 直接离开房间回到大厅 */
        Console::Warn(Translate("IdleEngine::OCR::WARN_LoadGameMap"));
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::IN_GAME &&
             analyzed_in_game_state == GAME_INTERFACE_TYPE::LOBBY)
    /* 从游戏场景返回到大厅，原因一般为：强制踢出、长时间没有有效操作 */
    {
        analyzed_in_game_state = GAME_INTERFACE_TYPE::LOBBY;
        Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromGamingToLobby"));
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::ROOM && analyzed_in_game_state == GAME_INTERFACE_TYPE::LOBBY)
    /* 从游戏房间返回到大厅，原因可能是：强制踢出、房间等待时间过长 */
    {
        analyzed_in_game_state = GAME_INTERFACE_TYPE::LOBBY;
        Console::Warn(Translate("IdleEngine::OCR::WARN_ReturnFromRoomToLobby"));
    }
    else /* 其他情形，除非手动操作，否则不会出现 */
    {
        switch (analyzed_in_game_state)
        {
        case GAME_INTERFACE_TYPE::LOBBY:
            Console::Info(Translate("IdleEngine::OCR::INFO_Lobby"));
            break;
        case GAME_INTERFACE_TYPE::IN_GAME:
            Console::Info(Translate("IdleEngine::OCR::INFO_Gaming"));
            break;
        case GAME_INTERFACE_TYPE::LOADING:
            Console::Info(Translate("IdleEngine::OCR::INFO_Loading"));
            break;
        case GAME_INTERFACE_TYPE::ROOM:
            Console::Info(Translate("IdleEngine::OCR::INFO_Room"));
            break;
        default:
            break;
        }
    }

    /* 超时检查 */
    if (game_interface_type_ == GAME_INTERFACE_TYPE::ROOM &&
        recognize_start - timepoint_ > std::chrono::seconds(Global::StartGameRoomTimeout))
    {
        analyzed_in_game_state = GAME_INTERFACE_TYPE::LOBBY;
        Console::Warn(Translate("IdleEngine::OCR::WARN_WaitStartGameRoomTimeout@1", Global::StartGameRoomTimeout));
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOGIN &&
             recognize_start - timepoint_ > std::chrono::seconds(Global::LoginTimeout))
    /* 登录超时 */
    {
        analyzed_in_game_state = GAME_INTERFACE_TYPE::UNKNOWN; /* 置为未知状态 */
        Console::Warn(Translate("IdleEngine::OCR::WARN_WaitLoginTimeout@1", Global::LoginTimeout));
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOADING &&
             recognize_start - timepoint_ > std::chrono::seconds(Global::LoadMapTimeout))
    /* 游戏加载超时 */
    {
        Console::Warn(Translate("IdleEngine::OCR::WaitLoadGameMapTimeout@1", Global::LoadMapTimeout));
        DWORD dwPId = 0;
        if (Global::RestartGameOnLoadingTimeout)
        {
            GetWindowThreadProcessId(game_process_info_->get_window_handle(), &dwPId);
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
            analyzed_in_game_state = GAME_INTERFACE_TYPE::UNKNOWN;
        }
        else
        {
            analyzed_in_game_state = GAME_INTERFACE_TYPE::IN_GAME;
        }
    }
    /* 更新状态 */
    SetInGameState(analyzed_in_game_state);
}

void OCRIdleEngine::Dispatch()
{
    Command::TYPE command;

    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    if (game_interface_type_ == GAME_INTERFACE_TYPE::UNKNOWN)
    {
        command = Command::TYPE::CMD_CLEAR_POPUPS;
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOGIN) /* 正在登陆 */
    {
        command = Command::TYPE::CMD_NOP;
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOBBY) /* 大厅内 */
    {
        command = Command::TYPE::CMD_CREATE_GAME_ROOM;
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::ROOM) /* 房间内（正常） */
    {
        command = Command::TYPE::CMD_START_GAME_ROOM;
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::LOADING) /* 加载 */
    {
        command = Command::TYPE::CMD_NOP;
    }
    else if (game_interface_type_ == GAME_INTERFACE_TYPE::IN_GAME) /* 游戏中 */
    {
        if (tp - timepoint_ > std::chrono::seconds(60) &&
            GetIdleMode() == IDLE_MODE::EXTENDED) /* 扩展模式启用，且进入游戏达到
                                                     60 秒 */
        {
            command = Command::TYPE::CMD_EXTENDED_IDLE;
#ifdef _DEBUG
            Console::Debug("扩展挂机模式。");
#endif
        }
        else if (tp - timepoint_ > std::chrono::seconds(7)) /* 角色选择完成，进入挂机模式 */
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

void OCRIdleEngine::Recognize(std::vector<std::string> &results)
{
    thread_local std::vector<uint8_t> buffer;
    thread_local auto capture_error_count{0};
    results.clear(); /* 清空结果 */
    auto game_window_handle = game_process_info_->get_window_handle();
    if (!IsWindow(game_window_handle))
    {
        return;
    }
    try
    {
        CaptureWindowAsBmp(game_window_handle, buffer);
#ifdef _DEBUG
        std::ofstream bmp_file("capture.bmp", std::ios::binary | std::ios::out | std::ios::trunc);
        bmp_file.write(reinterpret_cast<char *>(buffer.data()), buffer.size());
#endif
    }
    catch (std::exception &e)
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

    thread_local auto decode_error_count{0};
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
    auto ocr_results = ocr_->Detect(mat);
    results.clear();
    for (auto &r : ocr_results)
    {
        results.emplace_back(std::move(r.text));
    }
}
