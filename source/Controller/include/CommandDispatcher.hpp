#pragma once

#include <Windows.h>
#include <filesystem>
#include <string>
#include <winbase.h>
#include <winnt.h>
#include "Command.hpp"
#include "Module.hpp"

namespace CSOL_Utilities
{
    class CommandDispatcher : public Module
    {
    public:
        CommandDispatcher(std::shared_ptr<Command> cmd);
        ~CommandDispatcher() noexcept;
        void dispatch(COMMAND cmd, bool renew = false, bool repeatable = false);
        virtual void work() override;
    private:
        std::shared_ptr<Command> m_cmd; /* 命令对象 */
        std::string m_content; /* 命令内容 */
        std::filesystem::path m_file_path; /* 用 filesystem 处理文件路径 */
        HANDLE m_hFile; /* 命令内容大小基本固定，直接采用 Win32 API 写命令以提高效率 */
    };

}