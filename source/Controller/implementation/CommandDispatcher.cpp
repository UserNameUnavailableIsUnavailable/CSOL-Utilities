#include "CommandDispatcher.hpp"
#include "Exception.hpp"
#include <fileapi.h>
#include <filesystem>
#include <winbase.h>

namespace CSOL_Utilities
{
    void CommandDispatcher::work()
    {
        DWORD dwBytesWritten;
        m_cmd->get(m_content);
        SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN); // 移动到文件开始处
        if (WriteFile(m_hFile, m_content.c_str(), m_content.length() * sizeof(char), &dwBytesWritten, NULL))
        {
            // 这里不使用 filesystem 的 resize_file，在已经有 Win32 句柄的情况下直接设置 EOF，提高效率
            SetEndOfFile(m_hFile); // 写入 EOF
        }
    }

    CommandDispatcher::CommandDispatcher(std::shared_ptr<Command> cmd) :
        Module("CommandDispatcher", 2000),
        m_cmd(cmd)
    {
        assert(m_cmd);
        std::wstring wpath(128, L'\0');
        while (true)
        {
            unsigned size = 128;
            auto actual_size = GetModuleFileNameW(NULL, wpath.data(), size);
            if (actual_size == 0)
            {
                throw Exception("获取可执行文件路径失败，错误代码：%lu。", GetLastError());
            }
            else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            {
                m_file_path = std::move(wpath);
                break;
            }
            else // insuff buf
            {
                size *= 2;
                wpath.resize(size, L'\0');                    
            }
        }
        m_file_path = m_file_path / "Executor" / "$~cmd.lua";
        m_hFile = CreateFileW(
            m_file_path.wstring().c_str(),
            GENERIC_WRITE,
            FILE_SHARE_READ,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_HIDDEN | FILE_WRITE_FLAGS_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING,
            NULL
        );
        if (m_hFile == INVALID_HANDLE_VALUE)
        {
            throw Exception("打开文件 $~cmd.lua 失败。错误代码：%lu。", GetLastError());
        }
    }
    CommandDispatcher::~CommandDispatcher() noexcept
    {
        CloseHandle(m_hFile);
    }
}