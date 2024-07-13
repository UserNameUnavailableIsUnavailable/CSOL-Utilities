#include <cstdint>
#include <string>
#include <iostream>

void split(const char* buffer, int64_t size) noexcept
{
    if (size <= 0) return;
    int64_t begin = size;
    int64_t end = size;
    int64_t sec_since_1970;
    std::string line;
    /* 按照 CRLF 方式划分子串 */
    while (begin >= 0)
    {
        if (begin == 0 && begin != end) /* [begin, end) 可划分为子串 */
        {
            line = std::string(buffer + begin, end - begin);
            begin--; /* begin 减小为负数后退出 */
            std::cout << "meet string boundary" << std::endl;
        }
        else if (buffer[begin - 1] == '\n' && begin != end) /* [begin, end) 可划分为子串 */
        {
            line = std::string(buffer + begin, end - begin);
            begin -= 2; /* 越过 \r\n */
            end = begin;
        }
        else if (buffer[begin - 1] == '\n' && begin == end) /* [begin, end) 长度为 0 */
        {
            begin -= 2;
            end = begin;
            std::cout << "empty string" << std::endl;
            continue;
        }
        else
        {
            begin--;
            continue;
        }
        std::cout << line << std::endl;
    }
}

int main()
{
    const char* string = "Acc\r\nB\r\nCdef\r\n\r\n";
    split(string, strlen(string));
}