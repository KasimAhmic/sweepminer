#include "timer.hpp"

Timer::Timer(const std::function<void()> &callback, const uint32_t interval):
    callback(callback),
    interval(interval),
    running(false) {}

Timer::~Timer() {
    this->stop();
}

void Timer::start() {
    if (this->running.load()) {
        return;
    }

    this->running.store(true);

    this->worker = std::thread([this] {
        while (this->running.load()) {
            const auto delta = std::chrono::steady_clock::now() + std::chrono::milliseconds(this->interval);
            this->callback();
            std::this_thread::sleep_until(delta);
        }
    });
}

void Timer::stop() {
    this->running.store(false);

    if (this->worker.joinable()) {
        worker.join();
    }
}
