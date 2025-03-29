#include "task.hpp"
#include "esp_log.h"

void BaseTask::runTask(void *arg) {
    auto *task = static_cast<BaseTask *>(arg);
    task->setRunning(true);
    task->run();
    task->setRunning(false);
    vTaskDelete(nullptr);
}

void Task::start() {
    if (isRunning()) {
        ESP_LOGE(kTag, "There might be a task with name: %s already running!", name().c_str());
        abort();
    }

    auto res = xTaskCreatePinnedToCore(runTask, name().c_str(), stackSize(), this, priority(),
                                       &task_handle_, core());

    assert(pdTRUE == res);
    assert(task_handle_);
}

void StaticTask::start() {
    if (isRunning()) {
        ESP_LOGE(kTag, "There might be a task with name: %s already running!", name().c_str());
        abort();
    }

    uint32_t malloc_cap = place_stack_in_psram_ ? MALLOC_CAP_SPIRAM : MALLOC_CAP_INTERNAL;
    stack_.reset(static_cast<uint8_t *>(heap_caps_malloc(stackSize(), malloc_cap)));
    task_tcb_.reset(
        static_cast<StaticTask_t *>(heap_caps_malloc(sizeof(StaticTask_t), MALLOC_CAP_INTERNAL)));

    task_handle_ = xTaskCreateStaticPinnedToCore(runTask, name().c_str(), stackSize(), this,
                                                 priority(), stack_.get(), task_tcb_.get(), core());

    assert(task_handle_);
}