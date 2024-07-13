#include <cstdint>
#include <cstring>
#include <regex>
#include "CSOL24H.hpp"
#include "CSOL24H_EXCEPT.hpp"

/*
@brief 解析日志日期，以时间戳形式返回。
@param `lpBuffer` 日志存储的缓冲区。
@param `dwLength` 日志文件长度。
@return 解析所得日期的 00:00:00 时刻（非 UTC 标准时）的 UNIX 时间戳。
*/
int64_t CSOL24H::ResolveLogDate(LPCSTR lpBuffer, INT64 cbLength) noexcept
{
    std::regex date_pattern("\\d{1,2}:\\d{2}:\\d{2}\\.\\d{3}.+?(\\d{2})/(\\d{2})/(\\d{4})");
    std::time_t ret = 0llu;
    INT32 begin = 0, end = 1;
    while (end < cbLength)
    {
        if (lpBuffer[end] != '\n')
        {
            end++;
            continue;
        }
        std::string line(lpBuffer + begin, end - begin);
        begin = end + 1;
        end = begin;
        std::smatch smatch_result;
        if (
            std::regex_search(line, smatch_result, date_pattern) &&
            smatch_result.size() - 1 == 3
        )
        {
            auto month = std::atoi(smatch_result[1].str().c_str());
            auto day = std::atoi(smatch_result[2].str().c_str());
            auto year = std::atoi(smatch_result[3].str().c_str());
            ret = GetUNIXTimestamp(year, month, day);
            break;
        }
    }
    return ret;
}

/*
@brief 更新日志文件缓冲区。
@return 日志文件缓冲区是否更新。
*/
bool CSOL24H::UpdateErrorLogBuffer() noexcept
{
    DWORD dwBytesWritten;
    uint64_t qwLogFileLastModifiedTime = UINT64_MAX;
    GetFileTime(hErrorLogFile, nullptr, nullptr, (LPFILETIME)&qwLogFileLastModifiedTime);
    if (qwLogBufferLastModifiedTime != qwLogFileLastModifiedTime) /* 文件已经被修改，需要加载 */
    {
        qwLogBufferLastModifiedTime = qwLogFileLastModifiedTime;
        bLogBufferResolved = false;
        INT64 cbUpdatedLog = 0;
        DWORD dwNewSizeHigh = 0;
        DWORD dwNewSizeLow = GetFileSize(hErrorLogFile, &dwNewSizeHigh);
        cbUpdatedLog = dwNewSizeLow | ((INT64)dwNewSizeHigh << 32);
        if (cbUpdatedLog > cbErrorLogSize) /* 文件变大且时间戳非零（初始状态），则说明只是向 Error.log 追加新内容，故只需要读取新增部分 */
        {
            ReadFile(
                hErrorLogFile,
                lpErrorLogBuffer + cbErrorLogSize, /* append to buffer */
                dwNewSizeLow - cbErrorLogSize, /* new content */
                &dwBytesWritten,
                nullptr
            );
            cbErrorLogSize += dwBytesWritten;
        }
        else
        {
            SetFilePointer(
                hErrorLogFile,
                0,
                0,
                FILE_BEGIN
            );
            ReadFile(hErrorLogFile, lpErrorLogBuffer, dwNewSizeLow, &dwBytesWritten, nullptr);
            cbErrorLogSize = dwBytesWritten;
        }
        qwLogFileDate = ResolveLogDate(lpErrorLogBuffer, cbErrorLogSize); /* 更新日志时间 */
        return true;
    }
    return false;
}

/*
@brief 将消息中的时间解析为 UNIX 时间戳（调整为 UTC 标准时）。
@param `message` 消息。
@return 成功时返回时间戳，失败时返回 `0`。
*/
int64_t CSOL24H::ResolveMessageTimestamp(const std::string message, int32_t* lpMilliseconds) noexcept
{
    std::regex time_pattern("(\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{3})"); /* 忽略毫秒 */
    std::smatch match;
    int32_t hour = 0, minute = 0, second = 0, millisecond = 0;
    int64_t ret;
    if (
        std::regex_search(message, match, time_pattern) &&
        match.size() - 1 == 4 /* 时、分、秒、毫秒 */
    )
    {
        hour = std::atoi(match[1].str().c_str());
        minute = std::atoi(match[2].str().c_str());
        second = std::atoi(match[3].str().c_str());
        millisecond = std::atoi(match[4].str().c_str());
        ret = qwLogFileDate + hour * 3600 + minute * 60 + second + time_bias;
    }
    if (lpMilliseconds) *lpMilliseconds = millisecond;
    return ret;
}
