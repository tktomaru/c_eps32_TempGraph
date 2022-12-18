#include "FreeRTOS.h"  // FreeRTOS利用時必須
#include <Arduino.h>
#include "TaskControl.h"
#include "DBG.h"

int GetBTN( stBtn *startBtn, stBtn *nowBtn)
{
  int ret = IS_NONE;

  //ret = IS_EXSIST;
  return ret;
}

void vSendBtnTask( void *pvParameters )
{
  QueueHandle_t xQueue = *((QueueHandle_t*)pvParameters);
  BaseType_t xStatus;
  int ret = 0;
  stTaskMessage xMessage;
  stTaskMessage *pxMessage;
  stBtn beforeBtn;
  stBtn nowBtn;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);// ms

  DBG("start vSendBtnTask \n");

  while (1)
  {
    ret = IS_NONE;
    while ( ret == IS_NONE ) {
      ret = GetBTN( &beforeBtn, &nowBtn );
      delay(10);
    }

    xMessage.iMessageID = TASK_BTN;
    pxMessage = &xMessage;
      DBG("xQueueSend : BTN");
    xSemaphoreTake( mux_QueueDisp, portMAX_DELAY );
    xStatus = xQueueSend(xQueue, pxMessage, xTicksToWait);
    xSemaphoreGive( mux_QueueDisp );

    if ( xStatus != pdPASS )
    {
      while (1)
      {
        ERR("rtos queue send error, stopped \n");
        delay(1000);
      }
    }
    delay(10);
  }
}
