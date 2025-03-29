#pragma once

#include "freertos/FreeRTOSConfig.h"
#include "freertos/portmacro.h"

inline bool isInIsr() {
    return static_cast<bool>(xPortInIsrContext());
}