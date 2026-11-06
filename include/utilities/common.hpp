namespace utilities {
template <typename E>
requires (sizeof(E) == sizeof(int))
constexpr static inline std::error_code make_system_error(E err) noexcept
{
    return std::error_code(*reinterpret_cast<int*>(&err), std::system_category());
}

namespace detail {
template <typename F>
requires std::is_invocable_v<F>
class DeferCall {
public:
    explicit DeferCall(F&& f) noexcept : f_(std::move(f)) {}
    ~DeferCall() noexcept {
        f_();
    }

private:
    F f_;
};
} // namespace detail

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define DEFER(f) auto CONCAT(defer_1145141919810_, __COUNTER__) = utilities::MakeDefer(f)

template <typename F>
requires std::is_invocable_v<F>
detail::DeferCall<F> MakeDefer(F&& f) noexcept {
    return detail::DeferCall<F>(std::forward<F>(f));
}
} // namespace utilities
