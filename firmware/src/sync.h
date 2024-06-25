#ifndef SYNC_H
#define SYNC_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Semaphore to handle synchronization between tasks that use I2C, SPI, RMT peripherals, etc. where
// there seem to be interrupt priority conflicts with only a single core on the ESP32-S2
extern SemaphoreHandle_t peripheralSync;

#endif // SYNC_H