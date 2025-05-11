#include "timer.hpp"

Timer::Timer(const std::function<void()> &callback, const long interval) {
    this->callback = callback;
    this->interval = interval;
    this->running = false;
}

Timer::~Timer() {
    this->stop();
}

void Timer::start() {
    this->running = true;

    this->thread = std::thread([&] {
        while (this->running) {
            const auto delta = std::chrono::steady_clock::now() + std::chrono::microseconds(this->interval);
            this->callback();
            std::this_thread::sleep_until(delta);
        }
    });

    this->thread.detach();
}

void Timer::stop() {
    this->running = false;
    this->thread.~thread();
}

void Timer::restart() {
    this->stop();
    this->start();
}
