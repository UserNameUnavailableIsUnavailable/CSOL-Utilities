#include "CException.hpp"
#include "CSOL_Utilities.hpp"
#include <Windows.h>
#include <errhandlingapi.h>
#include <memory>
#include <winerror.h>

using namespace CSOL_Utilities;

std::shared_ptr<wchar_t[]> CSOL_Utilities::ConvertUtf8ToUtf16(const char *byte_string)
{
    auto length = MultiByteToWideChar(CP_UTF8, 0, (char *)(byte_string), -1, nullptr, 0);
    std::shared_ptr<wchar_t[]> ret(new wchar_t[length]);
    MultiByteToWideChar(CP_UTF8, 0, (char *)byte_string, -1, ret.get(), length);
    return ret;
}

std::shared_ptr<char[]> CSOL_Utilities::ConvertUtf16ToUtf8(const wchar_t *byte_string)
{
    auto length = WideCharToMultiByte(CP_UTF8, 0, (wchar_t *)byte_string, -1, nullptr, 0, nullptr, nullptr);
    std::shared_ptr<char[]> ret(new char[length]);
    WideCharToMultiByte(CP_UTF8, 0, (wchar_t *)byte_string, -1, ret.get(), length, nullptr, nullptr);
    return ret;
}

/*
 * @brief 获取字符串型注册表项。
 * @param `lpPredefinedTopItem` 注册表预定义的项，如 `HKEY_CURRENT_USER`。
 * @param `lpSubItem` 注册表子目录项。
 * @param `lpItemName` 目录中的项名。
 * @return 项的值。
 * @note 例如，CSOL 安装路径保存在注册表项 HKEY_CURRENT_USER\Software\TCGame\csol\gamepath 中，则 `hPredefinedTopDir =
 * HKEY_CURRENT_USER`，`lpSubDir = Software\TCGame`，`lpItemName = gamepath`。
 */
std::shared_ptr<wchar_t[]> CSOL_Utilities::QueryRegistryStringItem(HKEY hPredefinedTopDir, LPCWSTR lpSubDir,
                                                                   LPCWSTR lpItemName)
{
    DWORD cbRequiredBufferSize;
    const char *lpszPredefinedTopDirName = "";
    if (hPredefinedTopDir == HKEY_CLASSES_ROOT)
    {
        lpszPredefinedTopDirName = "HKEY_CLASSES_ROOT";
    }
    else if (hPredefinedTopDir == HKEY_CURRENT_CONFIG)
    {
        lpszPredefinedTopDirName = "HKEU_CURRENT_CONFIG";
    }
    else if (hPredefinedTopDir == HKEY_CURRENT_USER)
    {
        lpszPredefinedTopDirName = "HKEY_CURRENT_USER";
    }
    else if (hPredefinedTopDir == HKEY_LOCAL_MACHINE)
    {
        lpszPredefinedTopDirName = "HKEY_LOCAL_MACHINE";
    }
    else if (hPredefinedTopDir == HKEY_USERS)
    {
        lpszPredefinedTopDirName = "HKEY_USERS";
    }
    else
    {
        lpszPredefinedTopDirName = "UNKNOWN";
    }
    LSTATUS ret{ERROR_SUCCESS};
    ret = RegGetValueW(hPredefinedTopDir, lpSubDir, lpItemName, RRF_RT_REG_SZ, nullptr, nullptr,
                       &cbRequiredBufferSize); // 获取需要的缓冲区长度
    if (ret != ERROR_SUCCESS)
    {
        throw CException("查找注册表项失败。错误代码：%ld。", ret);
    }
    std::shared_ptr<wchar_t[]> lpwszResult(new wchar_t[(cbRequiredBufferSize - 1) / 2 + 1]{0});
    ret = RegGetValueW(hPredefinedTopDir, lpSubDir, lpItemName, RRF_RT_REG_SZ, nullptr, lpwszResult.get(),
                       &cbRequiredBufferSize); // 读取字符串
    if (ret != ERROR_SUCCESS)
    {
        throw CException("查找注册表项失败。错误代码：%ld。", ret);
    }
    return lpwszResult;
}

BOOL CSOL_Utilities::IsRunningAsAdmin() noexcept
{
    BOOL bRet = FALSE;
    PSID admin_group = NULL;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0,
                                 0, &admin_group))
    {
        CheckTokenMembership(NULL, admin_group, &bRet);
        FreeSid(admin_group);
    }
    return bRet;
}