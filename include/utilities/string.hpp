#pragma once

namespace utilities {
std::wstring Utf8ToUtf16(std::string_view u8);
std::string Utf16ToUtf8(std::wstring_view u16);
} // namespace utilities
