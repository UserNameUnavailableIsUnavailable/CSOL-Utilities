#include "Element.hpp"
#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include "Exception.hpp"
#include "StringLiterals.hpp"

namespace CSOL_Utilities
{
	void ConnectElements(std::shared_ptr<CircuitModule> before, std::shared_ptr<CircuitModule> after, bool mask)
	{
		if (!before || !after) // 前后级不允许为空
		{
			throw Exception(ELEMENT::MSG_NULL_ELEMENT);
		}

		// 锁定锁存器
		std::scoped_lock locks(before->m_input_latches_lock, after->m_input_latches_lock);

		if (auto before_output_element = before->m_output_latch) // 判断前级输出引脚是否已经连接到其他元件
		{
			throw Exception(ELEMENT::MSG_ELEMENT_ALREADY_HAS_OUTPUT);
		}

		// 新建一个锁存器
		auto latch = std::make_shared<Latch>(before, // input_pin
											 after, // output_pin
											 !mask, // value
											 mask); // 锁存器输出信号为 0，旨在先将后级器件置于挂机状态

		// 前后级元件通过锁存器连接
		before->m_output_latch = latch; // 前级元件接入锁存器作为输入
		after->m_inputs.push_back(latch); // 后级元件输入引脚连接到前级元件
	}

	void CircuitModule::entry() // 工作线程入口
	{
		while (true)
		{
			std::unique_lock lock(m_worker_state_lock);
			m_should_worker_resume.wait(lock,
										[this]
										{
											return !m_latches_resolved || m_worker_state == 1 ||
												m_worker_state == -1; // 锁存器更新、正常（1）、异常（-1）
										});
			if (!m_latches_resolved) // 如果锁存器信号变更，则根据信号计算出工作状态
			{
				m_worker_state = m_worker_state != -1 ? compute() : -1;
			}
			if (m_worker_state == -1) // 退出
			{
				break;
			}
			// 工作信号需要作为结果传递给下一级
			// 在信号传递完成之前不解锁
			try
			{
				int signal = work(); // 将工作结果作为输出信号输出到下级
			}
			catch (std::exception& e)
			{
				m_worker_state = -1;
				break; // 结束运行
			}
		}
		out(-1); // 向下一级输出异常
	}

	// 获取输入信号，更新工作状态
	void CircuitModule::in()
	{
		std::unique_lock lock(m_input_latches_lock); // 锁定锁存器
		// 继续运行条件：
		// - 锁存器尚未处理
		// - 非暂停状态
		m_should_worker_resume.wait(lock, [this] { return !m_latches_resolved || m_worker_state != 0; });
		if (!m_latches_resolved) // 如果锁存器信号变更，则根据信号计算出工作状态
		{
			m_worker_state = m_worker_state == -1 ? -1 : compute();
		}


		lock.unlock();
	}

	// 将信号输出到下一级输入锁存器
	void CircuitModule::out(int signal)
	{
		if (!m_output_latch) // 没有下一级元件
		{
			return;
		}
		auto next_element = m_output_latch->output_element.lock();
		if (next_element) // 下一级元件存在
		{
			// 出作用域解锁
			{
				// 暂停下一级条件：
				// - 锁存器已经处理完毕
				// - 暂停状态
				std::unique_lock lock(next_element->m_input_latches_lock);
				next_element->m_can_worker_suspend.wait(
					lock,
					[&]
					{
						return next_element->m_latches_resolved; // 下一级已经处理过锁存器，可以写入
					});
				m_output_latch->value = signal; // 锁存器内容更新
				next_element->m_latches_resolved = false;
			}
			next_element->m_should_worker_resume.notify_one(); // 唤醒
		}
		// latch->value = signal; // 当前元件写，下一级元件读，故只用最简单的原子互斥
		// if (signal == -1 || signal && latch->mask) //
		// 异常或正常（即不是挂起），则说明下一级元件也许能够运行，这时需要尝试将其唤醒
		// {
		//     m_should_worker_resume.notify_one(); //
		//     传入新信号之后，下一级可能处于挂起态（不接收锁存器输入），将其唤醒
		// }
	}

	// 计算工作状态
	int CircuitModule::compute()
	{
		int state = 1;
		// 加锁由 in 保证
		for (auto i : m_inputs)
		{
			if (i->value == -1) // 非法
			{
				return -1;
			}
			auto masked_value = i->value && i->mask;
			if (!masked_value)
			{
				return 0;
			}
			state = state && masked_value;
		}
		return state;
	}
} // namespace CSOL_Utilities
