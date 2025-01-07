
/* --------------- INCLUDE SECTION ---------------- */
#include "self_arduino.hpp"
#include "WiFi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "BCPU_wifi_server.hpp"
#include "BCPU_functions.hpp"
#include "BCPU_ramPrograms.hpp"

#include <HTTPClient.h>

/* ---------------- DEFINES / CONSTANTS ---------------- */

/* ---------------- HELP FUNCTION DECLARATION SECTION ---------------- */
void loopTask(void *pvParameters);
void mclkTask(void *pvParameters);
void selProgTask(void *pvParameters);

/* ---------------- GLOBEL VARIABLE SECTION ---------------- */

/* ---------------- RTOS SETUP SECTION ---------------- */
#ifndef CONFIG_ARDUINO_LOOP_STACK_SIZE
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 8192
#endif

TaskHandle_t loopTaskHandle = NULL;
TaskHandle_t mclkTaskHandle = NULL;
TaskHandle_t selProgTaskHandle = NULL;

bool loopTaskWDTEnabled;

extern "C" void app_main()
{
    loopTaskWDTEnabled = false;
    initArduino();

    xTaskCreateUniversal(loopTask, "loopTask", CONFIG_ARDUINO_LOOP_STACK_SIZE, NULL, 1, &loopTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreateUniversal(mclkTask, "mclkTask", CONFIG_ARDUINO_LOOP_STACK_SIZE, NULL, 1, &mclkTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreateUniversal(selProgTask, "selProgTask", CONFIG_ARDUINO_LOOP_STACK_SIZE, NULL, 1, &selProgTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);

    vTaskSuspend(selProgTaskHandle);
}


/* ---------------- RTOS TASK SECTION ---------------- */
void loopTask(void *pvParameters)
{
  /* main setup */
  Serial.begin(9600);

  initPins();
  newProg = prog_FibonacciSeries;
  vTaskResume(selProgTaskHandle);

  initWifi(selProgTaskHandle);
  initServer();

  Serial.println("Setup done");

  /* main loop */
  for(;;) 
   {
    if(loopTaskWDTEnabled)
    {
      esp_task_wdt_reset();
    }
    if (serialEventRun) serialEventRun();
  }
}

void mclkTask(void *pvParameters)
{
  pinMode(23, OUTPUT);

  for(;;)
  {
    if(freqDelay() > 0)
    {
      vTaskDelay(freqDelay());
      digitalWrite(23, !digitalRead(23));
    }
    else
    {
      vTaskDelay(2000);
    }
  }
}

void selProgTask(void *pvParameters)
{
  initPins();
  
  for(;;)
  {
    if(newProg != nullptr)
    {
      lockProgramming();
      programRAM(newProg, (uint8_t) RAM_SIZE);
      releaseProgramming();
    }

    vTaskSuspend(NULL);
  }
}

/* ---------------- HELP FUNCTION SECTION ---------------- */



