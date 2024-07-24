#pragma once
#include <exception>
#include <Windows.h>

#define ERROR_MESSAGE_LENGTH (512)

class CSOL24H_EXCEPT : std::exception
{
public:
    template<typename... ARG>
    explicit CSOL24H_EXCEPT(const char* fmt, const ARG&... args)
    {
        sprintf_s(
            message,
            sizeof(message) / sizeof(char),
            fmt,
            args...
        );
    }
    const char* what() noexcept;
private:
    char message[ERROR_MESSAGE_LENGTH];
};

void NotifyError(const char *lpszFmt) noexcept;

template<typename... ARG>
void NotifyError(const char *lpszFmt, const ARG&... args) noexcept
{
    char message[ERROR_MESSAGE_LENGTH];
    sprintf_s(
        message,
        sizeof(message) / sizeof(char),
        lpszFmt,
        args...
    );
    wchar_t wmessage[ERROR_MESSAGE_LENGTH];
    MultiByteToWideChar(
        CP_UTF8,
        0,
        message,
        -1,
        wmessage,
        ERROR_MESSAGE_LENGTH
    );
    MessageBoxW(nullptr, L"CSOL 24H 错误", wmessage, MB_OK);
}
