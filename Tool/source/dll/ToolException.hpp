#pragma once
#include <exception>
#include <cstdio>
#include <Windows.h>

#define LENGTHOF(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

class ToolException : public std::exception
{
public:
    template<typename... ARG>
    explicit ToolException(const char* fmt, const ARG&... args)
    {
        sprintf_s(
            message,
            LENGTHOF(message), 
            fmt,
            args...
        );
    }
    ~ToolException()
    {

    }
    const char* what() noexcept
    {
        return message;
    }
    void notify() noexcept
    {
        MultiByteToWideChar(
            CP_UTF8,
            0,
            message,
            -1,
            wmessage,
            ERROR_MESSAGE_LENGTH
        );
        MessageBoxW(
            NULL,
            wmessage,
            L"Tool",
            MB_OK
        );
    }
private:
    static const size_t ERROR_MESSAGE_LENGTH{ 256 };
    wchar_t wmessage[ERROR_MESSAGE_LENGTH];
    char message[ERROR_MESSAGE_LENGTH];
};

