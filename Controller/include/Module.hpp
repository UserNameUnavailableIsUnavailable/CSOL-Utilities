#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <stop_token>
#include "Event.hpp"

namespace CSOL_Utilities
{
    class Module
    {
    public:
        virtual void Resume() = 0;
        virtual void Suspend() = 0;
    };
}