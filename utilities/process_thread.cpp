#include <utilities/process_thread.hpp>
#include <utilities/common.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

namespace utilities {
std::expected<void, std::error_code> TraverseProcesses(std::function<bool (std::uintptr_t process_id, std::uintptr_t main_thread_id)> callback)
{
    ::HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    std::unique_ptr<std::remove_pointer_t<::HANDLE>, decltype(&::CloseHandle)> snapshot_guard(snapshot, &::CloseHandle);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return std::unexpected<std::error_code>(make_system_error(::GetLastError()));
    }
    
    ::PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);
    if (::Process32First(snapshot, &entry)) {
        do {
            callback(static_cast<std::uintptr_t>(entry.th32ProcessID), static_cast<std::uintptr_t>(entry.th32DefaultHeapID));
        } while (::Process32Next(snapshot, &entry));
    }

    return {};
}

std::expected<void, std::error_code> TerminateProcess(std::uintptr_t process_id, std::int32_t timeout_ms, bool force) {
    auto proc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD target_pid = static_cast<DWORD>(lParam);
        DWORD pid = 0;
        auto tid = ::GetWindowThreadProcessId(hWnd, &pid);
        (void)tid;
        if (pid == target_pid) {
            spdlog::debug("Sending WM_CLOSE to window of process {}", target_pid);
            ::PostMessage(hWnd, WM_CLOSE, 0, 0);
            return FALSE; // stop enumeration
        }
        return TRUE; // continue enumeration
    };
    ::EnumWindows(proc, static_cast<LPARAM>(process_id));
    auto hProcess = ::OpenProcess(
        PROCESS_TERMINATE | SYNCHRONIZE,
        false,
        static_cast<DWORD>(process_id)
    );
    if (!hProcess) {
        return std::unexpected<std::error_code>(make_system_error(::GetLastError()));
    }

    std::unique_ptr<std::remove_pointer_t<::HANDLE>, decltype(&::CloseHandle)> guard(hProcess, &::CloseHandle);

    auto wait_result = ::WaitForSingleObject(hProcess, static_cast<DWORD>(timeout_ms));
    if (wait_result == WAIT_OBJECT_0) {
        spdlog::debug("Process terminated gracefully");
        return {};
    }
    if (wait_result == WAIT_FAILED) {
        return std::unexpected<std::error_code>(make_system_error(::GetLastError()));
    }
    if (!force) {
        return std::unexpected<std::error_code>(make_error_code(std::errc::timed_out));
    }
    if (wait_result == WAIT_TIMEOUT) {
        if (::TerminateProcess(hProcess, 1) == 0) {
            // ::CloseHandle(hProcess); // handled by unique_ptr
            return std::unexpected<std::error_code>(make_system_error(::GetLastError()));
        } else {
            spdlog::debug("Process terminated forcefully");
            return {};
        }
    }
    assert("Unexpected wait result" && false); // This is impossible by documentation
    return {};
}

} // namespace utilities
