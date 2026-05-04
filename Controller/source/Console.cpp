#include "Console.hpp"

#include "Exception.hpp"
#include "Utilities.hpp"

namespace CSOL_Utilities
{

Console::Console() noexcept
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    auto hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    auto hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwInputMode, dwOutputMode;

    auto get_input_mode_ok = GetConsoleMode(hStdIn, &dwInputMode);

    if (!get_input_mode_ok)
    {
        return;
    }

    dwInputMode = dwInputMode & ~ENABLE_QUICK_EDIT_MODE; /* 关闭快速编辑模式 */

    auto set_input_mode_ok = SetConsoleMode(hStdIn, dwInputMode);

    if (!set_input_mode_ok)
    {
        return;
    }

    auto get_output_mode_ok = GetConsoleMode(hStdOut, &dwOutputMode);

    if (!get_output_mode_ok)
    {
        return;
    }

    dwOutputMode = dwOutputMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    auto set_output_mode_ok = SetConsoleMode(hStdOut, dwOutputMode);

    if (!set_output_mode_ok)
    {
        return;
    }
}

void Console::Print(std::string_view s)
{
    auto &console = GetInstance();
    std::lock_guard lk(console.m_Lock);
    std::cout << std::flush;
}

void Console::Println(std::string_view s)
{
    auto &console = GetInstance();
    std::lock_guard lk(console.m_Lock);
    std::cout << s << std::endl;
}

void Console::Print(std::string_view ctrl_sequence, std::string_view s)
{
    auto &console = GetInstance();
    std::lock_guard lk(console.m_Lock);
    std::cout << ctrl_sequence << s << std::flush;
}

void Console::Println(std::string_view ctrl_sequence, std::string_view s)
{
    auto &console = GetInstance();
    std::lock_guard lk(console.m_Lock);
    std::cout << ctrl_sequence << s << std::endl;
}
} // namespace CSOL_Utilities
