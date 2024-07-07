#define UNICODE
#include <Windows.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <filesystem>
#include <fstream>

enum REG_PREDEFINED_KEY_ENUM
{
    REG_CLASSES_ROOT,
    REG_CURRENT_CONFIG,
    REG_CURRENT_USER,
    REG_LOCAL_MACHINE,
    REG_USERS
};

enum ENUM_CSOL_GAME_COMMAND
{
    CSOL_NOP, // 此时无需任何操作
    CSOL_QUERY_CURSOR_LOCATION, // 获取鼠标光标位置，需要打开罗技控制台查看
    CSOL_ROUND_WAIT, // 在房间等待
    CSOL_ROUND_LOAD, // 游戏正在加载
    CSOL_ROUND_CHOOSE_CLASS, // 选择
    CSOL_ROUND_PLAY, // 游戏正在进行
    CSOL_ROUND_CONFIRM, // 游戏结算（若成功结算，则转入 CSOL_ROUND_WAIT）
    CSOL_ROUND_TIMEOUT, // 超时机制
    CSOL_CRAFT_PARTS_COMBINE, // 制造所合成配件
    CSOL_STORE_PURCHASE, // 商店购买物品
};

ENUM_CSOL_GAME_COMMAND g_CSOL_GAME_COMMAND = CSOL_NOP;

std::shared_ptr<wchar_t[]> ConvertUtf8ToUtf16(const void* byte_string)
{
    
    auto length = MultiByteToWideChar(
        CP_UTF8,
        0,
        (char*)(byte_string),
        -1,
        nullptr,
        0
    );
    std::shared_ptr<wchar_t[]> ret(new wchar_t[length]);
    MultiByteToWideChar(
        CP_UTF8,
        0,
        (char*)byte_string,
        -1,
        ret.get(),
        length
    );
    return ret;
}

std::shared_ptr<char[]> ConvertUtf16ToUtf8(const void* byte_string)
{
    auto length = WideCharToMultiByte(
        CP_UTF8,
        0,
        (wchar_t*)byte_string,
        -1,
        nullptr,
        0,
        nullptr,
        nullptr
    );
    std::shared_ptr<char[]> ret(new char[length]);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        (wchar_t*)byte_string,
        -1,
        ret.get(),
        length,
        nullptr,
        nullptr
    );
    return ret;
}

/*
* CSOL_QueryInstallationPath 获取 CSOL 安装路径
* @param lpPredefinedTopItem 注册表预定义的项
* @param lpSubItem 注册表子项
* @return CSOL 安装路径
* @note CSOL 安装路径保存在注册表项 HKEY_CURRENT_USER\Software\TCGame\csol\gamepath 中，返回的字符串在堆中
*/

std::shared_ptr<char[]> CSOL_QueryInstallationPath(REG_PREDEFINED_KEY_ENUM predefinedTopDir, LPCTSTR lpSubDir, LPCTSTR lpItemName)
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
    std::shared_ptr<char[]> errorLogContent(new char[dwBufferSize]{0});
    RegGetValueW(
        hPredefinedTopDir,
        lpSubDir,
        lpItemName,
        RRF_RT_REG_SZ,
        nullptr,
        errorLogContent.get(),
        &dwBufferSize
    ); // 读取字符串
    return ConvertUtf16ToUtf8(errorLogContent.get());
}

ENUM_CSOL_GAME_COMMAND CSOL_TrasferState(std::shared_ptr<char []> CSOL_ErrorLog, DWORD dwLength)
{
    for (DWORD dwIndex = dwLength - 1; dwIndex != 0xFFFFFFFF; dwIndex--) // 注意是无符号数，要用补码判断是否为负数
    {
        /* 从最后一条消息开始向前解析，获取游戏最新状态 */
        if (CSOL_ErrorLog[dwIndex] == '\r' && CSOL_ErrorLog[dwIndex + 1] == '\n')
        {
            CSOL_ErrorLog[dwIndex] = '\0'; CSOL_ErrorLog[dwIndex + 1] = '\0';
            if (std::regex_match(CSOL_ErrorLog.get() + dwIndex + 2, std::regex("[0-9]{2}:[0-9]{2}:[0-9]{2}\\.[0-9]{3}.Result Confirm")))
            {
                g_CSOL_GAME_COMMAND = ENUM_CSOL_GAME_COMMAND::CSOL_ROUND_WAIT; // 等待下回合开始
            }
            else if (std::regex_match(CSOL_ErrorLog.get(), std::regex("[0-9]{2}:[0-9]{2}:[0-9]{2}\\.[0-9]{3}.*Join HostServer")))
            {
                g_CSOL_GAME_COMMAND = ENUM_CSOL_GAME_COMMAND::CSOL_ROUND_LOAD; // 游戏正在加载
            }
        }
    }

}

int main()
{
    auto gameInstallationPath = CSOL_QueryInstallationPath(REG_PREDEFINED_KEY_ENUM::REG_CURRENT_USER, L"Software\\TCGame\\csol", L"gamepath");
    if (!std::regex_match(gameInstallationPath.get(), std::regex(".*\\\\$"))) // 尾字符非反斜杠
    {
        strcat(gameInstallationPath.get(), "\\");
    }
    std::string binDirectory = std::string(gameInstallationPath.get()) + "bin\\";
    std::filesystem::path errorLogFilePath = binDirectory + "Error.log";
    std::ifstream ifs(errorLogFilePath, std::ios::ate);
    if (!ifs.is_open())
    {
        return 1;
    }
    std::shared_ptr<char []> errorLogContent(new char[1 << 20]{0}); // 1 MiB 空间容纳日志文件
    ifs.read(errorLogContent.get(), (1 << 20) - 1);
    auto command = CSOL_TrasferState(errorLogContent, ifs.gcount());
    // errorLogFilePath = std::regex_replace(errorLogFilePath, std::regex("\\\\"), "/");
    // auto hErrorLogFile = CreateFileW(
    //     ConvertUtf8ToUtf16(errorLogFilePath.c_str()).get(),
    //     GENERIC_READ,
    //     FILE_SHARE_READ | FILE_SHARE_WRITE,
    //     nullptr,
    //     OPEN_EXISTING,
    //     FILE_ATTRIBUTE_NORMAL,
    //     nullptr
    // );
    // DWORD dwNumberOfBytesRead;
    // ReadFile(
    //     hErrorLogFile,
    //     ifs.get(),
    //     1 << 20,
    //     &dwNumberOfBytesRead,
    //     nullptr
    // );
    // ifs[dwNumberOfBytesRead - 1] = '\0';
    // CloseHandle(hErrorLogFile);
}

    