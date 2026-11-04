#pragma once

#include <cstdint>
#include <optional>
#include <span>

namespace lghubex {
std::uintptr_t SearchPattern(std::uintptr_t hMod, std::span<const std::optional<std::uint8_t>> pattern);
} // namespace lghubex