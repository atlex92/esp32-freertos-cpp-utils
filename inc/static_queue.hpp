#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "isr.hpp"
#include <cstdint>
#include <string>

template <typename T, size_t length> class StaticQueue {
public:
    StaticQueue(const char *name = "")
          : queue_handle_{xQueueCreateStatic(length, sizeof(T), queue_storage_,
                                             &queue_struct_data_)},
            name_(name) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        assert(nullptr != queue_handle_);
    }

    StaticQueue(const StaticQueue &) = delete;
    StaticQueue(StaticQueue &&) = delete;
    StaticQueue &operator=(const StaticQueue &) = delete;
    StaticQueue &operator=(StaticQueue &&) = delete;

    void setName(const char *name) {
        name_ = name;
    }

    [[nodiscard]] std::string name() const {
        return name_;
    }

    template <typename U> bool enqueueBack(U &msg, const uint32_t timeout_ms = 0U) {
        return isInIsr() ? xQueueSendToBackFromISR(queue_handle_, &msg, NULL)
                         : xQueueSendToBack(queue_handle_, &msg, pdMS_TO_TICKS(timeout_ms));
    }

    template <typename U> bool enqueueFront(U &msg, const uint32_t timeout_ms = 0U) {
        return isInIsr() ? xQueueSendToFrontFromISR(queue_handle_, &msg, NULL)
                         : xQueueSendToFront(queue_handle_, &msg, pdMS_TO_TICKS(timeout_ms));
    }

    bool peek(T &out, const uint32_t timeout_ms = 0) {
        auto result = isInIsr() ? xQueuePeekFromISR(queue_handle_, &out)
                                : xQueuePeek(queue_handle_, &out, pdMS_TO_TICKS(timeout_ms));
        return result;
    }

    bool receive(T &out, const uint32_t timeout_ms = 0) {
        auto result = isInIsr() ? xQueueReceiveFromISR(queue_handle_, &out, NULL)
                                : xQueueReceive(queue_handle_, &out, pdMS_TO_TICKS(timeout_ms));

        return result;
    }

    [[nodiscard]] size_t size() const {
        return uxQueueMessagesWaiting(queue_handle_);
    }

    void reset() {
        xQueueReset(queue_handle_);
    }

    [[nodiscard]] size_t available() const {
        return uxQueueSpacesAvailable(queue_handle_);
    }

    ~StaticQueue() {
        if (queue_handle_ != nullptr) {
            vQueueDelete(queue_handle_);
        }
    }

    [[nodiscard]] QueueHandle_t raw() const {
        return queue_handle_;
    }

private:
    StaticQueue_t queue_struct_data_{};
    uint8_t queue_storage_[length * sizeof(T)]{};
    QueueHandle_t queue_handle_{nullptr};
    std::string name_;
};
