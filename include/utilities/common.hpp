namespace utilities {
template <typename E>
requires (sizeof(E) == sizeof(int))
constexpr static inline std::error_code make_system_error(E err) noexcept
{
    return std::error_code(*reinterpret_cast<int*>(&err), std::system_category());
}

class Defer {
public:
    template <typename F>
    requires std::is_invocable_v<F>
    explicit Defer(F&& f) noexcept : func_(std::move(f)) {}
    ~Defer() noexcept {
        if (func_) {
            func_();
        }
    }

private:
    std::function<void()> func_;
};
} // namespace utilities
