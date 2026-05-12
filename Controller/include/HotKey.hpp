#pragma once

namespace CSOL_Utilities
{

class HotKey
{
  public:
    HotKey(const HotKey &) = delete;
    HotKey(HotKey &&) = default;
    std::uint32_t Id() const noexcept
    {
        return id_;
    }
    std::string Describe() const;
    HotKey(uint32_t modifiers, uint32_t vk, std::uintptr_t window_handle = 0, bool defer_registration = true);
    void Register();
    ~HotKey() noexcept;

  private:
    static std::atomic_uint32_t s_id_pool;
    const std::uint32_t id_;
    std::uint32_t m_Modifiers;
    std::int32_t m_Vk;
    std::uintptr_t associate_window_;
    bool m_Success = false;
};
} // namespace CSOL_Utilities
