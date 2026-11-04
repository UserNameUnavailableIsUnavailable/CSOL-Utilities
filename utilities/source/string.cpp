#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace utilities {
std::wstring Utf8ToUtf16(std::string_view u8) {
    if (u8.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max())) [[unlikely]] {
        throw std::length_error("Input string is too long");
    }
    auto required = ::MultiByteToWideChar(CP_UTF8, 0, u8.data(), static_cast<int>(u8.size()), nullptr, 0);
    assert(required >= 0); // documented
    std::wstring u16(static_cast<size_t>(required), L'0');
    u16.resize(static_cast<size_t>(required));
    auto chars_written = ::MultiByteToWideChar(CP_UTF8, 0, u8.data(), static_cast<int>(u8.size()), u16.data(), required);
    assert(chars_written == required); // documented
    return u16;

}

std::string Utf16ToUtf8(std::wstring_view u16)
{
    if (u16.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max())) [[unlikely]] {
        throw std::length_error("Input string is too long");
    }
    auto required = ::WideCharToMultiByte(CP_UTF8, 0, u16.data(), static_cast<int>(u16.size()), nullptr, 0, nullptr, nullptr);
    assert(required >= 0); // documented
    std::string u8;
    u8.resize(static_cast<size_t>(required));
    auto chars_written =
        ::WideCharToMultiByte(CP_UTF8, 0, u16.data(), static_cast<int>(u16.size()), u8.data(), required, nullptr, nullptr);
    assert(chars_written == required); // documented
    return u8;
}
} // namespace utilities