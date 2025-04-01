#pragma once

#include <atomic>
#include <memory>
#include <string>
#include "Callable.hpp"
#include "Module.hpp"

namespace CSOL_Utilities
{
    class GameProcessDetector : public Module
    {
    public:
        GameProcessDetector(std::wstring&& process_name, const std::wstring& launch_cmd);
        ~GameProcessDetector() noexcept;
        
    private:
        void terminate() noexcept;
        void detect() noexcept;
        void work() override;

    private:
        std::unique_ptr<wchar_t[]> m_LaunchCmd; /* 启动进程的命令 */
        std::wstring m_game_process_name; /* 检测的进程名称 */
        std::unique_ptr<void, decltype(&CloseHandle)> m_hGameProcess; /* 游戏进程 */
    };
}
