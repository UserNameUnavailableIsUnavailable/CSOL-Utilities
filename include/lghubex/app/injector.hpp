#pragma once

#include <expected>
#include <string>
#include <system_error>

namespace lghubex {
class Injector {
public:
    Injector();
    void Inject();
    ~Injector() noexcept;
private:
    std::expected<void, std::error_code> TerminateExisting();
    std::wstring process_name_;
    std::wstring image_path_;
    std::wstring executable_root_;
    std::wstring inject_dll_;
};
} // namespace lghubex
