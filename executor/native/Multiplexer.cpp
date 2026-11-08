#include <executor/native/Multiplexer.hpp>
#include <executor/native/Channel.hpp>

#include <stdexcept>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Multiplexer::Multiplexer() {
    auto h = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (!h) {
        throw std::runtime_error("Failed to create IO completion port");
    }
    handle_ = reinterpret_cast<std::uintptr_t>(h);
}

void Multiplexer::AddChannel(Channel* channel) {
    auto h = channel->handle();
    if (!CreateIoCompletionPort(reinterpret_cast<HANDLE>(h), reinterpret_cast<HANDLE>(handle_), 0, 0)) {
        throw std::runtime_error("Failed to add channel to IO completion port");
    }
    channels_.insert(h);
}

void Multiplexer::DeleteChannel(Channel* channel) {
    auto h = channel->handle();
    channels_.erase(h);
}

Multiplexer::~Multiplexer() noexcept{
    if (handle_) {
        CloseHandle(reinterpret_cast<HANDLE>(handle_));
    }
}
