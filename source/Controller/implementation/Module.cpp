#include "Module.hpp"
#include <atomic>
#include <cassert>
#include <exception>
#include <memory>
#include <mutex>
#include <unordered_map>

using namespace CSOL_Utilities;

ModuleBase::ModuleBase(std::string name, unsigned int message_queue_capacity) :
    m_name(name),
    m_MESSAGE_QUEUE_CAPACITY(message_queue_capacity),
    m_id(++s_module_counter) // id 分配时从 1 开始
{
}

ModuleBase::~ModuleBase() noexcept
{
    std::lock_guard lock(s_module_table_mutex);
    s_MODULE_TABLE.erase(m_id);
}

// 打工人入职。
void ModuleBase::entry()
{
    while (true)
    {
        std::unique_lock lock(m_worker_state_mutex);
        m_should_resume.wait(lock, [this] {
            return !m_sacked && m_task_assigned_to_worker || m_retired;
        }); // 未被开除的情况下，有任务则工作，无任务则休息，只要老板不让打工人停下，就一直工作；退休则收拾东西走人
        if (m_retired) { break; } // 退休
        m_task_finished_by_worker = false;
        lock.unlock();
        try {
            run();
        } catch (std::exception& e) {
            std::lock_guard lock(m_worker_state_mutex);
            m_dead = true;
            m_can_suspend.notify_one(); // 打工人只有一个直接上级，用 notify_one
            break; // 结束
        }
        lock.lock();
        m_task_finished_by_worker = true;
        m_can_suspend.notify_one(); // 打工人只有一个直接上级，告诉等待此打工人完成任务的老板
    }
}

// 让打工人休息。
void ModuleBase::rest()
{
    std::unique_lock lock(m_worker_state_mutex);
    // 老板良心发现
    m_can_suspend.wait(lock, [this] {
        return m_task_finished_by_worker || m_sacked || m_retired || m_dead; // 工作完成，则可以让打工人休息；或者，打工人被开除、退休、猝死
    });
    m_task_assigned_to_worker = false; // 不再派发新的任务给打工人
}

// 让打工人上班。
void ModuleBase::work()
{
    std::unique_lock lock(m_worker_state_mutex);
    // 打工人可能被开除、退休、猝死，但老板只管发任务
    m_task_assigned_to_worker = true; // 派发新的任务给打工人
    m_should_resume.notify_one(); // 唤醒可能正在休息的打工人
}

// 开除打工人。
void ModuleBase::sack()
{
    std::unique_lock lock(m_worker_state_mutex);
    m_can_suspend.wait(lock, [this] {
        return m_task_finished_by_worker || m_sacked || m_retired || m_dead;
    });
    m_sacked = true; // 打工人被炒鱿鱼
    m_should_resume.notify_one(); // 通知打工人收拾东西滚蛋
}

// 让打工人退休。
void ModuleBase::retire()
{
    std::unique_lock lock(m_worker_state_mutex);
    m_can_suspend.wait(lock, [this] {
        return m_task_finished_by_worker || m_sacked || m_retired || m_dead;
    });
    m_retired = true; // 打工人退休
    m_should_resume.notify_one(); // 通知打工人收拾东西滚蛋
}

std::mutex ModuleBase::s_module_table_mutex = std::mutex();
std::unordered_map<unsigned int , std::weak_ptr<ModuleBase>> ModuleBase::s_MODULE_TABLE = std::unordered_map<unsigned int , std::weak_ptr<ModuleBase>>();
std::atomic_uint ModuleBase::s_module_counter = 0; // 计数器，对计数器的访问是原子的


