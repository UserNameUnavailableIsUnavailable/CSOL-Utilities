#pragma once

#include <unordered_set>

class Channel;

class Multiplexer {
public:
    Multiplexer();
    ~Multiplexer() noexcept;
    void AddChannel(Channel* channel);
    void DeleteChannel(Channel* channel);
private:
    std::uintptr_t handle_{0};
    // several channel may share the same underlying kernel object
    std::unordered_multiset<std::uintptr_t> channels_;
};