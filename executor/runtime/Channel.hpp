#pragma once

enum class ChannelType;

class Channel {
public:
    Channel(std::uintptr_t handle);
    ~Channel() noexcept;
    std::uintptr_t handle() const { return handle_; }
    ChannelType type() const { return type_; }
    void Arm();
    void Disarm();
private:
    std::uintptr_t handle_;
    ChannelType type_;
};