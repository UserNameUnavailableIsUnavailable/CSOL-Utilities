#include "pch.hpp"

#include "HotKey.hpp"
#include "Console.hpp"
#include "Exception.hpp"
#include "Utilities.hpp"

namespace CSOL_Utilities
{
    std::atomic_int HotKey::s_IdPool{ 0 };

    HotKey::HotKey(uint32_t modifiers, uint32_t vk, HWND hWnd, bool defer_registration) :
        m_Id(s_IdPool++),
        m_Modifiers(modifiers | MOD_NOREPEAT),
        m_AssociatedWindow(hWnd),
        m_Vk(vk)
    {
		if (!defer_registration)
		{
			Register();
		}
    }

    void HotKey::Register()
    {
		if (m_Success)
		{
			return;
		}
        auto ok = RegisterHotKey(m_AssociatedWindow, m_Id, m_Modifiers, m_Vk);
        if (!ok)
        {
            throw Exception(Translate("HotKey::ERROR_RegisterHotKey@2", Describe(), GetLastError()));
        }
        m_Success = ok;
		Console::Info(Translate("HotKey::INFO_RegisterHotKey@1", Describe()));
    }

    HotKey::~HotKey() noexcept
    {
        if (m_Success)
        {
            UnregisterHotKey(m_AssociatedWindow, m_Id);
        }
    }

    std::string HotKey::Describe() const
    {
        std::stringstream s;
        if (m_Modifiers & MOD_ALT)
        {
            s << "Alt ";            
        }
        if (m_Modifiers & MOD_CONTROL)
        {
            s << "Ctrl ";
        }
        if (m_Modifiers & MOD_SHIFT)
        {
            s << "Shift ";
        }
        if (m_Modifiers & MOD_WIN)
        {
            s << "🪟 ";
        }
        if (0x30 <= m_Vk && m_Vk <= 0x39)
        {
            s << static_cast<char>(m_Vk);
        }
        else if (0x41 <= m_Vk && m_Vk <= 0x5A)
        {
            s << static_cast<char>(m_Vk);
        }
        else
        {
            s << "VK(0x" << std::hex << std::uppercase << m_Vk << ')';
        }
        return s.str();
    }
}
