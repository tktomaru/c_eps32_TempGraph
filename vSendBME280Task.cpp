#include "FreeRTOS.h"  // FreeRTOS利用時必須
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_BME280.h"
#include "TaskControl.h"
#include "DBG.h"

#define I2C_SDA 32
#define I2C_SCL 33
#define BME_AC 35
#define BME280_ADD 0x76
Adafruit_BME280 bme280((uint8_t)I2C_SDA, (uint8_t)I2C_SCL);

void vSendBME280Task( void *pvParameters )
{
  QueueHandle_t xQueue = *((QueueHandle_t*)pvParameters);
  BaseType_t xStatus;
  int ret = 0;
  stTaskMessage xMessage;
  stTaskMessage *pxMessage;
  stTaskServerMessage xServerMessage;
  stTaskServerMessage *pxServerMessage;
  stBtn beforeBtn;
  stBtn nowBtn;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);// ms
  bool status;
  stBME280 stbme280, stbme_server280;
  EventBits_t uxBits;
  const TickType_t xTicksToStartWait = pdMS_TO_TICKS(10 * 1000); // ms

  pinMode(BME_AC, OUTPUT);
  digitalWrite(BME_AC, HIGH);
  delay(1000);
  status = bme280.begin(BME280_ADD);
  if (!status) {
    DBG("Could not find a valid BME280 sensor, check wiring!");
    delay(100);
    digitalWrite(BME_AC, LOW);
    delay(2000);
    digitalWrite(BME_AC, HIGH);
    delay(3000);
  }
  uxBits = xEventGroupWaitBits(
             event_ntp_bme280,     /* The event group being tested. */
             BIT_0        , /* The bits within the event group to wait for. */
             pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
             pdFALSE,       /* Don't wait for both bits, either bit will do. */
             xTicksToStartWait );/* Wait a maximum of 100ms for either bit to be set. */
  DBG("start vSendBME280Task \n");

  while (1)
  {
    stbme280.temp = bme280.readTemperature();
    stbme280.humid = bme280.readHumidity();
    stbme280.pressure = bme280.readPressure();
    if ( !(stbme280.temp && stbme280.humid && stbme280.temp) ||
         isnan(stbme280.temp) || isnan(stbme280.humid) || isnan(stbme280.temp) ||
         stbme280.humid >= 100 || stbme280.temp >= 50 || stbme280.temp <= -10) {
      DBG("restart BME280 \n");
      digitalWrite(BME_AC, LOW);
      delay(2000);
      digitalWrite(BME_AC, HIGH);
      delay(3000);
      status = bme280.begin(BME280_ADD);
      if (!status) {
        ERR("Could not find a valid BME280 sensor, check wiring!");
      }
    } else {
      xMessage.iMessageID = TASK_BME280;
      xMessage.Data.BME280 = stbme280;
      pxMessage = &xMessage;
      DBG("xQueueSend : BME280 \n");

      xSemaphoreTake( mux_QueueDisp, portMAX_DELAY );
      xStatus = xQueueSend(xQueue, pxMessage, xTicksToWait);
      xSemaphoreGive( mux_QueueDisp );

      if ( xStatus != pdPASS ) {
        while (1) {
          ERR("rtos queue send error, stopped \n");
          delay(1000);
        }
      }

      stbme_server280 = stbme280;
      DBG("xQueueServer : BME280 \n");
      xServerMessage.iMessageID = TASK_SERVER_BME280;
      xServerMessage.Data.BME280 = stbme_server280;
      pxServerMessage = &xServerMessage;
      xStatus = xQueueSend(xQueueServer, pxServerMessage, xTicksToWait);

      if ( xStatus != pdPASS ) {
        while (1) {
          ERR("rtos queue send error, stopped \n");
          delay(1000);
        }
      }

    }
    delay(1000 * 30); // 30sec
  }
}
