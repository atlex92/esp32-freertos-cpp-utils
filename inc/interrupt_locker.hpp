#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>

class InterruptLocker {
public:
    InterruptLocker() {
        time_critical_mutex_ = portMUX_INITIALIZER_UNLOCKED;
        portENTER_CRITICAL(&time_critical_mutex_);
    }
    ~InterruptLocker() {
        portEXIT_CRITICAL(&time_critical_mutex_);
    }
    InterruptLocker(const InterruptLocker &) = default;
    InterruptLocker(InterruptLocker &&) = delete;
    InterruptLocker &operator=(const InterruptLocker &) = default;
    InterruptLocker &operator=(InterruptLocker &&) = delete;

private:
    portMUX_TYPE time_critical_mutex_{};
};
