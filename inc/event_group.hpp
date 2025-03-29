#pragma once

#include "isr.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

class EventGroup {
public:
    EventGroup() : handle_{xEventGroupCreate()} {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        assert(handle_);
    }

    EventBits_t waitForAny(const EventBits_t bits_to_wait, const uint32_t ticks_to_wait,
                           const bool clear = true) {
        return xEventGroupWaitBits(handle_, bits_to_wait, static_cast<BaseType_t>(clear), pdFALSE,
                                   ticks_to_wait);
    }

    EventBits_t waitForAll(const EventBits_t bits_to_wait, const uint32_t ticks_to_wait,
                           const bool clear = true) {
        return xEventGroupWaitBits(handle_, bits_to_wait, static_cast<BaseType_t>(clear), pdTRUE,
                                   ticks_to_wait);
    }

    EventBits_t setBits(const EventBits_t bits_to_set) {
        EventBits_t ret = 0;
        if (isInIsr()) {
            ret = xEventGroupSetBitsFromISR(handle_, bits_to_set, NULL);
        } else {
            ret = xEventGroupSetBits(handle_, bits_to_set);
        }
        return ret;
    }

    EventBits_t clearBits(const EventBits_t bits_to_clear) {
        EventBits_t ret = 0;
        if (isInIsr()) {
            ret = xEventGroupClearBitsFromISR(handle_, bits_to_clear);
        } else {
            ret = xEventGroupClearBits(handle_, bits_to_clear);
        }
        return ret;
    }

    [[nodiscard]] EventBits_t bits() const {
        EventBits_t ret = 0;
        if (isInIsr()) {
            ret = xEventGroupGetBitsFromISR(handle_);
        } else {
            ret = xEventGroupGetBits(handle_);
        }
        return ret;
    }

    [[nodiscard]] EventGroupHandle_t raw() const {
        return handle_;
    }

    template <const uint8_t bit_index> static constexpr EventBits_t bitToBits() {
        static_assert(bit_index <= 31U, "invalid bit index");
        return static_cast<EventBits_t>(1UL << bit_index);
    }

private:
    EventGroupHandle_t handle_{nullptr};
};
