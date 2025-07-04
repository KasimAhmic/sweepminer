#pragma once

#include <functional>
#include <thread>

class Timer {
public:
    Timer(const std::function<void()> &callback, uint32_t interval);
    ~Timer();

    void start();
    void stop();

private:
    std::function<void()> callback;
    uint32_t interval;
    std::atomic<bool> running;
    std::thread worker;
};
