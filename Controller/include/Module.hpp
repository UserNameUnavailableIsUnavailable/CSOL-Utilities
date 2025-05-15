#pragma once

namespace CSOL_Utilities
{
    class Module
    {
    public:
        virtual void Resume() = 0;
        virtual void Suspend() = 0;
        virtual ~Module() = default;
    };
}