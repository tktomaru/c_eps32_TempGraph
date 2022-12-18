#include "FreeRTOS.h"  // FreeRTOS利用時必須
#include <Arduino.h>
#include "TaskControl.h"
#include "DBG.h"
#include "TFT.h"
#include <TimeLib.h>                // for use now()
extern "C" {
#include "CTime.h"
}

double DispField[7][24][3];
char DispDate[7][32];

void drawtextxy(int x, int y, char *text, uint16_t color) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  //tft.setTextWrap(false);
  tft.print(text);
}

enum screen_center {
  SCREEN_CENTER_DOWN ,
  SCREEN_CENTER_UP ,
  SCREEN_RIGHT_UP ,
  SCREEN_RIGHT_DOWN ,
  SCREEN_LEFT_UP ,
  SCREEN_LEFT_DOWN,
};

void drawGraphLine()
{
  drawtextxy(40 + 10 * 0, 240 - 9, "0", TFT_WHITE);
  //drawtextxy(40 + 10 * 1, 240 - 9, "1", TFT_WHITE);
  //drawtextxy(40 + 10 * 2, 240 - 9, "2", TFT_WHITE);
  //drawtextxy(40 + 10 * 3, 240 - 9, "3", TFT_WHITE);
  //drawtextxy(40 + 10 * 4, 240 - 9, "4", TFT_WHITE);
  //drawtextxy(40 + 10 * 5, 240 - 9, "5", TFT_WHITE);
  drawtextxy(40 + 10 * 6, 240 - 9, "6", TFT_WHITE);
  //drawtextxy(40 + 10 * 7, 240 - 9, "7", TFT_WHITE);
  //drawtextxy(40 + 10 * 8, 240 - 9, "8", TFT_WHITE);
  //drawtextxy(40 + 10 * 9, 240 - 9, "9", TFT_WHITE);
  //drawtextxy(40 + 10 * 10, 240 - 9, "10", TFT_WHITE);
  //drawtextxy(40 + 10 * 11, 240 - 9, "11", TFT_WHITE);
  drawtextxy(40 + 10 * 12, 240 - 9, "12", TFT_WHITE);
  //drawtextxy(40 + 10 * 13, 240 - 9, "13", TFT_WHITE);
  //drawtextxy(40 + 10 * 14, 240 - 9, "14", TFT_WHITE);
  //drawtextxy(40 + 10 * 15, 240 - 9, "15", TFT_WHITE);
  //drawtextxy(40 + 10 * 16, 240 - 9, "16", TFT_WHITE);
  //drawtextxy(40 + 10 * 17, 240 - 9, "17", TFT_WHITE);
  drawtextxy(40 + 10 * 18, 240 - 9, "18", TFT_WHITE);
  //drawtextxy(40 + 10 * 19, 240 - 9, "19", TFT_WHITE);
  //drawtextxy(40 + 10 * 20, 240 - 9, "20", TFT_WHITE);
  //drawtextxy(40 + 10 * 21, 240 - 9, "21", TFT_WHITE);
  //drawtextxy(40 + 10 * 22, 240 - 9, "22", TFT_WHITE);
  //drawtextxy(40 + 10 * 23, 240 - 9, "23", TFT_WHITE);
  drawtextxy(40 + 10 * 24, 240 - 9, "24", TFT_WHITE);

  drawtextxy(0, 240 - 9, "0", TFT_WHITE);
  drawtextxy(0, 240 - 9 - 30 * 1, "10", TFT_WHITE);
  drawtextxy(0, 240 - 9 - 30 * 2, "20", TFT_WHITE);
  drawtextxy(0, 240 - 9 - 30 * 3, "30", TFT_WHITE);
  drawtextxy(0, 240 - 9 - 30 * 4, "40", TFT_WHITE);
  tft.drawLine(20, 240 - 10, 320, 230, TFT_WHITE);
  tft.drawLine(20, 240 - 25         , 320, 230 - 15 * 1, TFT_WHITE);
  tft.drawLine(20, 240 - 10 - 30 * 1, 320, 240 - 10 - 30 * 1, TFT_WHITE);
  tft.drawLine(20, 240 - 25 - 30 * 1, 320, 240 - 25 - 30 * 1, TFT_WHITE);
  tft.drawLine(20, 240 - 10 - 30 * 2, 320, 240 - 10 - 30 * 2, TFT_WHITE);
  tft.drawLine(20, 240 - 25 - 30 * 2, 320, 240 - 25 - 30 * 2, TFT_WHITE);
  tft.drawLine(20, 240 - 10 - 30 * 3, 320, 240 - 10 - 30 * 3, TFT_WHITE);
  tft.drawLine(20, 240 - 25 - 30 * 3, 320, 240 - 25 - 30 * 3, TFT_WHITE);
  tft.drawLine(20, 240 - 10 - 30 * 4, 320, 240 - 10 - 30 * 4, TFT_WHITE);
  /*
    tft.drawLine(20, 240 - 10, 320, 230, TFT_WHITE);
    tft.drawLine(20, 240 - 10 - 15 * 1, 320, 230 - 15 * 1, TFT_WHITE);
    tft.drawLine(20, 240 - 10 - 30 * 1, 320, 240 - 10 - 30 * 1, TFT_WHITE);
    tft.drawLine(20, 240 - 25 - 30 * 1, 320, 240 - 25 - 30 * 1, TFT_WHITE);
    tft.drawLine(20, 240 - 10 - 30 * 2, 320, 240 - 10 - 30 * 2, TFT_WHITE);
    tft.drawLine(20, 240 - 25 - 30 * 2, 320, 240 - 25 - 30 * 2, TFT_WHITE);
    tft.drawLine(20, 240 - 10 - 30 * 3, 320, 240 - 10 - 30 * 3, TFT_WHITE);
    tft.drawLine(20, 240 - 25 - 30 * 3, 320, 240 - 25 - 30 * 3, TFT_WHITE);
    tft.drawLine(20, 240 - 10 - 30 * 4, 320, 240 - 10 - 30 * 4, TFT_WHITE);
    vTaskDelay(10);
  */
}


