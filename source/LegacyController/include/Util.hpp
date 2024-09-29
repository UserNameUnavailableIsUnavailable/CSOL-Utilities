#pragma once
#include <Windows.h>
#include <memory>
#include <winnt.h>

std::shared_ptr<wchar_t[]> ConvertUtf8ToUtf16(const void* byte_string);
std::shared_ptr<char[]> ConvertUtf16ToUtf8(const void* byte_string);