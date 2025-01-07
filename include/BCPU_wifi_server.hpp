#ifndef BCPU_WIFI_SERVER
#define BCPU_WIFI_SERVER

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

void initWifi(TaskHandle_t task_selProg);
void initServer(void);

int freqDelay(void);

#endif /* BCPU_WIFI_SERVER */