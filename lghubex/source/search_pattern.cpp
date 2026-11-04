#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <lghubex/pattern.hpp>

namespace lghubex {
static std::expected<std::span<const std::uint8_t>, std::error_code> GetTextSection(std::uintptr_t module_base)
{
    if (module_base == 0) return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(module_base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return std::unexpected(std::make_error_code(std::errc::executable_format_error));

    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<std::uint8_t*>(dos) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return std::unexpected(std::make_error_code(std::errc::executable_format_error));

    auto* section = IMAGE_FIRST_SECTION(nt);
    if (section == nullptr) return std::unexpected(std::make_error_code(std::errc::executable_format_error));

    // find .text signature
    for (std::uint16_t i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (std::memcmp(section->Name, ".text", sizeof(".text")) == 0) {
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(module_base + section->VirtualAddress),
                section->Misc.VirtualSize
            );
        }
        ++section;
    }
    return std::unexpected(std::make_error_code(std::errc::executable_format_error));
}

std::uintptr_t SearchPattern(std::uintptr_t hMod, std::span<const std::optional<std::uint8_t>> pattern)
{
    if (hMod == NULL) {
        return 0;
    }
    auto text = GetTextSection(hMod);
    if (!text)  {
        return 0;
    }
    if (pattern.empty() || pattern.size() > text->size()) {
        return 0;
    }

    const std::size_t last_start = text->size() - pattern.size();
    for (std::size_t start = 0; start <= last_start; ++start) {
        std::size_t index = 0;
        for (; index < pattern.size(); ++index) {
            if (pattern[index].has_value() && (*text)[start + index] != *pattern[index]) {
                break;
            }
        }
        if (index == pattern.size()) {
            return reinterpret_cast<std::uintptr_t>(text->data() + start);
        }
    }
    return 0;
}
} // namespace lghubex