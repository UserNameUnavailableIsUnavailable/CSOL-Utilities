#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>

namespace
{
constexpr wchar_t AgentPath[] = LR"(C:\Program Files\LGHUB\lghub_agent.exe)";
constexpr char HookInitializerName[] = "InitializeLGHubHooks";

class Handle
{
  public:
    Handle() = default;
    explicit Handle(HANDLE value) : value_(value) {}
    ~Handle()
    {
        if (value_ != nullptr && value_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(value_);
        }
    }

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    [[nodiscard]] HANDLE get() const
    {
        return value_;
    }

    [[nodiscard]] explicit operator bool() const
    {
        return value_ != nullptr && value_ != INVALID_HANDLE_VALUE;
    }

  private:
    HANDLE value_ = nullptr;
};

std::wstring FormatError(DWORD error)
{
    wchar_t* buffer = nullptr;
    const DWORD length = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        0,
        reinterpret_cast<wchar_t*>(&buffer),
        0,
        nullptr
    );
    std::wstring message = length == 0 ? L"Unknown error" : std::wstring(buffer, length);
    LocalFree(buffer);
    return message;
}

bool WaitForThread(HANDLE thread, const wchar_t* operation, DWORD& exitCode)
{
    const DWORD waitResult = WaitForSingleObject(thread, INFINITE);
    if (waitResult != WAIT_OBJECT_0)
    {
        std::wcerr << operation << L" failed while waiting: " << FormatError(GetLastError()) << L'\n';
        return false;
    }
    if (!GetExitCodeThread(thread, &exitCode))
    {
        std::wcerr << operation << L" failed to obtain its result: " << FormatError(GetLastError()) << L'\n';
        return false;
    }
    return true;
}

std::filesystem::path CurrentExecutableDirectory()
{
    std::wstring path(MAX_PATH, L'\0');
    DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    while (length == path.size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        path.resize(path.size() * 2);
        length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    }
    if (length == 0)
    {
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "GetModuleFileNameW");
    }
    path.resize(length);
    return std::filesystem::path(path).parent_path();
}

uintptr_t FindRemoteModule(DWORD processId, const std::filesystem::path& modulePath)
{
    const Handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId));
    if (!snapshot)
    {
        return 0;
    }

    MODULEENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (!Module32FirstW(snapshot.get(), &entry))
    {
        return 0;
    }

    const std::wstring expectedName = modulePath.filename().wstring();
    do
    {
        if (_wcsicmp(entry.szModule, expectedName.c_str()) == 0)
        {
            return reinterpret_cast<uintptr_t>(entry.modBaseAddr);
        }
    } while (Module32NextW(snapshot.get(), &entry));

    return 0;
}

bool LoadHookDll(HANDLE process, const std::filesystem::path& dllPath)
{
    const std::wstring path = dllPath.wstring();
    const SIZE_T pathBytes = (path.size() + 1) * sizeof(wchar_t);
    void* remotePath = VirtualAllocEx(process, nullptr, pathBytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remotePath == nullptr)
    {
        std::wcerr << L"VirtualAllocEx failed: " << FormatError(GetLastError()) << L'\n';
        return false;
    }

    const auto freeRemotePath = [&]() { VirtualFreeEx(process, remotePath, 0, MEM_RELEASE); };
    if (!WriteProcessMemory(process, remotePath, path.c_str(), pathBytes, nullptr))
    {
        std::wcerr << L"WriteProcessMemory failed: " << FormatError(GetLastError()) << L'\n';
        freeRemotePath();
        return false;
    }

    const HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    const FARPROC loadLibrary = kernel32 == nullptr ? nullptr : GetProcAddress(kernel32, "LoadLibraryW");
    if (loadLibrary == nullptr)
    {
        std::wcerr << L"Unable to resolve LoadLibraryW: " << FormatError(GetLastError()) << L'\n';
        freeRemotePath();
        return false;
    }

    const Handle loadThread(CreateRemoteThread(
        process,
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibrary),
        remotePath,
        0,
        nullptr
    ));
    if (!loadThread)
    {
        std::wcerr << L"CreateRemoteThread(LoadLibraryW) failed: " << FormatError(GetLastError()) << L'\n';
        freeRemotePath();
        return false;
    }

    DWORD loadResult = 0;
    const bool loaded = WaitForThread(loadThread.get(), L"LoadLibraryW", loadResult) && loadResult != 0;
    freeRemotePath();
    if (!loaded)
    {
        if (loadResult == 0)
        {
            std::wcerr << L"LoadLibraryW failed in lghub_agent.exe.\n";
        }
        return false;
    }
    return true;
}

