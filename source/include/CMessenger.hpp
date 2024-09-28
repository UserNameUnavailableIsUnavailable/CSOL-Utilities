#pragma once

#include "CSOL_Utilities.hpp"
#include <mutex>
#include <filesystem>
#include <fstream>

namespace CSOL_Utilities
{
    class CMessenger
    {
    public:
        explicit CMessenger(std::filesystem::path file);
        CMessenger(const CMessenger&) = delete;
        CMessenger& operator=(const CMessenger&) = delete;
        CMessenger(const CMessenger&&) = delete;
        CMessenger& operator=(const CMessenger&&) = delete;
        ~CMessenger() noexcept;
        void AssignAndDispatch(COMMAND cmd, std::time_t cmd_time) noexcept;
    private:
        static constexpr const char* QueryCommandString(COMMAND cmd) noexcept;
        std::filesystem::path m_CommandFile;
        std::ofstream m_FileStream;
        const char* m_CommandString;
        std::time_t m_CommandTime{ 0 };
        std::mutex m_Mutex;
        char* m_Buffer;
    };
};