#pragma once

#include "CSOL_Utilities.hpp"
#include <filesystem>
#include <fstream>
#include <mutex>
#include "ExecutorCommand.hpp"

namespace CSOL_Utilities
{
class CMessenger
{
  public:
    explicit CMessenger(std::filesystem::path file);
    CMessenger(const CMessenger &) = delete;
    CMessenger &operator=(const CMessenger &) = delete;
    CMessenger(const CMessenger &&) = delete;
    CMessenger &operator=(const CMessenger &&) = delete;
    ~CMessenger() noexcept;
    void Dispatch(const ExecutorCommand& ec) noexcept;
    void DispatchNOP() noexcept;
  private:
    static constexpr const char *QueryCommandString(EXECUTOR_COMMAND cmd) noexcept;
    std::filesystem::path m_CommandFile;
    std::ofstream m_FileStream;
    const char *m_CommandString;
    std::time_t m_CommandTimepoint{0};
    std::mutex m_Mutex;
    char *m_Buffer;
};
}; // namespace CSOL_Utilities