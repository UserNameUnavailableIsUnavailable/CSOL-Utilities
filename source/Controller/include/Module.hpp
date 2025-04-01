#pragma once

#include <atomic>
#include <cassert>
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include "CSOL_Utilities.hpp"
#include "Signal.hpp"
#include "Event.hpp"
#include "Console.hpp"

namespace CSOL_Utilities
{
    class Module
    {
    public:
        Module(std::string name, unsigned int interval = 500) :
            m_name(std::move(name)),
            m_interval(interval)
        { }
        void input(std::shared_ptr<Token> pin)
        {
            if (pin->possess())
            {
                m_in = pin;
            }
            else
            {
                throw std::runtime_error("托管输入信号失败：同一令牌只允许被一个线程拥有。");
            }
        }
        void output(std::string name, std::shared_ptr<UnitToken> pin)
        {

            if (pin->possess())
            {
                m_outs[name] = pin;
            }
            else
            {
                throw std::runtime_error("托管输出信号失败：同一令牌只允许被一个线程拥有。");
            }
        }
        void enable(std::string output_pin_name)
        {
            if (!m_outs.contains(output_pin_name))
            {
                return;
            }
            auto pin = m_outs[output_pin_name];
            pin->release();
        }
        void disable(std::string output_pin_name)
        {
            if (!m_outs.contains(output_pin_name))
            {
                return;
            }
            auto pin = m_outs[output_pin_name];
            pin->acquire();
        }
        virtual void bootstrap(std::shared_ptr<Event> _switch)
        {
            assert(!m_worker.joinable());
            assert(m_in);
            assert(_switch);
            m_exit_event = _switch;
            m_alive.store(1, std::memory_order_release);
            m_worker = std::thread([this] { entry(); });
        }
        virtual void suspend()
        {
            m_in->acquire();
        }
        virtual void resume()
        {
            m_in->release();
        }
        virtual void retire()
        {
            assert(m_worker.joinable());
            m_alive.store(false, std::memory_order_release);
            m_worker.join();
        }
    protected:
        virtual int entry()
        {
            int exit_status = 0;
            while (true)
            {
                m_in->acquire();
                if (m_alive == 0)
                {
                    m_in->release();
                    break;
                }
                try
                {
                    work();
                }
                catch (std::exception& e)
                {
                    // todo: 错误消息
                    std::cout << e.what() << std::endl;
                    m_in->release();
                    Console::Log(EXCEPTION_LEVEL::EL_ERROR, "模块 %s 遇到严重错误，即将退出。", m_name.c_str());
                    exit_status = -1;
                    break;
                }
                m_in->release();
                std::this_thread::sleep_for(std::chrono::milliseconds(m_interval));
            }
            m_exit_event->set();
            Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "模块 %s 退出。", m_name.c_str());
            // todo: 退出消息
            return exit_status;
        }
        virtual void work() = 0;
    private:
        std::string m_name;
        std::shared_ptr<Event> m_exit_event;
        std::shared_ptr<Token> m_in; /* 模块是否可以运行 */
        std::unordered_map<std::string, std::shared_ptr<UnitToken>> m_outs;
        std::atomic_bool m_alive = true;
        std::thread m_worker;
        unsigned int m_interval;
    };
}