#include <Windows.h>
#include <cstddef>
#include <cwchar>
#include <fileapi.h>
#include <handleapi.h>
#include <string>
#include <cwchar>
#include <iostream>
#include <winnt.h>
#include "util.hpp"
#include "game.hpp"

int main()
{
    auto path = query_installation_path();
    auto qwLength = std::wcslen(path.get()) + std::wcslen(L"\\bin\\Error.log");
    auto error_log_path = std::shared_ptr<wchar_t[]>(new wchar_t[qwLength]);
    wcscpy_s(error_log_path.get(), qwLength, path.get());
    wcscat_s(error_log_path.get(), qwLength, L"\\bin\\Error.log");
    HANDLE hFile = CreateFileW(
        error_log_path.get(), 
        GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, 
        nullptr, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        nullptr
    );
    std::cout << "File " << ConvertUtf16ToUtf8(error_log_path.get()).get() << " opened successfully." << std::endl;
    ULONGLONG size;
    ULONGLONG changed_size;
    *(LPDWORD)size = GetFileSize(hFile, (LPDWORD)&size + 1);
    std::cout << "File size " << size << std::endl;
    while (true)
    {
        *(LPDWORD)size = GetFileSize(hFile, (LPDWORD)&size + 1);
        Sleep(5);
    }
    std::cout << "File size changed to " << changed_size << std::endl;
    CloseHandle(hFile);
}