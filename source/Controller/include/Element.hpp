#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>
#include <Windows.h>
#include <winscard.h>

namespace CSOL_Utilities
{
    // 器件的引脚，内部集成非门
    // 引脚输出端允许接到多个其他引脚上
    class Pin : std::enable_shared_from_this<Pin>
    {
        struct PinInit
        {
            int raw_level;
            bool reverse;
        };
    public:
        Pin(PinInit& init) : m_raw_level(init.raw_level), m_reverse(init.reverse) {  };
        // 创建引脚
        static std::shared_ptr<Pin> create(int raw_level, bool reverse)
        {
            PinInit init { .raw_level = raw_level, .reverse = reverse };
            return std::make_shared<Pin>(init);
        }
        // 连接两个引脚
        void connect(std::shared_ptr<Pin> pin)
        {
            if (pin)
            {
                m_next_pins.push_back(pin);
                pin->in(out()); // 向该引脚输出信号
            }
        }
        // 输出经过信号处理的引脚电平信号
        int out() const noexcept
        {
            // 尝试获取前级更新
            if (m_raw_level == -1) { return -1; }
            if (m_reverse) { return !m_raw_level; }
            return m_raw_level;
        }
        // 将电平信号输入至此引脚，即更新此引脚的原始电平信号，信号更新级联传导
        void in(int signal) noexcept
        {
            if (signal == m_raw_level) { return; }
            signal = m_raw_level;
            // 级联更新
            for (auto pin : m_next_pins)
            {
                if (auto p = pin.lock())
                {
                    p->in(out()); // 向所有的后级引脚输出信号
                }
            }
        }
        Pin& operator=(int value) noexcept
        {
            in(value);
            return *this;
        }
        Pin& operator>>(Pin& pin) // 输出
        {
            pin.in(out());
            return *this;
        }
        Pin& operator<<(Pin& pin) // 输入
        {
            in(pin.out());
            return *this;
        }
        int operator&&(Pin& pin) const noexcept
        {
            if (out() == -1 || pin.out() == -1) { return -1; } // 遇到非法电平直接输出 -1
            return out() && pin.out();
        }
        int operator||(Pin& pin) const noexcept
        {
            if (out() == -1 || pin.out() == -1) { return -1; } // 遇到非法电平直接输出 -1
            return out() && pin.out();
        }
        int operator!() const noexcept
        {
            if (out() == -1) { return -1; } // 遇到非法电平直接输出 -1
            return !out();
        }
    private:
        std::atomic_int m_raw_level; // 原始电平信号
        const bool m_reverse = false; // 信号处理，是否反转电平（是否启用非门）
        std::vector<std::weak_ptr<Pin>> m_next_pins; // 后级引脚
    };

    // 抽象类：门
    class Gate
    {
    public:
        virtual void compute() = 0; // 计算
        int out() { compute(); return m_out->out(); }
    protected:
        std::shared_ptr<Pin> m_in_0;
        std::shared_ptr<Pin> m_in_1;
        std::shared_ptr<Pin> m_out;
    };

    // 与门
    class AndGate : public Gate
    {
    public:
        virtual void compute() override
        {
            *m_out = *m_in_0 && *m_in_1;
        }
    };

    // 或门
    class OrGate : public Gate
    {
    public:
        virtual void compute() override
        {
            *m_out = *m_in_0 || *m_in_1;
        }
    };


    // 元件
    class Element
    {
    public:
    private:
        std::shared_ptr<Pin> m_in; // 输入引脚
        std::shared_ptr<Pin> m_out; // 输出引脚
    };

    class CircuitModule;
    // 锁存器
    struct Latch
    {
        std::weak_ptr<CircuitModule> input_element; // 输入引脚
        std::weak_ptr<CircuitModule> output_element; // 输出引脚
        std::atomic_int value; // 锁存器的值
        const bool mask;
    };

    // 电路元件
    // 高电平 1 有效（运行）、低电平 0 无效（挂起）、非法电平 -1 异常（退出）。
    // 每个元件有任意多个输入引脚和一个输出引脚。
    // 两个不同元件的输出引脚、输入引脚之间通过锁存器连接，锁存器中有掩码。
    // 锁存器输出通过锁存器内的值和掩码与运算得到，非法电平不参与计算，直接输出 -1。
    // 元件从锁存器获取上一级输出值后，做与运算确定工作状态，若遇到非法电平则工作状态直接置为 -1（异常工作状态）。
    class CircuitModule : public std::enable_shared_from_this<CircuitModule>
    {
        // 连接两个元件
        friend void ConnectElements(std::shared_ptr<CircuitModule> before, std::shared_ptr<CircuitModule> after, bool mask);
    public:
        virtual int work() = 0; // 工作一轮，产生的输出传递给下一级
        virtual void entry();

        // 根据前级输入信号计算本元件状态
        // 若状态发生变更，则将新的状态写入后级锁存器
        virtual void in(); // 输入（单线程内运行）
        virtual void out(int signal); // 输出（单线程内运行）
        virtual int compute(); // 根据输入信号计算元件状态，由 in 调用并确保互斥，故子类重写时无需加锁
    protected:
        std::mutex m_input_latches_lock; // 锁定输入锁存器
        std::vector<std::shared_ptr<Latch>> m_inputs; // 多个输入锁存器（效率起见，采用串行输入方式，带来的问题是异步更新）
        std::vector<bool> m_mask; // 输入信号掩码，1 表示保持，0 表示反转，-1 表示异常
        std::shared_ptr<Latch> m_output_latch; // 输出引脚连接到一个元件
        int m_worker_state; // 状态：-1（非法电平，打工人工作完将退休）/ 0（低电平，打工人休息）/ 1（高电平，打工人在工作）
        bool m_task_finished_by_worker; // true（已经执行完一次任务）、false（尚未执行完一次任务）
        bool m_latch_updated = false; // 锁存器是否更新
        mutable std::condition_variable m_should_compute; // 锁存器更新事件
        mutable std::mutex m_worker_state_lock; // 锁定打工人状态
        bool m_latches_resolved = true;
        std::condition_variable m_can_worker_suspend; // 是否可以暂停
        std::condition_variable m_should_worker_resume; // 是否可以继续
    };

    // 与门
    void ConnectElements(std::shared_ptr<CircuitModule> before, std::shared_ptr<CircuitModule> after, bool mask = true);
}