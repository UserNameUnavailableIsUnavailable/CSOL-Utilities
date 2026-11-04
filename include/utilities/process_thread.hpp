#pragma once

namespace utilities {
std::expected<void, std::error_code> TraverseProcesses(std::function<bool (std::uintptr_t process_id, std::uintptr_t main_thread_id)> callback);
std::expected<void, std::error_code> TerminateProcess(std::uintptr_t pid, std::int32_t timeout_ms, bool force);
std::expected<std::wstring, std::error_code> GetExecutableRoot();
} // namespace utilities
