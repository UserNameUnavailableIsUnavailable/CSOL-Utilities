#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <errhandlingapi.h>
#include <exception>
#include <fileapi.h>
#include <filesystem>
#include <handleapi.h>
#include <memory>
#include <minwindef.h>
#include <processenv.h>
#include <string>
#include <cwchar>
#include <iostream>
#include <wincon.h>
#include <winnt.h>
#include "global.hpp"
#include "util.hpp"
#include "game.hpp"
#include "csol24except.hpp"
#include "filesystem"

static void cls(HANDLE hConsole)
{
    COORD coordScreen = { 0, 0 };    // home for the cursor
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the entire screen with blanks.
    if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
                                    (TCHAR)' ',      // Character to write to the buffer
                                    dwConSize,       // Number of cells to write
                                    coordScreen,     // Coordinates of first cell
                                    &cCharsWritten)) // Receive number of characters written
    {
        return;
    }

    // Get the current text attribute.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    // Set the buffer's attributes accordingly.
    if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
                                    csbi.wAttributes, // Character attributes to use
                                    dwConSize,        // Number of cells to set attribute
                                    coordScreen,      // Coordinates of first cell
                                    &cCharsWritten))  // Receive number of characters written
    {
        return;
    }

    // Put the cursor at its home coordinates.
    SetConsoleCursorPosition(hConsole, coordScreen);
}


int main()
{
    std::shared_ptr<wchar_t[]> path;
    size_t qwLength;
    std::shared_ptr<wchar_t[]> error_log_path;
    try
    {
        path = CSOL24H::QueryInstallationPath();
        std::cout << ConvertUtf16ToUtf8(path.get()) << std::endl;
        qwLength = wcslen(path.get()) + wcslen(L"\\bin\\Error.log") + 1;
        error_log_path = std::shared_ptr<wchar_t[]>(new wchar_t[qwLength]);
        wcscpy_s(error_log_path.get(), qwLength, path.get());
        wcscat_s(error_log_path.get(), qwLength, L"\\bin\\Error.log");
    }
    catch (CSOL24HEXCEPT e)
    {
        NotifyError(e.what());
        return -1;
    }
    HANDLE hFile;
    try
    {
        hFile = CreateFileW(
            error_log_path.get(), 
            GENERIC_READ, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, 
            nullptr, 
            OPEN_EXISTING, 
            FILE_ATTRIBUTE_NORMAL, 
            nullptr
        );
        if (hFile == INVALID_HANDLE_VALUE)
        {
            throw CSOL24HEXCEPT("打开文件 .../Error.log 失败！错误代码 %lu。", GetLastError());
        }
    }
    catch (CSOL24HEXCEPT e)
    {
        NotifyError(e.what());
    }
    CSOL24H::UpdateGameState(hFile);
    CloseHandle(hFile);
    // DWORD dwLow;
    // ULONGLONG qwSize;
    // dwLow = GetFileSize(hFile, (LPDWORD)&qwSize);
    // qwSize = dwLow | (qwSize << 32);
    // qwSize |= dwLow;
    // std::cout << "File size " << qwSize << std::endl;
    // while (true)
    // {
    //     DWORD dwChangedLow;
    //     ULONGLONG qwChangedSize;
    //     dwChangedLow = GetFileSize(hFile, (LPDWORD)&qwChangedSize);
    //     qwChangedSize = dwChangedLow | (qwChangedSize << 32);
    //     if (qwChangedSize != qwSize)
    //     {
    //         qwSize = qwChangedSize;
    //         break;
    //     }
    //     Sleep(5);
    // }
    // std::cout << "File size changed to " << qwSize << std::endl;
}