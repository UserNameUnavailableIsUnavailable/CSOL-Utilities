#pragma once

#include "CSOL_Utilities.hpp"
#include <mutex>
#include <filesystem>
#include <fstream>

class CSOL_Utilities::CMessenger
{
public:
    explicit CMessenger(std::filesystem::path file);
    CMessenger(const CMessenger&) = delete;
    CMessenger& operator=(const CMessenger&) = delete;
    CMessenger(const CMessenger&&);
    CMessenger& operator=(const CMessenger&&);
    ~CMessenger() noexcept;
    CMessenger& assign(CSOL_Utilities::COMMAND cmd) noexcept;
    void dispatch() noexcept;
private:
    static constexpr const char* QueryCommandString(CSOL_Utilities::COMMAND cmd) noexcept;
    std::filesystem::path m_CommandFile;
    std::ofstream m_FileStream;
    const char* m_CommandString;
    std::mutex m_Mutex;
    char* m_Buffer;
};