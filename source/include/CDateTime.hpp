#pragma once

#include "CSOL_Utilities.hpp"
#include <ctime>
#include <filesystem>
#include <regex>

namespace CSOL_Utilities
{
class CDateTime
{
  public:
    CDateTime() = delete;
    ~CDateTime() = delete;
    static bool IsLeap(uint32_t year) noexcept;
    static std::time_t CreateUnixTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute,
                                      uint32_t second, float time_zone) noexcept;
    static std::time_t ResolveMessageTimestamp(const std::string &message, std::time_t midnight_timestamp,
                                               uint32_t *p_ms, std::time_t time_bias) noexcept;
    static std::time_t GetTimeBias() noexcept;
    static std::time_t FileTimeToUnixTime(const std::filesystem::file_time_type &file_time_point) noexcept;
};
}; // namespace CSOL_Utilities