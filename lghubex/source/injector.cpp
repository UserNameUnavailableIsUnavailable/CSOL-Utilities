#include <lghubex/injector.hpp>
#include <utilities/common.hpp>
#include <utilities/string.hpp>
#include <utilities/process_thread.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace lghubex {
namespace detail {
    
} // namespace detail

static const wchar_t kProcessName[] = L"lghub_agent.exe";
static const wchar_t kImagePath[] = L"C:\\Program Files\\LGHUB\\lghub_agent.exe";
static const wchar_t kInjectDll[] = L"lghubex.dll";

Injector::Injector() :
    process_name_(kProcessName),
    image_path_(kImagePath),
    inject_dll_(kInjectDll)
{
    std::filesystem::path p(kImagePath);
    if (!std::filesystem::is_regular_file(p)) {
        throw std::runtime_error(
            std::format("`{}` does not exist or is not a regular file.", utilities::Utf16ToUtf8(kImagePath))
        );
    }
    auto res = utilities::GetExecutableRoot();
    if (!res) {
        throw std::runtime_error(
            std::format("Failed to get executable root: {}.", res.error().message())
        );
    } else {
        executable_root_ = *res;
    }
    if (!std::filesystem::is_regular_file(executable_root_ + L"\\" + inject_dll_)) {
        throw std::runtime_error(
            std::format("`{}` does not exist or is not a regular file.", utilities::Utf16ToUtf8(executable_root_ + L"\\" + inject_dll_))
        );
    }
}

Injector::~Injector() noexcept = default;

void Injector::Inject() {
    if (auto res = TerminateExisting(); !res) {
        spdlog::error("Failed to terminate existing process: {}.", res.error().message());
        return;
    }
    auto path = image_path_;
    PROCESS_INFORMATION pi{};
    auto si = STARTUPINFOW{
        .cb = sizeof(STARTUPINFOW)
    };
    auto ok = CreateProcessW(
        path.data(),
        nullptr,
        nullptr,
        nullptr,
        false,
        CREATE_SUSPENDED,
        nullptr,
        nullptr,
        &si,
        &pi
    );
    if (!ok) {
        spdlog::error("Failed to create process.");
    } else {
        spdlog::info("Successfully created process in suspended state.");
    }
    auto hProcess = pi.hProcess;
    auto hThread = pi.hThread;
    utilities::Defer d1([&] {
        ResumeThread(hThread);
        spdlog::info("Resuming thread");
        ::CloseHandle(hThread);
        ::CloseHandle(hProcess);
    });
    std::wstring kernel32 = L"kernel32.dll";
    auto kernel32_handle = ::GetModuleHandleW(kernel32.c_str());
    if (!kernel32_handle) {
        spdlog::error("Failed to get handle for `kernel32.dll`.");
        return;
    }
    auto load_library_addr = ::GetProcAddress(kernel32_handle, "LoadLibraryW");
    if (!load_library_addr) {
        spdlog::error("Failed to get address of `LoadLibraryW`.");
        return;
    }
    std::wstring dll_path = executable_root_ + L"\\" + inject_dll_;
    if (!std::filesystem::is_regular_file(dll_path)) {
        spdlog::error("`{}` does not exist or is not a regular file.", utilities::Utf16ToUtf8(dll_path));
        return;
    }
    auto remote_mem = ::VirtualAllocEx(
        hProcess,
        nullptr,
        dll_path.size(),
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    if (!remote_mem) {
        spdlog::error("Failed to allocate memory in remote process.");
        return;
    }
    utilities::Defer d2([&] {
        ::VirtualFreeEx(hProcess, remote_mem, 0, MEM_RELEASE);
    });
    if (!::WriteProcessMemory(
        hProcess,
        remote_mem,
        dll_path.data(),
        dll_path.size() * sizeof(wchar_t),
        nullptr
    )) {
        spdlog::error("Failed to write to remote process memory.");
        return;
    }
    auto h = CreateRemoteThread(
        hProcess,
        nullptr,
        4096,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(load_library_addr),
        remote_mem,
        0,
        nullptr
    );
    if (!h) {
        spdlog::error("Failed to create remote thread.");
        return;
    }
    spdlog::info("Created remote thread, waiting for it to finish.");
    auto wait_result = ::WaitForSingleObjectEx(h, INFINITE, false);
    if (wait_result == WAIT_OBJECT_0) {
        spdlog::info("Remote thread exited.");
    } else {
        auto e = utilities::make_system_error(::GetLastError());
        spdlog::error("Error occurred while waiting for remote thread to finish: {}.", e.message());
    }
    ::CloseHandle(h);
}

std::expected<void, std::error_code> Injector::TerminateExisting() {
    DWORD dwPathCapacity = static_cast<DWORD>(image_path_.capacity());
    auto exe_path = std::make_unique<wchar_t[]>(dwPathCapacity);

    std::unique_ptr<std::remove_pointer_t<::HANDLE>, decltype(&::CloseHandle)> process_handle(nullptr, &::CloseHandle);

    auto match_process = [&] (std::uintptr_t process_id, std::uintptr_t main_thread_id) -> bool {
        (void)main_thread_id;
        decltype(process_handle) guard(nullptr, &::CloseHandle);
        auto h = ::OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            false,
            static_cast<DWORD>(process_id)
        );
        if (!h) {
            return true; // skip this process if we can't open it
        }

        guard.reset(h);

        BOOL ok = TRUE;
            
        // On input, specifies the size of the lpExeName buffer, in characters. On success, receives the number of characters written to the buffer, not including the null-terminating character.
        DWORD dwLength = dwPathCapacity;
        ok = ::QueryFullProcessImageNameW(
            h,
            0,
            exe_path.get(),
            &dwLength
        );
        if (!ok) {
            return true; // skip this process if we can't query its full image name
        }
        if (dwLength != image_path_.size()) {
            return true; // skip if longer than the target's path
        } else {
            // Windows is case insensitive
            for (std::size_t i = 0; i < image_path_.size(); ++i) {
                if (std::towlower(exe_path[i]) != std::towlower(image_path_[i])) {
                    return true; // skip if any character doesn't match
                }
            }
            spdlog::info("Found running process matching image path: `{}`.", utilities::Utf16ToUtf8(image_path_));
            std::swap(process_handle, guard);
            return false;
        }
    };
    if (auto res = utilities::TraverseProcesses(match_process); !res) {
        spdlog::error("Failed to traverse processes: {}.", res.error().message());
        return std::unexpected(res.error());
    }
    if (!process_handle) {
        return {}; // no matching process found, nothing to terminate
    }
    auto pid = GetProcessId(process_handle.get());
    return utilities::TerminateProcess(pid, 5000, true);
}
} // namespace lghubex
