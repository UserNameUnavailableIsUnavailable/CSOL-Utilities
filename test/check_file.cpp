#include <Windows.h>
#include <cstddef>
#include <cwchar>
#include <errhandlingapi.h>
#include <exception>
#include <fileapi.h>
#include <filesystem>
#include <handleapi.h>
#include <memory>
#include <minwindef.h>
#include <string>
#include <cwchar>
#include <iostream>
#include <winnt.h>
#include "util.hpp"
#include "game.hpp"
#include "csol24except.hpp"
#include "filesystem"

int main()
{
    std::shared_ptr<wchar_t[]> path;
    size_t qwLength;
    std::shared_ptr<wchar_t[]> error_log_path;
    try
    {
        path = query_installation_path();
        std::cout << ConvertUtf16ToUtf8(path.get()) << std::endl;
        qwLength = wcslen(path.get()) + wcslen(L"\\bin\\Error.log") + 1;
        error_log_path = std::shared_ptr<wchar_t[]>(new wchar_t[qwLength]);
        wcscpy_s(error_log_path.get(), qwLength, path.get());
        wcscat_s(error_log_path.get(), qwLength, L"\\bin\\Error.log");
    }
    catch (CSOL24EXCEPT e)
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
            throw CSOL24EXCEPT("打开文件 .../Error.log 失败！错误代码 %lu。", GetLastError());
        }
    }
    catch (CSOL24EXCEPT e)
    {
        NotifyError(e.what());
    }
    DWORD dwLow;
    ULONGLONG qwSize;
    dwLow = GetFileSize(hFile, (LPDWORD)&qwSize);
    qwSize = dwLow | (qwSize << 32);
    qwSize |= dwLow;
    std::cout << "File size " << qwSize << std::endl;
    while (true)
    {
        DWORD dwChangedLow;
        ULONGLONG qwChangedSize;
        dwChangedLow = GetFileSize(hFile, (LPDWORD)&qwChangedSize);
        qwChangedSize = dwChangedLow | (qwChangedSize << 32);
        if (qwChangedSize != qwSize)
        {
            qwSize = qwChangedSize;
            break;
        }
        Sleep(5);
    }
    std::cout << "File size changed to " << qwSize << std::endl;
    CloseHandle(hFile);
}