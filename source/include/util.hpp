#pragma once
#include <Windows.h>
#include <memory>
#include <winnt.h>

enum REG_PREDEFINED_KEY_ENUM
{
    REG_CLASSES_ROOT,
    REG_CURRENT_CONFIG,
    REG_CURRENT_USER,
    REG_LOCAL_MACHINE,
    REG_USERS
};

std::shared_ptr<wchar_t[]> ConvertUtf8ToUtf16(const void* byte_string);
std::shared_ptr<char[]> ConvertUtf16ToUtf8(const void* byte_string);