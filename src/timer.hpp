#pragma once
#include <functional>
#include <thread>

class Timer {
public:
    Timer(const std::function<void()> &callback, long interval);
    ~Timer();

    void start();
    void stop();
    void restart();
private:
    std::function<void(void)> callback;
    long interval;
    bool running;
    std::thread thread;
};
