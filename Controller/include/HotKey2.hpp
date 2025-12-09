namespace CSOL_Utilities
{
struct Modifiers
{
  bool win = false;
  bool alt = false;
  bool ctrl = false;
  bool shift = false;
  bool no_repeat = true;
};

// 支持 RAII 的热键类
class HotKey2
{
friend HotKey2 CreateHotKey(const Modifiers& modifiers, uint32_t vk, std::uintptr_t hWnd);
public:
/// 获取热键 id
///\return 返回热键 id
  std::uint32_t GetId() const noexcept
  {
      return id_;
  }
private:
  HotKey2(std::uint32_t id, std::string description, std::function<void()> deleter) noexcept :
      id_(id),
      description(std::move(description)),
      deleter(std::move(deleter))
  {
  }
  HotKey2(const HotKey2&) = delete;
  HotKey2(HotKey2&&) = default;
  ~HotKey2() noexcept
  {
      deleter();
  }
private:
  const uint32_t id_ = 0; // 热键 id
  const std::string description; // 热键描述
  const std::function<void()> deleter; // 注销热键的函数（不可抛出异常）
};

/// 创建热键。
///\param modifiers 修饰键
///\param vk 虚拟键码
///\param window_handle 关联窗口句柄
///\return 返回创建的 HotKey2 对象
HotKey2 CreateHotKey(const Modifiers& modifiers, std::uint32_t vk, std::uintptr_t window_handle);
}