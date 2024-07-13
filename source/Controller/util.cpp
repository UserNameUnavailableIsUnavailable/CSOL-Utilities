#include <Windows.h>
#include <memory>
#include "util.hpp"

std::shared_ptr<wchar_t[]> ConvertUtf8ToUtf16(const void* byte_string)
{
    auto length = MultiByteToWideChar(
        CP_UTF8,
        0,
        (char*)(byte_string),
        -1,
        nullptr,
        0
    );
    std::shared_ptr<wchar_t[]> ret(new wchar_t[length]);
    MultiByteToWideChar(
        CP_UTF8,
        0,
        (char*)byte_string,
        -1,
        ret.get(),
        length
    );
    return ret;
}

std::shared_ptr<char[]> ConvertUtf16ToUtf8(const void* byte_string)
{
    auto length = WideCharToMultiByte(
        CP_UTF8,
        0,
        (wchar_t*)byte_string,
        -1,
        nullptr,
        0,
        nullptr,
        nullptr
    );
    std::shared_ptr<char[]> ret(new char[length]);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        (wchar_t*)byte_string,
        -1,
        ret.get(),
        length,
        nullptr,
        nullptr
    );
    return ret;
}