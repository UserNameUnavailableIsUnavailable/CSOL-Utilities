#pragma once

namespace controller {
class Injector {
public:
    Injector();
    void Inject();
    ~Injector() noexcept;
private:
    std::expected<void, std::error_code> TerminateExisting();
    std::wstring process_name_;
    std::wstring image_path_;
};
} // namespace Controller
