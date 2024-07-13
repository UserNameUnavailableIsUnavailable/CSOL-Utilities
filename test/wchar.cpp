#include <clocale>
#include <cwchar>
#include <iostream>

int main()
{
    char message[32];
    std::setlocale(LC_ALL, ".UTF-8");
    sprintf_s(message, 32, "%ls", L"你好");
    std::puts(message);
}