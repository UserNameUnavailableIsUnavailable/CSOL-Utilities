#pragma once

#include "Module.hpp"
#include "Command.hpp"
#include "OCR/OcrLiteCApi.h"

namespace CSOL_Utilities
{
    class IdlerEngine : public Module
    {
    public:
        virtual void work() override;
        void dispatch();
        void analyze();
        void toggle_extended_mode(bool flag) { m_extended.store(flag, std::memory_order::release); }
    protected:
        IN_GAME_STATE m_igs;
        std::atomic_bool m_extended = false;
        std::chrono::system_clock::time_point m_tp;
        HMODULE m_hGamingTool;
        HWND m_hGameWindow;
        std::shared_ptr<Command> m_cmd;
        std::vector<char> m_image_buffer;
        OCR_HANDLE m_hOcr;
    };
}