int GetScreenPoint( int x, int y, int z) {
  if ( 8 <= z ) {
    // 弱い押下検出
    if (8 >= x ) {
      // X下
      if ( 10 >= y ) {
        // Y右
        return SCREEN_RIGHT_DOWN;
      }
      if ( 16 <= y && 23 >= y) {
        // Y真ん中
        return SCREEN_CENTER_DOWN;
      }
      if ( 32 <= y) {
        // Y左
        return SCREEN_LEFT_DOWN;
      }
    }
    if ( 33 <= x ) {
      // 上
      if ( 10 >= y ) {
        // Y右
        return SCREEN_RIGHT_UP;
      }
      if ( 16 <= y && 23 >= y) {
        // Y真ん中
        return SCREEN_CENTER_UP;
      }
      if ( 32 <= y) {
        // Y左
        return SCREEN_LEFT_UP;
      }
    }
  }
  return -1;
}

int gosa( int value, int old_value, int gosa)
{
  int before = old_value - gosa;
  int after = old_value + gosa;
  //Serial.printf("before %d ", before );Serial.printf("after %d \n", after );
  if ( ( before <= value) && (value <= after))
  {
    return 0;
  }
  return 1;
}

int GetDisplayXYZ( TFT_eSPI tft, stXYZ *start, stXYZ *_now, int *_input_count)
{
  int ret = IS_NONE;
  uint16_t x = 0, y = 0, z = 0;
  uint16_t gx, gy, gz; //
  int input_count = *_input_count;

  int sx = (*start).iX;
  int sy = (*start).iY;
  int sz = (*start).iZ;
  tft.getTouchRaw(&x, &y);
  z = tft.getTouchRawZ();
  if ( x < 510 ) {
    x = 0;
  }
  if ( y > 3700 ) {
    y = 3900;
  }
  if ( z <= 200 ) {
    z = 0;
  }

  gx = gosa( x, sx, 100 );
  gy = gosa( y, sy, 500);
  gz = gosa( z, sz, 100);
  if ( gx || gy || gz ) {
    if ( input_count == 0 ) {
      (*start).iX = x;
      (*start).iY = y;
      (*start).iZ = z;
    }

    if ( input_count <= 5 ) {
      input_count = input_count + 1;
      //Serial.printf("not output\n");
    } else {
      DBG("!!output!!\n");
      input_count = 0;
      DBG2("x %i   ", gx );
      DBG2("y %i   ", gy );
      DBG2("z %i \n", gz );

      DBG2("x: %i     ", x);
      DBG2("y: %i     ", y);
      DBG2("z: %i \n", z);

      //tft.fillScreen(random(0xFFFF));
      x = x / 100;
      y = y / 100;
      z = z / 100;

      //tft.setCursor(0, 0, 2);
      //tft.print("x = "); tft.print(x);  tft.println("    ");
      //tft.print("y = "); tft.print(y);  tft.println("    ");
      //tft.print("z = "); tft.print(z);  tft.print("    ");

      ret = IS_EXSIST;
    }
  } else {
    input_count = 0;
  }
  *_input_count = input_count;
  (*_now).iX = x;
  (*_now).iY = y;
  (*_now).iZ = z;
  return ret;
}

