#ifndef FREERTOS_DBG_H
#define FREERTOS_DBG_H

#include "Arduino.h"
#include "FreeRTOS.h"  // FreeRTOS利用時必須
#include "TaskControl.h"

#define SerialSpeed 115200
#define _DEBUG 1
#if _DEBUG
#define DBG(A) {xSemaphoreTake( mux_DBG, portMAX_DELAY );Serial.print(A); xSemaphoreGive( mux_DBG );}
#define ERR(A) {xSemaphoreTake( mux_DBG, portMAX_DELAY );TaskHandle_t handle = xTaskGetCurrentTaskHandle();char* taskname = pcTaskGetTaskName(handle);Serial.print(taskname);Serial.print("  ");Serial.print(A); xSemaphoreGive( mux_DBG );}
void DBG2( const char *format, ... );
#else
#define DBG(A) {}
void DBG2( const char *format, ... );
#endif /* _DBG */
#endif /* FREERTOS_DBG_H */
