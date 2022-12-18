// This sketch is to test the touch controller, nothing is displayed
// on the TFT.  The TFT_eSPI library must be configured to suit your
// pins used. Make sure both the touch chip select and the TFT chip
// select are correctly defined to avoid SPI bus contention.

// Make sure you have defined a pin for the touch controller chip
// select line in the user setup file or you will see "no member"
// compile errors for the touch functions!

// It is a support and diagnostic sketch for the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

// The "raw" (unprocessed) touch sensor outputs are sent to the
// serial port. Touching the screen should show changes to the x, y
// and z values. x and y are raw ADC readings, not pixel coordinates.

#include "FreeRTOS.h"  // FreeRTOS利用時必須
#include <Arduino.h>

#include "TFT.h"
#include "DBG.h"
#include "TaskControl.h"

#define QUEUE_LENGTH 30

TFT_eSPI tft = TFT_eSPI();

//====================================================================

QueueHandle_t xQueueServer;
QueueHandle_t xQueueSD;
EventGroupHandle_t event_ntp;
EventGroupHandle_t event_ntp_bme280;
EventGroupHandle_t event_ntp_disp;
EventGroupHandle_t event_SendToch;
xSemaphoreHandle mux_DBG;
xSemaphoreHandle mux_QueueDisp;

stStrMessage disp_messege_string[5] =
{
  { DISP_SD_MOUNT_FAIL, "Card Mount Failed"},
  { DISP_SD_ATTACH_FAIL, "No SD card attached"},
  { DISP_SD_OPEN_FAIL, "Failed to open file for reading"},
  { DISP_RTOS_QUEUE_START_FAIL, "rtos queue send error, stopped"},
  { DISP_SD_FAIL, "Card Mount Failed"}
};

void setup(void) {
  Serial.begin(115200);

  mux_DBG = xSemaphoreCreateMutex();
  mux_QueueDisp = xSemaphoreCreateMutex();

  DBG("\n\nStarting...");
  QueueHandle_t xQueue;

  // vRecvDisplayTask用のキュー
  xQueue = xQueueCreate( QUEUE_LENGTH, sizeof(stTaskMessage));
  if ( NULL == xQueue ) {
    // キュー作成失敗、処理停止
    while (1) {
      DBG("xQueue Create Error. Stop.  \n");
      delay(1000);
    }
  }
  // vRecvSDTask用のキュー
  xQueueSD = xQueueCreate( QUEUE_LENGTH, sizeof(TaskSDMessage));
  if ( NULL == xQueue ) {
    // キュー作成失敗、処理停止
    while (1) {
      DBG("xQueueSD Create Error. Stop.  \n");
      delay(1000);
    }
  }
  // vRecvServerTask用のキュー
  xQueueServer = xQueueCreate( QUEUE_LENGTH, sizeof(TaskServerMessage));
  if ( NULL == xQueue ) {
    // キュー作成失敗、処理停止
    while (1) {
      DBG("xQueueServer Create Error. Stop.  \n");
      delay(1000);
    }
  }
  delay(200);
  event_ntp = xEventGroupCreate( );
  event_ntp_bme280 = xEventGroupCreate( );
  event_ntp_disp = xEventGroupCreate( );
  event_SendToch = xEventGroupCreate( );

  DBG("xTaskCreatePinnedToCore tasks vRecvDisplayTask \n");
  // 受信表示タスク登録。プライオリティ5
  xTaskCreatePinnedToCore(
    vRecvDisplayTask,     /* task name */
    "TaskDisp",           /* task name string */
    10000,
    (void *) &xQueue,      /* The queue handle is used as the task parameter */
    6,                    /* priority */
    (TaskHandle_t *) NULL, /* task handle pointer */
    0                     /* core ID*/
  );

  DBG("xTaskCreatePinnedToCore tasks vRecvServerTask \n");
  // 受信サーバタスク登録。プライオリティ６
  xTaskCreatePinnedToCore(
    vRecvServerTask,     /* task name */
    "TaskRecvServer",           /* task name string */
    3072,
    (void *)NULL,      /* The queue handle is used as the task parameter */
    6,                    /* priority */
    (TaskHandle_t *) NULL, /* task handle pointer */
    0                     /* core ID*/
  );
  DBG("xTaskCreatePinnedToCore tasks vRecvSDTask \n");
  // 受信SDタスク登録。プライオリティ６
  xTaskCreatePinnedToCore(
    vRecvSDTask,     /* task name */
    "TaskRecvSD",           /* task name string */
    8000,
    (void *) &xQueue,      /* The queue handle is used as the task parameter */
    6,                    /* priority */
    (TaskHandle_t *) NULL, /* task handle pointer */
    0                     /* core ID*/
  );
  DBG("tasks registered END\n");

  /*
    DBG("xTaskCreatePinnedToCore vSendReTempTask \n");
    // 送信　気温・湿度の再表示タスク登録。プライオリティ7
    xTaskCreatePinnedToCore(
      vSendReTempTask,
      "TaskReTemp",
      2500,
      (void *) &xQueue,
      7,
      (TaskHandle_t *) NULL,
      0
    );
  */

  DBG("xTaskCreatePinnedToCore vSendBtnTask \n");
  // 送信ボタンタスク登録。プライオリティ7
  xTaskCreatePinnedToCore (
    vSendBtnTask,        /* task name */
    "TaskBtn",           /* task name string */
    1524,
    (void *) &xQueue,     /* The queue handle is used as the task parameter */
    7,                   /* priority */
    (TaskHandle_t *) NULL, /* task handle pointer */
    0                    /* core ID*/
  );

  DBG("xTaskCreatePinnedToCore vSendBME280Task \n");
  // 送信BME280タスク登録。プライオリティ7
  xTaskCreatePinnedToCore (
    vSendBME280Task,        /* task name */
    "TaskBme280",           /* task name string */
    5072,
    (void *) &xQueue,     /* The queue handle is used as the task parameter */
    7,                   /* priority */
    (TaskHandle_t *) NULL, /* task handle pointer */
    1                    /* core ID*/
  );
}

//====================================================================

void loop() {
  delay(5000);
}
//====================================================================
