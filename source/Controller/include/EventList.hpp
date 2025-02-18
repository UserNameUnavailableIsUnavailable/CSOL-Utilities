#pragma once

#include "Event.hpp"
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace CSOL_Utilities
{
class EventList
{
  private:
    std::vector<Event *> m_List;

  public:
    EventList(std::initializer_list<Event *> init_list)
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
