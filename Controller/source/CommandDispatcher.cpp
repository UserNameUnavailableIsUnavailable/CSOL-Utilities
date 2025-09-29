#include "CommandDispatcher.hpp"

#include "Command.hpp"
#include "Console.hpp"
#include "Exception.hpp"
#include "Utilities.hpp"

using namespace CSOL_Utilities;
void CommandDispatcher::Run(std::stop_token st)
{
    while (true)
    {
        {
            std::unique_lock lk(worker_state_lock_);
            worker_runnable_cond_.wait(lk, [this, &st] { return st.stop_requested() || is_worker_runnable_; });
            if (st.stop_requested())
            {
                break;
            }
            has_worker_finished_ = false;
        }
        thread_local std::string last_cmd;
        thread_local std::string this_cmd;
        Command::Get(this_cmd);
        WriteCommandFile(this_cmd);
        swap(last_cmd, this_cmd); /* 交换当前命令内容及上一次命令，避免重复创建新的字符串对象，提高效率
                                   */
        {
            std::lock_guard lk(worker_state_lock_);
            has_worker_finished_ = true;
        }
        worker_finished_cond_.notify_one();
        SleepEx(1000, true); /* 每隔 1 秒运行一次，但不一定会触发文件写入 */
    }
}

CommandDispatcher::CommandDispatcher(std::filesystem::path command_file_path)
    : cmd_file_path_(std::move(command_file_path))
{
    command_buffer_ =
        reinterpret_cast<BYTE *>(VirtualAlloc(nullptr, COMMAND_BUFFER_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!command_buffer_)
    {
        throw Exception(Translate("ERROR_Win32_API@2", "VirtualAlloc", GetLastError()));
    }

    constexpr auto attrbutes = FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

    file_handle_ = CreateFileW(cmd_file_path_.wstring().c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
                               attrbutes, NULL);
    if (file_handle_ == INVALID_HANDLE_VALUE)
    {
        throw Exception(Translate("ERROR_Win32_API@2", "CreateFileW", ConvertUtf16ToUtf8(cmd_file_path_.wstring())));
    }
}

void CommandDispatcher::Boot()
{
    std::lock_guard lk(boot_lock_);
    if (booted_)
        return; // 防止重复启动
    worker_ = std::thread(
        [this](std::stop_token st) {
            std::string module_name = "CommandDispatcher";
            try
            {
                Run(st);
            }
            catch (std::exception &e)
            {
                Console::Error(Translate("Module::ERROR_ModulePanic@2", module_name, e.what()));
            }
            Console::Info(Translate("Module::INFO_ModuleExited@1", module_name));
        },
        stop_source_.get_token());
    booted_ = true; // 标记为启动
}

void CommandDispatcher::Resume() noexcept
{
    {
        std::lock_guard lk(worker_state_lock_);
        is_worker_runnable_ = true;
    }
    worker_runnable_cond_.notify_one();
}

void CommandDispatcher::Suspend() noexcept
{
    std::unique_lock lk(worker_state_lock_);
    is_worker_runnable_ = false;
    if (stop_source_.get_token().stop_requested())
    {
        return;
    }
    worker_finished_cond_.wait(lk, [this] { return has_worker_finished_; });
}

void CommandDispatcher::Terminate() noexcept
{
    std::lock_guard lk(boot_lock_);
    if (!booted_)
        return; // 未启动
    stop_source_.request_stop();
    worker_runnable_cond_.notify_one();
    QueueUserAPC([](ULONG_PTR) {}, reinterpret_cast<HANDLE>(worker_.native_handle()), 0);
    if (worker_.joinable())
        worker_.join();
    booted_ = false; // 标记为未启动
    WriteCommandFile(Command::NOP());
}

void CommandDispatcher::WriteCommandFile(std::string_view directives)
{
    DWORD dwBytesWritten = 0;
    SetFilePointer(file_handle_, 0, NULL, FILE_BEGIN);
    memset(command_buffer_, ' ', COMMAND_BUFFER_SIZE); // 用空格清空缓冲区
    assert(directives.length() < COMMAND_BUFFER_SIZE); // 命令不可能超过缓冲区大小
    memcpy(command_buffer_, directives.data(),
           directives.length() * sizeof(char)); // 拷贝命令内容
    auto ok = WriteFile(file_handle_, command_buffer_, COMMAND_BUFFER_SIZE, &dwBytesWritten, NULL);
    FlushFileBuffers(file_handle_);
    if (ok)
    {
        // 直接写入 COMMAND_BUFFER_SIZE 字节，文件大小固定，因此不需要截断文件
        // SetFilePointer(file_handle_, dwBytesWritten, NULL, FILE_BEGIN);
        // SetEndOfFile(file_handle_);
    }
    else
    {
        Console::Warn(Translate("ERROR_Win32_API@2", "WriteFile", GetLastError()));
    }
}

CommandDispatcher::~CommandDispatcher() noexcept
{
    Terminate();
    if (file_handle_ && file_handle_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(file_handle_);
        file_handle_ = INVALID_HANDLE_VALUE;
    }
    if (command_buffer_)
    {
        VirtualFree(command_buffer_, 0, MEM_RELEASE);
        command_buffer_ = nullptr;
    }
}