// 向上级报告消息。
// @param `message` 消息
// @return 操作是否成功
bool ModuleBase::send_message_to_boss(MODULE_MESSAGE_T message)
{
    ModuleMessage module_message {
        .id = m_id,
        .message = message
    };
    auto boss_module = get_module(m_boss_id);
    if (!m_boss_id)
    {
        return false;
    }
    std::unique_lock<std::mutex> lock(boss_module->m_message_queue_mutex);
    boss_module->m_message_queue_not_full.wait(lock, [&] { return boss_module->m_message_queue.size() < m_MESSAGE_QUEUE_CAPACITY; });
    boss_module->m_message_queue.push_back(module_message); // 插入一则消息
    m_message_queue_not_empty.notify_one();
    return true;
}


// 向下级通知消息
// @param `message` 消息
// @param `employee_module_id` 子模块 `id`
// @return 操作是否成功
bool ModuleBase::send_message_to_employee(MODULE_MESSAGE_T message, unsigned int employee_module_id)
{
    ModuleMessage module_message {
        .id = m_id,
        .message = message
    };
    auto boss_module = get_module(m_boss_id);
    if (!m_boss_id)
    {
        return false;
    }
    if (m_staff_ids.find(employee_module_id) == m_staff_ids.end()) // 是否包含具有此 id 的子模块
    {
        return false;
    }
    auto employee_module = get_module(employee_module_id);
    if (!employee_module)
    {
        return false;
    }
    std::unique_lock<std::mutex> lock(employee_module->m_message_queue_mutex);
    employee_module->m_message_queue_not_full.wait(lock, [&] { return employee_module->m_message_queue.size() < m_MESSAGE_QUEUE_CAPACITY; });
    employee_module->m_message_queue.push_back(module_message); // 插入一则消息
    m_message_queue_not_empty.notify_one();
    return true;
}

// 向下级广播消息。
// @param `message` 消息
// @return 若存在通知失败的情况，返回 `false`；否则，返回 `true`。
bool ModuleBase::broadcast_message_to_staff(MODULE_MESSAGE_T message)
{
    auto boss_module = get_module(m_boss_id);
    if (!m_boss_id)
    {
        return false;
    }
    bool ret = true;
    for (auto id : m_staff_ids)
    {
        ret = ret && send_message_to_employee(message, id);
    }
    return ret;
}

// 接收消息
bool ModuleBase::get_message(ModuleMessage& message)
{
    std::unique_lock<std::mutex> lock(m_message_queue_mutex);
    m_message_queue_not_empty.wait(lock, [&] { return m_message_queue.size() > 0; });
    message = std::move(m_message_queue.front());
    m_message_queue.pop_front();
    m_message_queue_not_full.notify_one();
    return true;
}

std::shared_ptr<ModuleBase> ModuleBase::get_module(unsigned int id)
{
    auto it = s_MODULE_TABLE.find(id);
    if (it == s_MODULE_TABLE.end())
    {
        return nullptr;
    }
    return it->second.lock(); // 将 weak_ptr 转换成 shared_ptr
}

// 招聘打工人。
void ModuleBase::recruit(unsigned int id)
{
    auto employee = get_module(id);
    if (!employee)
    {
        return;
    }
    // 锁定该打工人，让其暂时不要干活
    std::unique_lock lock(employee->m_worker_state_mutex);
    employee->m_can_suspend.wait(lock, [employee] {
        return employee->m_task_finished_by_worker || employee->m_sacked || employee->m_retired || employee->m_dead;
    });
    // 打工人是否已经有老板，兼职的不要
    if (employee->m_boss_id != 0 && !employee->m_sacked)
    {
        return;
    }
    // 打工人是否已经退休，老的不要  
    if (employee->m_retired)
    {
        return;
    }
    // 打工人是不是还活着，死的不要
    if (employee->m_dead)
    {
        return;
    }
    // 打工人符合招聘条件：
    // - m_boss_id == 0 || m_sacked = true：没有老板，或者被炒
    // - m_retire == false 没有退休
    // - m_dead == false 没有死
    employee->m_boss_id = m_id; // 打工人认自己做老板
    employee->m_sacked = false;
    m_staff_ids.insert(employee->m_id); // 录入打工人身份证号
    // 让打工人开始干活

}