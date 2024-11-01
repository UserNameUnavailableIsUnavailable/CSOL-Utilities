#include "CException.hpp"
#include <Windows.h>
#include <cstddef>
#include <errhandlingapi.h>
#include <functional>
#include <rpcdce.h>
#include <unknwnbase.h>
#include <memory>
#include <type_traits>
#include <utility>
#include <cstdint>

namespace CSOL_Utilities
{
/* control messages for communication among control units */
using CU_T = enum class CONTROL_UNIT_TYPE
{
    CU_CREATE = WM_APP,
    CU_MASTER_CONTROLLER,
    CU_STATE_ANALYZER,
    CU_GAME_SUPERVISOR,
    CU_COMMAND_DISPATCHER,
    CU_MODE_SWITCHER,
    CU_EXECUTOR_DEBUGGER,
};

class CControlUnit
{
public:
    template <typename Callable, typename... VarArg>
    CControlUnit(CONTROL_UNIT_TYPE type, Callable&& c, VarArg&&... va);
    ~CControlUnit() noexcept
    {
        CloseHandle(m_hThread);
    }
    template <typename ContentType>
    void Notify(CONTROL_UNIT_TYPE source, CONTROL_UNIT_TYPE destination, std::shared_ptr<const ContentType> content);
private:
    template <typename Tuple, std::size_t... ArgIndex>
    static DWORD InvokeCallable(LPVOID lpParam) noexcept;
    template <typename Tuple, std::size_t... ArgIndex>
    static auto constexpr GetInvoker(std::index_sequence<ArgIndex...>) noexcept;
    CONTROL_UNIT_TYPE m_Type;
    HANDLE m_hThread = nullptr;
    DWORD dwThreadId = 0;
    const CControlUnit& m_Superior; /* each CU has its unique superior, who is responsible for dealing with messages from its multiple inferiors */
};
} /* namespace CSOL_Utilities */

using namespace CSOL_Utilities;

template <typename Callable, typename... VarArg>
CControlUnit::CControlUnit(CONTROL_UNIT_TYPE type, Callable&& c, VarArg&&... va) : m_Type{ type }
{
    using Tuple = std::tuple<typename std::decay<Callable>::type, typename std::decay<VarArg>::type...>;
    auto closure = std::unique_ptr<Tuple>(std::forward<Callable>(c), std::forward<VarArg>(va)...);
    auto procedure = GetInvoker<Tuple>(std::make_index_sequence<sizeof...(VarArg) + 1>());
    m_hThread = CreateThread(
        nullptr,
        0,
        procedure,
        closure.get(),
        0,
        &dwThreadId
    );
    if (!m_hThread)
    {
        throw CException("Failed to create thread for control unit, error code: %lu.", GetLastError());
    }
    /* detach closure and leave it to the new thread if no error occurred */
    closure.release();
}

template <typename Tuple, std::size_t... ArgIndex>
DWORD CControlUnit::InvokeCallable(LPVOID lpParam) noexcept
{
    const std::unique_ptr<Tuple> param_list(static_cast<Tuple*>(lpParam)); /* take the ownership of params */
    Tuple& t = *param_list;
    std::invoke(std::move(std::get<ArgIndex>(t))...);
    return 0; /* param_list will destruct automatically */
}

template <typename Tuple, std::size_t... ArgIndex>
constexpr auto CControlUnit::GetInvoker(std::index_sequence<ArgIndex...>) noexcept
{
    return &InvokeCallable<Tuple, ArgIndex...>;
}