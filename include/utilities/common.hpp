namespace utilities {
template <typename E>
requires (sizeof(E) == sizeof(int))
constexpr static inline std::error_code make_system_error(E err) noexcept
{
    return std::error_code(*reinterpret_cast<int*>(&err), std::system_category());
}
} // namespace utilities
