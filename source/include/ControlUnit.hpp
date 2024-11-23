#pragma once
#include "CException.hpp"
#include "Mailbox.hpp"
#include <Windows.h>
#include <condition_variable>
#include <cstddef>
#include <errhandlingapi.h>
#include <functional>
#include <minwindef.h>
#include <mutex>
#include <processthreadsapi.h>
#include <rpcdce.h>
#include <shared_mutex>
#include <synchapi.h>
#include <unknwnbase.h>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace CSOL_Utilities
{
/* cu type specifier to be used as `wParam` in `PostThreadMessage` */
using CU = enum class CONTROL_UNIT_TYPE
{
    CU_MASTER_CONTROLLER,
    CU_STATE_ANALYZER,
    CU_GAME_SUPERVISOR,
    CU_COMMAND_DISPATCHER,
    CU_MODE_SWITCHER,
    CU_EXECUTOR_DEBUGGER,
};

/* cu messages to be used as `lParam` in `PostThreadMessage` */
using CUMSG = enum class CONTROL_UNIT_MESSAGE_TYPE
{
    CUMSG_RUN,
    CUMSG_EXIT,
    CUMSG_MAILBOX,
    CUMSG_DUMMY,
};

template <typename MailboxType>
class ControlUnit
{
public:
    DWORD GetThreadId() const { return m_dwThreadId; }
    HANDLE GetThreadHandle() const { return m_hThread; }
    CU GetCUType() const { return m_Type; }
    template <typename Callable, typename... VarArg>
    ControlUnit(CONTROL_UNIT_TYPE type, Callable&& c, VarArg&&... va);
    ~ControlUnit() noexcept
    {
        PostThreadMessageW(m_dwThreadId, WM_QUIT, 0, 0);
        auto ret = WaitForSingleObject(m_hThread, 2000);
        if (ret != WAIT_OBJECT_0)
        {
            TerminateThread(m_hThread, -1);
        }
        CloseHandle(m_hThread);
        m_dwThreadId = 0;
        m_hThread = nullptr;
    };
    void SendControlMessage(CU destination, CUMSG msg);
    template <typename DestMailboxType>
    void SendControlMessage(CU destination, CUMSG msg, const DestMailboxType& content);
    void BroadcastControlMessage(CUMSG msg);
    void Quit() noexcept;
protected:
    Mailbox<MailboxType> m_Mailbox;
private:
    std::shared_mutex m_CUListMutex;
    std::unordered_map<CONTROL_UNIT_TYPE, ControlUnit*> m_CUMap;
    template <typename Tuple, std::size_t... ArgIndex>
    static DWORD InvokeCallable(LPVOID lpClosure);
    template <typename Tuple, std::size_t... ArgIndex>
    static auto constexpr GetInvoker(std::index_sequence<ArgIndex...>) noexcept;
    CONTROL_UNIT_TYPE m_Type;
    HANDLE m_hThread = nullptr;
    DWORD m_dwThreadId = 0;
};

template <typename MailboxType>
void ControlUnit<MailboxType>::SendControlMessage(CU destination, CUMSG msg)
{
    if (!m_CUMap.find(destination)) return;
    PostThreadMessageW(m_CUMap[destination].GetThreadId(), WM_APP, static_cast<WPARAM>(destination), static_cast<LPARAM>(msg));
}

template <typename MailboxType>
template <typename DestMailboxType>
void ControlUnit<MailboxType>::SendControlMessage(CU destination, CUMSG msg, const DestMailboxType& content)
{
    if (!m_CUMap.find(destination)) return;
    m_CUMap[destination]->m_MailBox.Post(content);
    PostThreadMessageW(m_CUMap[destination].GetThreadId(), WM_APP, static_cast<WPARAM>(destination), static_cast<LPARAM>(msg));
}

template <typename MailBoxType>
template <typename Callable, typename... VarArg>
ControlUnit<MailBoxType>::ControlUnit(CONTROL_UNIT_TYPE type, Callable&& c, VarArg&&... va) : m_Type{ type }
{
    std::unique_lock _(m_CUListMutex);
    using Tuple = std::tuple<typename std::decay<Callable>::type, typename std::decay<VarArg>::type...>;
    auto closure = std::unique_ptr<Tuple>(std::forward<Callable>(c), std::forward<VarArg>(va)...);
    auto procedure = GetInvoker<Tuple>(std::make_index_sequence<sizeof...(VarArg) + 1>());
    m_hThread = CreateThread(
        nullptr,
        0,
        procedure,
        closure.get(),
        0,
        &m_dwThreadId 
    );
    if (!m_hThread)
    {
        throw CException("创建控制单元线程时发生错误，错误代码 %lu。", GetLastError());
    }
    closure.release();
    m_CUMap.insert_or_assign(type, dynamic_cast<ControlUnit*>(this));
}

template <typename MailBoxType>
template <typename Tuple, std::size_t... ArgIndex>
DWORD ControlUnit<MailBoxType>::InvokeCallable(LPVOID lpClosure)
{
    const std::unique_ptr<Tuple> param_list(static_cast<Tuple*>(lpClosure)); /* take the ownership of params */
    Tuple& t = *param_list;
    std::invoke(std::move(std::get<ArgIndex>(t))...);
    return 0; /* param_list will destruct automatically */
}

template <typename MailBoxType>
template <typename Tuple, std::size_t... ArgIndex>
constexpr auto ControlUnit<MailBoxType>::GetInvoker(std::index_sequence<ArgIndex...>) noexcept
{
    return &InvokeCallable<Tuple, ArgIndex...>;
}

} /* namespace CSOL_Utilities */