char disp_buffer[6][64];

void DispBMP280(stBME280 now_bme) {
  tft.setTextSize(4);
  DBG("temp :    ");
  DBG(now_bme.temp);
  DBG("\n");
  drawtextxy(50, 0, &disp_buffer[3][0], TFT_BLACK);
  memset( &disp_buffer[3][0], 0x00, sizeof(disp_buffer[0]));
  sprintf(&(disp_buffer[3][0]), "%.2lf C", now_bme.temp);
  delay(10);
  drawtextxy(50, 0, &disp_buffer[3][0], TFT_WHITE);
  DBG("humid :   ");
  DBG(now_bme.humid);
  DBG("\n");
  drawtextxy(50, 60, &disp_buffer[4][0], TFT_BLACK);
  memset( &disp_buffer[4][0], 0x00, sizeof(disp_buffer[0]));
  sprintf(&(disp_buffer[4][0]), "%.2lf", now_bme.humid);
  delay(10);
  drawtextxy(50, 60, &disp_buffer[4][0], TFT_WHITE);
  DBG("pressure: ");
  DBG(now_bme.pressure);
  DBG("\n");

  tft.setTextSize(1);
  drawtextxy(0, 100, &disp_buffer[5][0], TFT_BLACK);
  memset( &disp_buffer[5][0], 0x00, sizeof(disp_buffer[0]));
  sprintf(&(disp_buffer[5][0]), "%.2lf hPa", now_bme.pressure);
  delay(10);
  drawtextxy(0, 100, &disp_buffer[5][0], TFT_WHITE);
}

