#pragma once

#include "CEvent.hpp"
#include <initializer_list>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <vector>

namespace CSOL_Utilities
{
    class CMannualEventList
    {
    private:
        std::vector<CMannualEvent*> m_List;
    public:
        CMannualEventList(std::initializer_list<CMannualEvent*> init_list)
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
            for (auto i = m_List.begin(); i != m_List.end(); i++) {
                if (!(*i)->PeekBeforeWait()) {
                    i = m_List.begin(); /* 从头开始等待 */
                }
            }
        }
    };
};
