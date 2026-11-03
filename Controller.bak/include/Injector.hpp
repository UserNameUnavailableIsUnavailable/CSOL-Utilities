#pragma once

#include <filesystem>

namespace CSOL_Utilities {
class Injector {
public:
    Injector();
    void Inject();
    ~Injector() noexcept;
private:
    void TerminateExisting();
    std::filesystem::path path_;
};
} // namespace CSOL_Utilities