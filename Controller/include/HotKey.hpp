#pragma once

namespace CSOL_Utilities
{
    class HotKey
    {
    public:
        HotKey(const HotKey&) = delete;
        HotKey(HotKey&&) = default;
        int Id() const noexcept { return m_Id; }
        std::string Describe() const;
        HotKey(uint32_t modifiers, uint32_t vk, HWND hWnd = nullptr, bool defer_registration = true);
		void Register();
        ~HotKey() noexcept;
    private:
        static std::atomic_int s_IdPool;
        const int m_Id;
        UINT m_Modifiers;
        UINT m_Vk;
        HWND m_AssociatedWindow;
        bool m_Success = false;
    };
}
