#include <Windows.h>
#include <memory>
#include <winerror.h>
#include "game.hpp"
#include "util.hpp"
#include "csol24except.hpp"

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