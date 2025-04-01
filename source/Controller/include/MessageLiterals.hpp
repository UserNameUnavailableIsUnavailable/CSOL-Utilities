#pragma once

#define CSOL_Utilities_ZH_CN

#ifdef CSOL_Utilities_ZH_CN

namespace CSOL_Utilities
{
    namespace MessageLiterals
    {
        // 模块
        namespace MODULE
        {
            static constexpr const char* MSG_NULL_TOKEN = "不允许提供空的令牌。";
            static constexpr const char* MSG_MODULE_NORMAL_EXIT = "模块 %s 正常退出。";
            static constexpr const char* MSG_MODULE_ABNORMAL_EXIT = "模块 %s 异常退出。";
        }
        // 游戏进程
        namespace GAME_PROCESS_DETECTOR
        {
            static constexpr const char* MOD_GAME_PROCESS_DETECTOR = "游戏进程探测器";
            static constexpr const char* MSG_GAME_PROCESS_EXIT = "游戏进程退出";
            static constexpr const char* FMT_SUCCESSFULLY_OPEN_GAME_PROCESS = "成功获取游戏进程句柄。游戏进程标识符：%lu。";
            static constexpr const char* FMT_FAILED_TO_OPEN_GAME_PROCESS = "尝试获取游戏进程句柄失败。错误代码：%lu。";
            static constexpr const char* FMT_FAILED_TO_CREATE_GAME_PROCESS = "尝试通过游戏启动器创建游戏进程失败，请手动运行游戏进程。错误代码：%lu。";
            static constexpr const char* FMT_GAME_PROCESS_DETECTED = "检测到游戏进程正在运行。游戏进程标识符：%lu。";
        }
    }
}

#endif