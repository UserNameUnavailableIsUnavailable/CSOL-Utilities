#include <memory>
#include <tuple>
#include <any>
#include <type_traits>
#include <utility>

namespace CSOL_Utilities
{
    class CallableInterface
    {
    public:
        virtual ~CallableInterface() = default;
        virtual void Invoke() = 0;
        virtual std::any GetResult() const noexcept = 0;
    };

    template <typename Method, typename... ARGS>
    class CallableImplementation : public CallableInterface
    {
    friend void swap(CallableImplementation& c1, CallableImplementation& c2)
    {
        using std::swap;
        swap(c1.m_Method, c2.m_Method);
        swap(c1.m_Args, c2.m_Args);
        swap(c1.m_Result, c2.m_Result);
    }
    public:
        CallableImplementation(Method&& method, ARGS&&... args) :
            m_Method(std::forward<Method>(method)), m_Args(std::make_tuple(std::forward<ARGS>(args)...)) { }
        CallableImplementation(const CallableImplementation& callableimpl) = delete;
        CallableImplementation operator=(const CallableImplementation&) = delete;
        CallableImplementation(CallableImplementation&& callableimpl)
        {
            swap(*this, callableimpl);
        }
        CallableImplementation operator=(CallableImplementation &&c)
        {
            swap(*this, &c);
            return *this;
        }
        void Invoke() override
        {
            if constexpr (std::is_same_v<decltype(m_Method()), void>)
            {
                std::apply(m_Method, m_Args);
            }
            else
            {
                m_Result = std::move(std::apply(m_Method, m_Args));
            }
        }
        std::any GetResult() const noexcept override 
        {
            return m_Result;
        }
    private:
        Method m_Method;
        std::tuple<ARGS...> m_Args;
        std::any m_Result = 0;
    };

    class Callable
    {
    friend void swap(Callable& c1, Callable& c2) noexcept
    {
        using std::swap;
        swap(c1.m_Callable, c2.m_Callable);
    }
    public:
        template <typename Method, typename... ARGS>
        // 不使用 explicit，允许隐式构造
        Callable(Method&& method, ARGS&&... args) :
            m_Callable(std::make_unique<CallableImplementation<Method, ARGS...>, Method, ARGS...>(std::forward<Method>(method), std::forward<ARGS>(args)...)) { }
        Callable(const Callable&) = delete;
        Callable& operator=(const Callable&) = delete;
        Callable(Callable&& callable) noexcept
        {
            swap(*this, callable);
        }
        Callable& operator=(Callable&& callable)
        {
            swap(*this, callable);
            return *this;
        }
        auto operator()()
        {
            m_Callable->Invoke();
            return m_Callable->GetResult();
        }
    private:
        std::unique_ptr<CallableInterface> m_Callable;
    };
};