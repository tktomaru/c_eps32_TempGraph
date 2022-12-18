#include "FreeRTOS.h"  // FreeRTOS利用時必須
#include <Arduino.h>
#include "TaskControl.h"
#include "DBG.h"

void vSendReTempTask( void *pvParameters )
{
  QueueHandle_t xQueue = *((QueueHandle_t*)pvParameters);
  BaseType_t xStatus;
  int ret = 0;
  stTaskMessage xMessage;
  stTaskMessage *pxMessage;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);// ms
  DBG("start vSendReTempTask \n");

  while (1)
  {
    //DBG("TASK_GET_TOUCH"); DBG("\n");

    DBG("xQueueSend : RE_TEMP \n");
    xMessage.iMessageID = TASK_RE_TEMP;
    pxMessage = &xMessage;
    xSemaphoreTake( mux_QueueDisp, portMAX_DELAY );
    xStatus = xQueueSend(xQueue, pxMessage, xTicksToWait);
    xSemaphoreGive( mux_QueueDisp );

    if ( xStatus != pdPASS ) {
      while (1) {
        ERR("rtos queue send error, stopped \n");
        delay(1000);
      }
    }

    delay(1000 * 60 * 30);//5 sec
  }
}
