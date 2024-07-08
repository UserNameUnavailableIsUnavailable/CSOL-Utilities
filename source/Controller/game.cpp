#include <Windows.h>
#include <cstdint>
#include <fileapi.h>
#include <memory>
#include <minwindef.h>
#include <processenv.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <wincon.h>
#include <winerror.h>
#include "global.hpp"
#include "game.hpp"
#include "util.hpp"
#include "csol24except.hpp"
#include "command.hpp"

/*
* CSOL_QueryInstallationPath 获取 CSOL 安装路径
* @param lpPredefinedTopItem 注册表预定义的项
* @param lpSubItem 注册表子项
* @return CSOL 安装路径
* @note CSOL 安装路径保存在注册表项 HKEY_CURRENT_USER\Software\TCGame\csol\gamepath 中，返回的字符串在堆中
*/

std::shared_ptr<wchar_t[]> query_installation_path(REG_PREDEFINED_KEY_ENUM predefinedTopDir, LPCWSTR lpSubDir, LPCWSTR lpItemName)
{
    HKEY hPredefinedTopDir;
    DWORD dwBufferSize;

    switch (predefinedTopDir)
    {
    case REG_PREDEFINED_KEY_ENUM::REG_CLASSES_ROOT: hPredefinedTopDir = HKEY_CLASSES_ROOT; break;
    case REG_PREDEFINED_KEY_ENUM::REG_CURRENT_CONFIG: hPredefinedTopDir = HKEY_CURRENT_CONFIG; break;
    case REG_PREDEFINED_KEY_ENUM::REG_CURRENT_USER: hPredefinedTopDir = HKEY_CURRENT_USER; break;
    case REG_PREDEFINED_KEY_ENUM::REG_LOCAL_MACHINE: hPredefinedTopDir = HKEY_LOCAL_MACHINE; break;
    case REG_PREDEFINED_KEY_ENUM::REG_USERS: hPredefinedTopDir = HKEY_USERS; break;
    }

    RegGetValueW(
        hPredefinedTopDir,
        lpSubDir,
        lpItemName,
        RRF_RT_REG_SZ,
        nullptr,
        nullptr,
        &dwBufferSize
    ); // 获取需要的缓冲区长度
    std::shared_ptr<wchar_t[]> game_installation_path(new wchar_t[(dwBufferSize - 1) / 2 + 1]{0});
    LSTATUS ret = RegGetValueW(
        hPredefinedTopDir,
        lpSubDir,
        lpItemName,
        RRF_RT_REG_SZ,
        nullptr,
        game_installation_path.get(),
        &dwBufferSize
    ); // 读取字符串
    if (ret != ERROR_SUCCESS)
    {
        throw CSOL24EXCEPT(u8"获取注册表信息失败！错误代码：%ld。", ret);
    }
    return game_installation_path;
}

DWORD CALLBACK update_game_state(LPVOID lpParam)
{
    LPBYTE lpBuffer = new BYTE[1024 * 1024 * 1024]; /* 1 GiB to accomodate the file, impossible to get that large in practice */
    static UINT32 dwLogFileSize = 0xFFFFFFFF; /* log file size, initialize it to maximum */
    static UINT64 qwLogFileLastModifiedTime = 0;
    HANDLE hEvent = lpParam;
    /* track Error.log once event signaled; block otherwise */
    while (WAIT_OBJECT_0 == WaitForSingleObject(hEvent, INFINITE) && !g_bExit)
    {
        HANDLE g_hErrorLogFile;
        // TODO: wait for a waitable object, which indicates if the auto-play is enabled
        UINT64 qwNewLastModifiedTime;
        ULONG dwBytesWritten;
        GetFileTime(g_hErrorLogFile, nullptr, nullptr, (LPFILETIME)&qwNewLastModifiedTime);
        if (qwNewLastModifiedTime != qwLogFileLastModifiedTime) /* file modified */
        {
            qwLogFileLastModifiedTime = qwNewLastModifiedTime; /* update time stamp */
            UINT32 dwNewSize = GetFileSize(g_hErrorLogFile, nullptr);
            if (dwNewSize > dwLogFileSize) /* just read the incremental part */
            {
                ReadFile(
                    g_hErrorLogFile,
                    lpBuffer + dwLogFileSize, /* append to buffer */
                    dwNewSize - dwLogFileSize, /* new content */
                    &dwBytesWritten,
                    nullptr
                );
                if (dwBytesWritten != 0)
                {
                    WriteConsoleA(
                        GetStdHandle(STD_OUTPUT_HANDLE),
                        lpBuffer + dwLogFileSize,
                        dwBytesWritten,
                        nullptr,
                        nullptr
                    );
                    dwLogFileSize += dwBytesWritten;
                }
            }
            else /* size not incremented, which means the file is truncated, so read the entire file */
            {
                SetFilePointer(
                    g_hErrorLogFile,
                    0,
                    0,
                    FILE_BEGIN
                );
                ReadFile(g_hErrorLogFile, lpBuffer, dwNewSize, &dwBytesWritten, nullptr);
                WriteConsoleA(
                    GetStdHandle(STD_OUTPUT_HANDLE),
                    lpBuffer,
                    dwBytesWritten,
                    nullptr,
                    nullptr
                );
                dwLogFileSize = dwBytesWritten;
            }
        }
        Sleep(1000); /* update every 1 s */
    }
    ExitThread(ERROR_SUCCESS);
}


/*
@brief start game by clicking the Start button
*/
DWORD CALLBACK start_game(LPVOID lpParam)
{   
    HANDLE hEvent = lpParam;
    while (WaitForSingleObject(g_hStartGameEvent, INFINITE) && !g_bExit)
    {
        give_command(CMD_START_GAME, sizeof(CMD_START_GAME) / sizeof(char));
        /* Basically, it takes less than 20 s to load */
        Sleep(30 * 1000); /* Wait 30 s for the round to start */
    }
    ExitThread(ERROR_SUCCESS);
}