void vRecvDisplayTask( void *pvParameters )
{
  QueueHandle_t xQueue = *((QueueHandle_t*)pvParameters);
  BaseType_t xStatus;
  int ret = 0;
  stTaskMessage xMessage = {0};
  stTaskMessage *pxMessage;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(30);// ms
  int ii, jj, kk;
  int before_disp_str_id = -1;
  int before_day = -1;
  unsigned long tik;
  struct tm tm_now;
  int graph_day = 0;
  int screen_point = 0;
  stXYZ stbefore;
  stXYZ stnow;
  int input_count = 0;
  unsigned long before_tik = 0;
  bool next_flg = 0;
  stBME280 now_bme;
  now_bme.temp = 0;
  now_bme.humid = 0;
  now_bme.pressure = 0;
  EventBits_t uxBits;
  const TickType_t xTicksToStartWait = pdMS_TO_TICKS(10 * 1000); // ms

  DBG("start vRecvDisplayTask \n");

  DBG("start initialize TFT_eSPI \n");
  tft.init();
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  DBG("end initialize TFT_eSPI \n");


  uxBits = xEventGroupWaitBits(
             event_ntp,     /* The event group being tested. */
             BIT_0        , /* The bits within the event group to wait for. */
             pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
             pdFALSE,       /* Don't wait for both bits, either bit will do. */
             xTicksToStartWait );/* Wait a maximum of 100ms for either bit to be set. */


  tik = now();
  tm_now = unix_time_to_date(tik);
  before_day = tm_now.tm_yday;

  drawGraphLine();
  while (1)
  {
    xStatus = xQueueReceive( xQueue, &xMessage, xTicksToWait );

    if ( xStatus != pdPASS ) {
      while (1) {
        //DBG("TASK_GET_TOUCH"); DBG("\n");
        ret = GetDisplayXYZ(tft, &stbefore, &stnow, &input_count );
        if ( ret == IS_EXSIST) {
          drawtextxy(0, 0, &disp_buffer[0][0], TFT_BLACK);
          memset( &disp_buffer[0][0], 0x00, sizeof(disp_buffer[0]));
          sprintf(disp_buffer[0], "x=%d", stnow.iX);
          drawtextxy(0, 0,  &disp_buffer[0][0], TFT_WHITE);

          drawtextxy(0, 20, &disp_buffer[1][0], TFT_BLACK);
          memset(  disp_buffer[1], 0x00, sizeof(disp_buffer[0]));
          sprintf(disp_buffer[1], "y=%d", stnow.iY);
          drawtextxy(0, 20,  &disp_buffer[1][0], TFT_WHITE);

          drawtextxy(0, 40, disp_buffer[2], TFT_BLACK);
          memset(  disp_buffer[2], 0x00, sizeof(disp_buffer[0]));
          sprintf(disp_buffer[2], "z=%d", stnow.iZ);
          drawtextxy(0, 40,  &(disp_buffer[2][0]), TFT_WHITE);

          DBG("x="); DBG(stnow.iX); DBG("  ");
          DBG("y="); DBG(stnow.iY); DBG("  ");
          DBG("z="); DBG(stnow.iZ); DBG("\n");

          screen_point = GetScreenPoint( stnow.iX, stnow.iY, stnow.iZ);
          if ( -1 == screen_point ) {
            break;
          }
          next_flg = 0;
          switch ( screen_point ) {
            case SCREEN_LEFT_DOWN:
              // TODO 温度と湿度の表示切り替え
              break;
            case SCREEN_CENTER_DOWN:
              // グラフの日付切り替え
              DBG("SCREEN_CENTER_DOWN"); DBG("\n");
              next_flg = 1;
              drawtextxy(100, 100, DispDate[graph_day], TFT_BLACK);
              graph_day = graph_day - 1;
              if (-1 >= graph_day) {
                graph_day = 6;
              }
              drawtextxy(100, 100, DispDate[graph_day], TFT_WHITE);
              break;
            case SCREEN_CENTER_UP:
              // グラフの日付切り替え
              DBG("SCREEN_CENTER_UP"); DBG("\n");
              next_flg = 1;
              drawtextxy(100, 100, DispDate[graph_day], TFT_BLACK);
              graph_day = graph_day + 1;
              if (7 <= graph_day) {
                graph_day = 0;
              }
              drawtextxy(100, 100, DispDate[graph_day], TFT_WHITE);
              break;
          }
          if ( 0 == next_flg ) {
          } else {
            xStatus = pdPASS;
            xMessage.iMessageID = TASK_INIT_GRAPH;
          }
        }
        break;
      }
    }
    if ( xStatus == pdPASS )
    {
      switch ( xMessage.iMessageID ) {
        case TASK_BTN:
          DBG("DISP_BTN"); DBG("\n");
          break;
        case TASK_BME280:
          DBG("DISP_BME"); DBG("\n");

          now_bme = xMessage.Data.BME280;

          DispBMP280(now_bme);

          tik = now();
          tm_now = unix_time_to_date(tik);

          drawtextxy(100, 100, DispDate[graph_day], TFT_BLACK);
          if ( before_day != tm_now.tm_yday ) {
            memcpy( DispField[6], DispField[5], sizeof(DispField[0]));
            memcpy( DispField[5], DispField[4], sizeof(DispField[0]));
            memcpy( DispField[4], DispField[3], sizeof(DispField[0]));
            memcpy( DispField[3], DispField[2], sizeof(DispField[0]));
            memcpy( DispField[2], DispField[1], sizeof(DispField[0]));
            memcpy( DispField[1], DispField[0], sizeof(DispField[0]));
            memset( DispField[0], 0x00, sizeof(DispField[0]));
            before_day = tm_now.tm_yday;

            memcpy( DispDate[6], DispDate[5], sizeof(DispDate[0]));
            memcpy( DispDate[5], DispDate[4], sizeof(DispDate[0]));
            memcpy( DispDate[4], DispDate[3], sizeof(DispDate[0]));
            memcpy( DispDate[3], DispDate[2], sizeof(DispDate[0]));
            memcpy( DispDate[2], DispDate[1], sizeof(DispDate[0]));
            memcpy( DispDate[1], DispDate[0], sizeof(DispDate[0]));
            sprintf(DispDate[0], "%04d/%02d/%02d", tm_now.tm_year, tm_now.tm_mon, tm_now.tm_yday);
          }
          drawtextxy(100, 100, DispDate[graph_day], TFT_WHITE);

          // 日付の時間位置に出力
          double dtmp;
          for ( ii = 0; ii < 3; ii++) {
            switch ( ii ) {
              case 0: dtmp = xMessage.Data.BME280.temp; break;
              case 1: dtmp = xMessage.Data.BME280.humid; break;
              case 2: dtmp = xMessage.Data.BME280.pressure; break;
              default: dtmp = 0; break;
            }
            if ( DispField[0][tm_now.tm_hour][ii] ) {
              DispField[0][tm_now.tm_hour][ii] = (DispField[0][tm_now.tm_hour][ii] + dtmp) / 2;
            } else {
              DispField[0][tm_now.tm_hour][ii] = dtmp;
            }
          }
        case TASK_INIT_GRAPH:
          DBG("TASK_INIT_GRAPH"); DBG("\n");
          // 0 to 23
          drawtextxy(100, 100, DispDate[graph_day], TFT_WHITE);
          tft.fillRect( 0, tft.height() / 2 , tft.width(), tft.height() / 2 , TFT_BLACK);
          delay(2);
          drawGraphLine();
          delay(2);
          for (ii = 0; ii < (sizeof(DispField[0]) / sizeof(DispField[0][0])); ii++) {
            // 0 to 9
            for (jj = 0; jj < (sizeof(DispField[0][0]) / sizeof(double)); jj++) {
              if ( jj == 0 ) {
                tft.fillRect( ii * 10 + 40 , 240 - DispField[graph_day][ii][jj] * 3 - 9, 8, DispField[graph_day][ii][jj] * 3 , TFT_WHITE);
                delay(2);
                tft.fillRect( ii * 10 + 40 , 240 - DispField[graph_day][ii][jj] * 3 - 9, 8, DispField[graph_day][ii][jj] * 3, TFT_WHITE);
                delay(2);
              }
            }
          }
          break;
        case TASK_RE_TEMP:
          DBG("TASK_RE_TEMP"); DBG("\n");
          DispBMP280(now_bme);
          break;
        case TASK_STRING:
          DBG("TASK_STRING"); DBG("\n");

          tft.fillRect( 0, 0 , tft.width(), tft.height() / 2 , TFT_BLACK);
          delay(2);

          if ( before_disp_str_id >= 0 ) {
            before_disp_str_id = xMessage.Data.strID;
            drawtextxy(0, 0, disp_messege_string[before_disp_str_id].message, TFT_BLACK);
          }
          for ( ii = 0; ii < sizeof(disp_messege_string) / sizeof(disp_messege_string[0]); ii++) {
            if ( disp_messege_string[ii].strID == xMessage.Data.strID ) {
              drawtextxy(0, 0, disp_messege_string[ii].message, TFT_WHITE);
            }
          }
          break;
      }
    } else {
      if (uxQueueMessagesWaiting(xQueue) != 0) {
        while (1) {
          ERR("rtos queue receive error, stopped \n");
          delay(1000);
        }
      }
    }
    delay(5);
  }
}