bool InitializeHook(HANDLE process, DWORD processId, const std::filesystem::path& dllPath)
{
    const HMODULE localModule = LoadLibraryExW(
        dllPath.c_str(),
        nullptr,
        DONT_RESOLVE_DLL_REFERENCES
    );
    if (localModule == nullptr)
    {
        std::wcerr << L"Unable to inspect lgextender.dll: " << FormatError(GetLastError()) << L'\n';
        return false;
    }

    const FARPROC localInitializer = GetProcAddress(localModule, HookInitializerName);
    if (localInitializer == nullptr)
    {
        std::wcerr << L"lgextender.dll does not export " << HookInitializerName << L".\n";
        FreeLibrary(localModule);
        return false;
    }

    const uintptr_t initializerOffset =
        reinterpret_cast<uintptr_t>(localInitializer) - reinterpret_cast<uintptr_t>(localModule);
    FreeLibrary(localModule);

    const uintptr_t remoteModule = FindRemoteModule(processId, dllPath);
    if (remoteModule == 0)
    {
        std::wcerr << L"Unable to locate lgextender.dll in lghub_agent.exe.\n";
        return false;
    }

    const Handle hookThread(CreateRemoteThread(
        process,
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteModule + initializerOffset),
        nullptr,
        0,
        nullptr
    ));
    if (!hookThread)
    {
        std::wcerr << L"CreateRemoteThread(" << HookInitializerName
                   << L") failed: " << FormatError(GetLastError()) << L'\n';
        return false;
    }

    DWORD hookResult = ERROR_GEN_FAILURE;
    if (!WaitForThread(hookThread.get(), L"Hook initialization", hookResult))
    {
        return false;
    }
    if (hookResult != ERROR_SUCCESS)
    {
        std::wcerr << L"Hook initialization failed with error " << hookResult << L".\n";
        return false;
    }
    return true;
}
}

int wmain(int argc, wchar_t* argv[])
{
    try
    {
        const std::filesystem::path dllPath =
            std::filesystem::absolute(argc > 1 ? argv[1] : CurrentExecutableDirectory() / L"lgextender.dll");
        if (!std::filesystem::is_regular_file(dllPath))
        {
            std::wcerr << L"Hook DLL not found: " << dllPath << L'\n';
            return 1;
        }

        STARTUPINFOW startupInfo{};
        startupInfo.cb = sizeof(startupInfo);
        PROCESS_INFORMATION processInfo{};
        if (!CreateProcessW(
                AgentPath,
                nullptr,
                nullptr,
                nullptr,
                FALSE,
                CREATE_SUSPENDED,
                nullptr,
                nullptr,
                &startupInfo,
                &processInfo
            ))
        {
            std::wcerr << L"CreateProcessW failed: " << FormatError(GetLastError()) << L'\n';
            return 1;
        }

        const Handle process(processInfo.hProcess);
        const Handle primaryThread(processInfo.hThread);
        if (!LoadHookDll(process.get(), dllPath) ||
            !InitializeHook(process.get(), processInfo.dwProcessId, dllPath))
        {
            TerminateProcess(process.get(), ERROR_DLL_INIT_FAILED);
            WaitForSingleObject(process.get(), INFINITE);
            return 1;
        }

        if (ResumeThread(primaryThread.get()) == static_cast<DWORD>(-1))
        {
            std::wcerr << L"ResumeThread failed: " << FormatError(GetLastError()) << L'\n';
            TerminateProcess(process.get(), GetLastError());
            WaitForSingleObject(process.get(), INFINITE);
            return 1;
        }

        std::wcout << L"Hook installed; lghub_agent.exe resumed (PID " << processInfo.dwProcessId << L").\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
