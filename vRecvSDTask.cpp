#include "FreeRTOS.h"  // FreeRTOS利用時必須
#include <Arduino.h>
#include "TaskControl.h"
#include "DBG.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <TimeLib.h>                // for use now()
extern "C" {
#include "CTime.h"
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  DBG2("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    DBG("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    DBG("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      DBG("  DIR : ");
      DBG(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      DBG("  FILE: ");
      DBG(file.name()); DBG("\n");
      DBG("  SIZE: ");
      DBG(file.size()); DBG("\n");
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path) {
  DBG2("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    DBG("Dir created"); DBG("\n");
  } else {
    DBG("mkdir failed"); DBG("\n");
  }
}

void removeDir(fs::FS &fs, const char * path) {
  DBG2("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    DBG("Dir removed"); DBG("\n");
  } else {
    DBG("rmdir failed"); DBG("\n");
  }
}

void readFile(fs::FS &fs, const char * path) {
  DBG2("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    DBG("Failed to open file for reading"); DBG("\n");
    return;
  }

  DBG("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

int writeFile(fs::FS &fs, const char * path, const char * message) {
  DBG2("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    DBG("Failed to open file for writing"); DBG("\n");
    return 0;
  }
  if (file.print(message)) {
    DBG("File written"); DBG("\n");
  } else {
    DBG("Write failed"); DBG("\n");
  }
  file.close();
  return 1;
}

int appendFile(fs::FS &fs, const char * path, const char * message) {
  DBG2("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    DBG("Failed to open file for appending"); DBG("\n");
    return 0;
  }
  if (file.print(message)) {
    DBG("Message appended"); DBG("\n");
  } else {
    DBG("Append failed"); DBG("\n");
  }
  file.close();
  return 1;
}

void renameFile(fs::FS &fs, const char * path1, const char * path2) {
  DBG2("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    DBG("File renamed"); DBG("\n");
  } else {
    DBG("Rename failed"); DBG("\n");
  }
}

void deleteFile(fs::FS &fs, const char * path) {
  DBG2("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    DBG("File deleted"); DBG("\n");
  } else {
    DBG("Delete failed"); DBG("\n");
  }
}

void testFileIO(fs::FS &fs, const char * path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    DBG2("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    DBG("Failed to open file for reading"); DBG("\n");
  }


  file = fs.open(path, FILE_WRITE);
  if (!file) {
    DBG("Failed to open file for writing"); DBG("\n");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  DBG2("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

#define CHAR_VALUE 1
#define RETURN_VALUE 2
#define COMMA_VALUE 3
#define DATE_VALUE 4

int get_char(File *f, char *output)
{
  char c;
  int ret = -1;
  // 数値、改行、カンマのいづれかがを探す
  while (1) {
    if (f->available() < 1) {
      //DBG("f->available() < 1"); DBG("\n");
      return -1;
    }
    c = f->read();
    //DBG(c);
    if (c == ',') {
      return COMMA_VALUE;
    }
    if (c == '\n') {
      return RETURN_VALUE;
    }
    if ( c == '/' ) {
      while (1) {
        c = f->read();
        if (  c == ',' ) {
          break;
        }
      }
      return DATE_VALUE;
    }
    *output = c;
    return CHAR_VALUE;
  }
}

int get_csv(fs::FS &fs, const char * path, double field[24][3])
{
  int field_row = 0, field_column = 0;
  int ret = 0;
  char inbuffer[255];
  double outbuffer;
  char *ep;
  int ii;
  struct tm tm_now;
  int is_start = 1;
  int point_hour = 0;

  DBG(path); DBG("\n");
  File file = fs.open(path);
  if (!file) {
    DBG("Failed to open file for reading"); DBG("\n");
    return -1;
  }
  do {
    memset(inbuffer, 0x00, sizeof(inbuffer));
    for ( ii = 0; ii < 255; ii++ ) {
      ret = get_char(&file, &inbuffer[ii]);
      if ( (ret == RETURN_VALUE) || (ret == COMMA_VALUE) ) {
        //DBG("(ret == RETURN_VALUE) || (ret == COMMA_VALUE)"); DBG("\n");
        outbuffer = strtod(inbuffer, &ep );
        //DBG(outbuffer); DBG("  ");
        if (outbuffer == 0) {
          DBG("csv convert error , outbuffer == 0\n");
          DBG(ep); DBG("\n");
          break;
        }
        if ( 1 == is_start ) {
          // 日付取得
          tm_now = unix_time_to_date(outbuffer);
          point_hour = tm_now.tm_hour;
          is_start = 0;
        } else {
          // 日付の時間位置に出力
          if ( field[point_hour][field_column] ) {
            field[point_hour][field_column] = (field[point_hour][field_column] + outbuffer) / 2;
          } else {
            field[point_hour][field_column] = outbuffer;
          }
          //DBG("field[");DBG(point_hour);DBG("][");DBG(field_column);DBG("]=");DBG(field[point_hour][field_column]); DBG("\n");
        }
        // 改行なら先頭のtikから日付取得するフラグを立てる
        if ( ret == RETURN_VALUE ) {
          //DBG("RETURN_VALUE"); DBG("\n");
          is_start = 1;
          field_column = 0;
        }
        if ( ret == COMMA_VALUE ) {
          //DBG("COMMA_VALUE"); DBG("\n");
          field_column = field_column + 1;
        }
        break;
      }
      if ( ret == DATE_VALUE ) {
        //DBG("DATE_VALUE"); DBG("\n");
        memset(inbuffer, 0x00, sizeof(inbuffer));
        field_column = 0;
        break;
      }
      if ( ret == -1 ) {
        //DBG("FILE END"); DBG("\n");
        // ファイル終端
        break;
      }
    }
    if ( ret == -1 ) {
      // ファイル終端
      break;
    }
  } while (ret != -1);
  file.close();
  return 1;
}

void DispStrMessage(  QueueHandle_t xQueue, int strID )
{
  DBG("DispStrMessage\n");
  stTaskMessage xDispMessage;
  stTaskMessage *pxDispMessage;
  BaseType_t xStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);// ms

  xDispMessage.iMessageID = TASK_STRING;
  xDispMessage.Data.strID = strID;
  pxDispMessage = &xDispMessage;
  xStatus = xQueueSend(xQueue, pxDispMessage, xTicksToWait);
}

void vRecvSDTask( void *pvParameters )
{
  QueueHandle_t xQueue = *((QueueHandle_t*)pvParameters);
  BaseType_t xStatus;
  int ret = 0;
  stTaskMessage xDispMessage;
  stTaskMessage *pxDispMessage;
  stTaskSDMessage xMessage;
  stTaskSDMessage *pxMessage;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);// ms
  char buffer[100];
  char filename[100];
  unsigned long tik;
  struct tm tm_now;
  int ii = 0, jj = 0;
  EventBits_t uxBits;
  const TickType_t xTicksToStartWait = pdMS_TO_TICKS(10 * 1000); // ms

  memset( DispField, 0x00, sizeof(DispField));

  if (!SD.begin()) {
    DBG("Card Mount Failed"); DBG("\n");
    delay(1000);
    while (1) {
      DispStrMessage( xQueue, (int)DISP_SD_MOUNT_FAIL );
      DBG("Card Mount Failed");
      delay(1000);
    }
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    DBG("No SD card attached");
    delay(1000);
    while (1) {
      DispStrMessage( &xQueue, (int)DISP_SD_ATTACH_FAIL );
      DBG("No SD card attached");
      delay(1000);
    }
  }

  DBG("SD Card Type: ");
  if (cardType == CARD_MMC) {
    DBG("MMC"); DBG("\n");
  } else if (cardType == CARD_SD) {
    DBG("SDSC"); DBG("\n");
  } else if (cardType == CARD_SDHC) {
    DBG("SDHC"); DBG("\n");
  } else {
    DBG("UNKNOWN"); DBG("\n");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  DBG2("SD Card Size: %lluMB\n", cardSize);

  uxBits = xEventGroupWaitBits(
             event_ntp,     /* The event group being tested. */
             BIT_0        , /* The bits within the event group to wait for. */
             pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
             pdFALSE,       /* Don't wait for both bits, either bit will do. */
             xTicksToStartWait );/* Wait a maximum of 100ms for either bit to be set. */

  DBG("start vRecvSDTask \n");
  delay(10);

  tik = now();
#define SECONDS_IN_DAY   86400   // 24 * 60 * 60
  for ( ii = 0; ii < 7; ii++ ) {
    DBG("get_csv count="); DBG(ii); DBG("\n");
    tm_now = unix_time_to_date(tik - SECONDS_IN_DAY * ii);
    memset( filename, 0x00, sizeof(filename));
    sprintf(filename, "/temp-%04d-%02d-%02d.txt", tm_now.tm_year, tm_now.tm_mon, tm_now.tm_yday);
    sprintf(DispDate[ii], "%04d/%02d/%02d", tm_now.tm_year, tm_now.tm_mon, tm_now.tm_yday);
    DBG("open file name = "); DBG(filename); DBG("\n");
    if ( SD.exists(filename) ) {
      ret = get_csv( SD, filename, DispField[ii]);
      delay(10);
    }
    if (  0 == ii && -1 == ret ) {
      while (1) {
        DispStrMessage( &xQueue, (int)DISP_SD_OPEN_FAIL );
        DBG2("file open error at SD = %s\n", filename);
        delay(1000);
      }
    }
  }

  DBG("get_csv result"); DBG("\n"); DBG("\n");
  delay(10);
  for (ii = 0; ii < (sizeof(DispField[0]) / sizeof(DispField[0][0])); ii++) {
    DBG("ii="); DBG(ii); DBG("\n");
    for (jj = 0; jj < (sizeof(DispField[0][0]) / sizeof(double)); jj++) {
      DBG(DispField[0][ii][jj]); DBG(" ");
    }
    DBG("\n");
  }
  DBG("\n");

  memset( &xDispMessage, 0x00, sizeof(xDispMessage));
  xDispMessage.iMessageID = TASK_INIT_GRAPH;
  pxDispMessage = &xDispMessage;
  DBG("vRecvSDTask xQueueSend(xQueue\n");
  xStatus = xQueueSend(xQueue, pxDispMessage, xTicksToWait);

  if ( xStatus != pdPASS ) {
    while (1) {
      DispStrMessage( &xQueue, (int)DISP_RTOS_QUEUE_START_FAIL );
      DBG("rtos queue send error, stopped \n");
      delay(1000);
    }
  }

  while (1)
  {
    xStatus = xQueueReceive( xQueueSD, &xMessage, xTicksToWait );

    if ( xStatus == pdPASS )
    {
      switch ( xMessage.iMessageID ) {
        case TASK_SD_BME280:
          tik = now();
          tm_now = unix_time_to_date(tik);
          DBG2("%d/", tm_now.tm_year);
          DBG2("%d/", tm_now.tm_mon);
          DBG2("%d ", tm_now.tm_yday);
          DBG2("%d:", tm_now.tm_hour);
          DBG2("%d:", tm_now.tm_min);
          DBG2("%d\n", tm_now.tm_sec);
          memset( buffer, 0x00, sizeof(buffer));
          sprintf(buffer, "%d,%04d/%02d/%02d %02d:%02d:%02d,%.2lf,%.2lf,%.2lf \r\n",
                  now(),
                  tm_now.tm_year, tm_now.tm_mon, tm_now.tm_yday , tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec,
                  xMessage.Data.BME280.temp, xMessage.Data.BME280.humid, xMessage.Data.BME280.pressure);
          memset( filename, 0x00, sizeof(filename));
          sprintf(filename, "/temp-%04d-%02d-%02d.txt", tm_now.tm_year, tm_now.tm_mon, tm_now.tm_yday);
          if ( 0 == appendFile(SD, filename, buffer) ) {
            ;// Error
          }

          break;
      }
    }
    else
    {
      if (uxQueueMessagesWaiting(xQueueSD) != 0) {
        while (1) {
          ERR("rtos queue receive error, stopped \n");
          delay(1000);
        }
      }
    }
    delay(10);
  }
}
