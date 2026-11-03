#include <controller/injector.hpp>
#include <utilities/string.hpp>
#include <utilities/process_thread.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace controller {
namespace detail {
    
} // namespace detail

static const wchar_t kProcessName[] = L"lghub_agent.exe";
static const wchar_t kImagePath[] = L"C:\\Program Files\\LGHUB\\lghub_agent.exe";

Injector::Injector() :
    process_name_(kProcessName),
    image_path_(kImagePath)
{
    std::filesystem::path p(kImagePath);
    if (!std::filesystem::is_regular_file(p)) {
        spdlog::error("`{}` does not exist or is not a regular file", utilities::Utf16ToUtf8(kImagePath));
    } else {
        spdlog::info("Logitech G Hub Agent executable found at: `{}`", utilities::Utf16ToUtf8(kImagePath));
    }
}

Injector::~Injector() noexcept = default;

void Injector::Inject() {
    if (auto res = TerminateExisting(); !res) {
        spdlog::error("Failed to terminate existing process: {}", res.error().message());
        return;
    }
    auto path = image_path_;
    PROCESS_INFORMATION pi{};
    auto si = STARTUPINFOW{
        .cb = sizeof(STARTUPINFOW)
    };
    auto h = CreateProcessW(
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
    if (!h) {
        spdlog::error("Failed to create process");
    } else {
        spdlog::info("Successfully created process in suspended state");
    }
    // TODO: Inject here
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
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
            spdlog::info("Found running process matching image path: {}", utilities::Utf16ToUtf8(image_path_));
            std::swap(process_handle, guard);
            return false;
        }
    };
    if (auto res = utilities::TraverseProcesses(match_process); !res) {
        spdlog::error("Failed to traverse processes: {}", res.error().message());
        return std::unexpected(res.error());
    }
    if (!process_handle) {
        return {}; // no matching process found, nothing to terminate
    }
    auto pid = GetProcessId(process_handle.get());
    return utilities::TerminateProcess(pid, 5000, true);
}
} // namespace controller
