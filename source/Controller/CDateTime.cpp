#include "CDateTime.hpp"
#include <ctime>
#include <string>
#include <regex>
#include <timezoneapi.h>

using namespace CSOL_Utilities;

/*
@brief 判断 `dwYear` 是否为闰年。
@return 判断结果。
*/
bool CDateTime::IsLeap(uint32_t year) noexcept
{
    if (year % 4 == 0 && year % 100 != 0)
    {
        return true;
    }
    else if (year % 400 == 0)
    {
        return true;
    }
    return false;
}

/*
@brief 将消息中的时间解析为 UNIX 时间戳（调整为 UTC 标准时）。
@param `message` 消息。
@param `midnight_timestamp` 该消息发送当日午夜时间戳（LTC 00:00）。
@param `p_ms` 消息中包含的毫秒。
@param `time_bias` 时差，time_bias = LTC - UTC，如东八区（UTC + 08:00）时差为 8 * 60 * 60 = 28800
@return 成功时返回时间戳，失败时返回 `0`。
*/
std::time_t CDateTime::ResolveMessageTimestamp(const std::string& message, std::time_t midnight_timestamp, uint32_t* p_ms, std::time_t time_bias) noexcept
{
    static std::regex time_pattern("(\\d{1,2}):(\\d{2}):(\\d{2})\\.(\\d{3})"); /* 忽略毫秒 */
    std::smatch match;
    std::int32_t hour = 0, minute = 0, second = 0, millisecond = 0;
    std::time_t ret{ 0 };
    if (
        std::regex_search(message, match, time_pattern) &&
        match.size() - 1 == 4 /* 时、分、秒、毫秒 */
    )
    {
        hour = std::atoi(match[1].str().c_str());
        minute = std::atoi(match[2].str().c_str());
        second = std::atoi(match[3].str().c_str());
        millisecond = std::atoi(match[4].str().c_str());
        std::int32_t ltc_day_time = hour * 3600 + minute * 60 + second;
        std::int32_t utc_day_time = ltc_day_time - time_bias;
        if (utc_day_time < 0) /* UTC 比本地少一天 */
        {
            utc_day_time += 24 * 60 * 60;
        }
        else if (utc_day_time > 24 * 60 * 60) /* > 24 * 60 * 60，UTC 比本地多一天 */
        {
            utc_day_time -= 24 * 60 * 60;
        }
        ret = midnight_timestamp /* UTC 当日 00:00:00 时间戳 */ + utc_day_time /* UTC 自从 00:00:00 以来经过的时间 */;
    }
    if (p_ms) *p_ms = millisecond;
    return ret;
}

/*
@brief 将给定事件转换为 UNIX 时间戳。
@param `dwYear = 0`
@param `dwMonth = 0`
@param `dwDay = 0`
@param `dwHour = 0`
@param `dwSecond = 0`
@param `fTimeZone = 0` 时区，东半球为正，西半球为服。如东八区（UTC + 8）对应的 `fTimeZone` 为 `8.0`。
@return UNIX 时间戳。
*/
std::time_t CDateTime::GetUNIXTimestamp(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second, float time_zone) noexcept
{
    uint64_t ret = 0ull;
    uint64_t total_days = 0ull;
    uint64_t i;
    for (i = 1970; i < year; i++)
    {
        if (IsLeap(i))
            total_days += 366;
        else
            total_days += 365;
    }
    i = 1;
    for (i = 1; i < month; i++)
    {
        if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 10 || i == 12)
        {
            total_days += 31;
        }
        else if (i == 2)
        {
            total_days += 28;
        }
        else
        {
            total_days += 30;
        }
    }
    if (month > 2 && IsLeap(year)) /* 已过 2 月，且为闰年 */
    {
        total_days++;
    }
    total_days += day - 1;
    ret = total_days * 86400 + hour * 3600 + minute * 60 + second;
    ret -= time_zone * 3600;
    return ret;
}

/*
@brief 获取本地时间与世界标准时的时差 bias = LTC - UTC。
*/
std::time_t CDateTime::GetTimeBias() noexcept
{
    TIME_ZONE_INFORMATION tzi{ };
    GetTimeZoneInformation(&tzi);
    return -tzi.Bias * 60;
}