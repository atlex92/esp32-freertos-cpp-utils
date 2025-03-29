#pragma once

#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <string>
#include <utility>

class BaseTask {
public:
    explicit BaseTask(std::string name, size_t stack_size, uint8_t prio, uint8_t core)
          : name_(std::move(name)), stack_size_((stack_size)), prio_(prio), core_(core) {}

    virtual ~BaseTask() = default;

    BaseTask(const BaseTask &) = delete;
    BaseTask(BaseTask &&) = delete;
    BaseTask &operator=(const BaseTask &) = delete;
    BaseTask &operator=(BaseTask &&) = delete;

    virtual void start() = 0;

    [[nodiscard]] std::string name() const {
        return name_;
    }

    [[nodiscard]] size_t stackSize() const {
        return stack_size_;
    }

    [[nodiscard]] uint8_t priority() const {
        return prio_;
    }

    [[nodiscard]] uint8_t core() const {
        return core_;
    }

    [[nodiscard]] bool isRunning() const {
        return is_running_;
    }

    static void delayMs(uint64_t ms) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }

protected:
    void setRunning(bool val) {
        is_running_ = val;
    }
    static void runTask(void *arg);

private:
    virtual void run() = 0;
    std::string name_;
    size_t stack_size_;
    uint8_t prio_;
    uint8_t core_;
    std::atomic_bool is_running_ = false;
};

class Task : public BaseTask {
public:
    friend class StaticTask;
    using BaseTask::BaseTask;
    void start() override;

private:
    static constexpr const char *kTag = "Task";
    TaskHandle_t task_handle_ = nullptr;
};

class StaticTask : public Task {
public:
    explicit StaticTask(std::string name, size_t stack_size, uint8_t prio, uint8_t core,
                        bool place_stack_in_psram)
          : Task(std::move(name), stack_size, prio, core),
            place_stack_in_psram_(place_stack_in_psram) {}

    void start() override;

private:
    struct CommonDeleter {
        void operator()(void *ptr) const {
            // NOLINTNEXTLINE
            free(ptr);
        }
    };
    static constexpr const char *kTag = "StaticTask";
    bool place_stack_in_psram_;
    std::unique_ptr<StaticTask_t, CommonDeleter> task_tcb_;
    std::unique_ptr<uint8_t, CommonDeleter> stack_;
};