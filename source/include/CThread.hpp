#include <Windows.h>
#include <functional>
#include <memory>
#include <processthreadsapi.h>
#include <tuple>
#include <type_traits>

namespace CSOL_Utilities
{
class CThread
{
public:
    template <typename F, typename ...VA>
    CThread(F&& f, VA&&... va);
    ~CThread();
private:
    HANDLE m_hThread;
    DWORD m_dwThreadId;
};

/* 线程 */
template <typename Tuple, std::size_t... Indice>
static DWORD WINAPI Invoke(LPVOID lpParam) noexcept
{
    const std::unique_ptr<Tuple> param_list(static_cast<Tuple*>(lpParam));
    Tuple& param = *param_list;
    std::invoke(std::move(std::get<Indice>(param))...);
    return 0;
}

template <typename F, typename ...VA>
CThread::CThread(F&& f, VA&&... va)
{
    using Callable = std::tuple<std::decay<F>, std::decay<VA>...>;
    auto pCallable = std::make_unique<Callable>(std::forward<F>(f), std::forward<VA>(va)...);
    m_hThread = CreateThread(nullptr, 0, f, pCallable.get(), 0, &m_dwThreadId);
}

}