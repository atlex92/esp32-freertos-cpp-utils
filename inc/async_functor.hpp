#pragma once

#include "task.hpp"
#include <functional>
#include <utility>

class AsyncFunctor : public StaticTask {
public:
    using function_t = std::function<void()>;
    explicit AsyncFunctor(function_t function, std::string task_name, uint16_t stack_size,
                          uint8_t priority, uint8_t core, bool place_stack_in_psram)
          : StaticTask(std::move(task_name), stack_size, priority, core, place_stack_in_psram),
            func_{std::move(function)} {}

private:
    void run() override {
        if (func_) {
            func_();
        }
    }
    function_t func_ = nullptr;
};