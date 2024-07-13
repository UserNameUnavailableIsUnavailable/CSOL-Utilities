#include "CSOL24H.hpp"

/*
@brief 判断 `dwYear` 是否为闰年。
@return 判断结果。
*/
static bool is_leap(uint32_t dwYear)
{
    if (dwYear % 4 == 0 && dwYear % 100 != 0)
    {
        return true;
    }
    else if (dwYear % 400 == 0)
    {
        return true;
    }
    return false;
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
uint64_t CSOL24H::GetUNIXTimestamp(uint32_t dwYear, uint32_t dwMonth, uint32_t dwDay, uint32_t dwHour, uint32_t dwMinute, uint32_t dwSecond, float fTimeZone)
{
    uint64_t ret = 0ull;
    uint64_t total_days = 0ull;
    uint64_t i;
    for (i = 1970; i < dwYear; i++)
    {
        if (is_leap(i))
            total_days += 366;
        else
            total_days += 365;
    }
    i = 1;
    for (i = 1; i < dwMonth; i++)
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
    if (dwMonth > 2 && is_leap(dwYear)) /* 已过 2 月，且为闰年 */
    {
        total_days++;
    }
    total_days += dwDay - 1;
    ret = total_days * 86400 + dwHour * 3600 + dwMinute * 60 + dwSecond;
    ret -= fTimeZone * 3600;
    return ret;
}