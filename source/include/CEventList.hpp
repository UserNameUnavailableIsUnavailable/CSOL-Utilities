#pragma once

#include "CEvent.hpp"
#include <condition_variable>
#include <initializer_list>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace CSOL_Utilities
{
class CMannualEventList
{
  private:
    std::vector<CMannualEvent *> m_List;

  public:
    CMannualEventList(std::initializer_list<CMannualEvent *> init_list)
    {
        for (auto pe : init_list)
        {
            if (pe)
            {
                m_List.push_back(pe);
            }
            else
            {
                throw std::invalid_argument("nullptr provided in initializer list.");
            }
        }
    };
    void WaitAll()
    {
        auto i = m_List.begin();
        while (i != m_List.end())
        {
            if (!(*i)->PeekAndWait())
            {
                i = m_List.begin(); /* 从头开始等待 */
                continue;
            }
            i++;
        }
    }
};
}; // namespace CSOL_Utilities
