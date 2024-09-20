#pragma once

#include "CEvent.hpp"
#include <initializer_list>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <vector>

namespace CSOL_Utilities
{
    class CEventList
    {
    private:
        std::mutex m_Mutex;
        std::condition_variable m_CondVar;
        std::vector<CEvent*> m_List;
    public:
        CEventList(std::initializer_list<CEvent*> init_list)
        {
            for (auto pe : init_list) {
                if (pe) {
                    m_List.push_back(pe);
                } else {
                    throw std::invalid_argument("nullptr provided in initializer list.");
                }
            }
        };
        void WaitAll()
        {
            std::unique_lock<std::mutex> unique_lock(m_Mutex);
            m_CondVar.wait(
                unique_lock,
                [this] {
                    for (auto& event : m_List) {
                        if (!event->Signaled()) {
                            return false;
                        }
                    }
                    return true;
                }
            );
        }
    };
};
