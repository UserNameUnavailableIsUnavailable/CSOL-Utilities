#include <cstdio>
#include <iterator>
#include <regex>
#include <iostream>

#define DATE_STRING 

int main()
{
    std::string text = "16:34:26.954 -  -   07/09/2024 - 16:33:26 "
    "02:35:26.954 -  -   07/09/2024 - 16:33:26";
    std::regex date_pattern("(\\d{2}):(\\d{2}):(\\d{2})\\.([0-9]{3}).+?([0-9]{2})/([0-9]{2})/([0-9]{4})");
    std::smatch smatch_results;
    if (!std::regex_search(text, smatch_results, date_pattern))
    {
        return 0;
    }
    std::cout << smatch_results.size() << std::endl;
    uint32_t hour, minute, second, millisec;
    uint32_t month, day, year;
    std::smatch::iterator iter = smatch_results.begin() + 1;
    hour = std::atoi((iter++)->str().c_str());
    minute = std::atoi((iter++)->str().c_str());
    second = std::atoi((iter++)->str().c_str());
    millisec = std::atoi((iter++)->str().c_str());
    month = std::atoi((iter++)->str().c_str());
    day = std::atoi((iter++)->str().c_str());
    year = std::atoi((iter++)->str().c_str());
    std::printf("%04u/%02u/%02u %02u:%02u:%02u.%03u\n", year, month, day, hour, minute, second, millisec);
}
