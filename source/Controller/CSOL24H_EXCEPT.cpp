#include "CSOL24H_EXCEPT.hpp"

const char* CSOL24H_EXCEPT::what() noexcept
{
    return message;
}

void NotifyError(const char *lpszErrorMessage) noexcept
{
    wchar_t wmessage[ERROR_MESSAGE_LENGTH];
    MultiByteToWideChar(
        CP_UTF8,
        0,
        lpszErrorMessage,
        -1,
        wmessage,
        ERROR_MESSAGE_LENGTH
    );
    MessageBoxW(nullptr, L"CSOL 24H 错误", wmessage, MB_OK);
}
