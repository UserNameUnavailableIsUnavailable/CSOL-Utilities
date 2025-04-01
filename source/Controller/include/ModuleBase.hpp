#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>

namespace CSOL_Utilities
{
    struct ModuleMessage
    {
        unsigned int id; // 发送方 id
        std::string content; // 消息内容
    };
    class ModuleBase;
    class ModuleFactory;

    // 模块基类
    class ModuleBase
    {
    friend class ModuleFactory;
    public:
        unsigned int get_id() const noexcept { return m_id; }
        std::string get_name() const noexcept { return m_name; }
        virtual std::thread::native_handle_type get_worker_handle() const noexcept = 0; // 获取打工人线程句柄
        virtual void resume() noexcept = 0; // 无条件让打工人复工
        virtual void suspend() noexcept = 0; // 无条件让打工人停工
        virtual void retire() noexcept = 0; // 打工人退休
        virtual bool bootstrap() noexcept = 0; // 模块构造完成后，通过此成员函数启动打工人线程，确保线程安全
        virtual void send(unsigned int id, ModuleMessage&& module_message) = 0;
        virtual void send(unsigned int id, ModuleMessage&& module_message, unsigned int timeout_milliseconds) = 0;
        void set_message_queue_size(std::size_t size) noexcept;
        virtual void receive(ModuleMessage& module_message) = 0;
        virtual void receive(ModuleMessage& module_message, unsigned int timeout_milliseconds) = 0;
        virtual ~ModuleBase();
    protected:
        ModuleBase(std::string name) :
            m_name(name), m_id(++s_id_pool)
        {};
        virtual void entry() noexcept = 0; // 打工人入职
        virtual bool work() noexcept = 0; // 打工人的一轮工作
    private:
        static std::atomic_uint s_id_pool; // 分配标识符
        const unsigned int m_id;
        const std::string m_name; // 打工人名称
        std::weak_ptr<ModuleFactory> m_manufacturer;
    };

    class ModuleFactory : std::enable_shared_from_this<ModuleFactory>
    {
        struct Private { explicit Private() = default; };
    public:
        template <typename ModuleType, typename... Arguments>
        std::shared_ptr<ModuleType> make_module(Arguments&&... args);
        std::shared_ptr<ModuleBase> find_module(unsigned int id);
        void remove_module(unsigned int id);
        ModuleFactory(ModuleFactory&) = delete;
        ModuleFactory(ModuleFactory&&) = delete;
        static std::shared_ptr<ModuleFactory> create();
        ModuleFactory(Private) {  }
    private:
        std::mutex s_module_table_lock;
        std::unordered_map<unsigned int, std::weak_ptr<ModuleBase>> s_module_table;
    };

    template <typename ModuleType, typename... Arguments>
    std::shared_ptr<ModuleType> ModuleFactory::make_module(Arguments&&... args)
    {
        static_assert(std::is_base_of_v<ModuleBase, ModuleType>, "ModuleType must derive from ModuleBase.");
        std::lock_guard lock(s_module_table_lock);
        auto instance = std::make_shared<ModuleType>(std::forward<Arguments>(args)...);
        instance->m_manufacturer = weak_from_this();
        s_module_table[instance->m_id] = instance; // 记录 id 与实例映射
        return instance;
    }
}