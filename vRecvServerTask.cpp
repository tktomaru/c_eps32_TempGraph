#include "FreeRTOS.h"  // FreeRTOS利用時必須
#include <Arduino.h>
#include "TaskControl.h"
#include "DBG.h"
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <SPIFFS.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include "NTPClient.h"
#include <WiFiUdp.h>
#include "Zackert.h"
#include <TimeLib.h>                // for use now()
extern "C" {
#include "CTime.h"
}

//===(S)==NTP=========================
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.nict.jp", 0);
//===(E)==NTP=========================

//===(S)===========================
WiFiClient client;
Zackert zach;

char ssid[100] = "input ssid";
char password[100] = "input ssid password";

char writeKey[100] = "input writeKey";
char accountID[100] = "input accountID";
#define debug_zach 0 // 0=運用   1=ローカルテスト

#define BUFSIZE 1024
char buffer[BUFSIZE];
//===(E)============================


//############  NTP   ############
/**
   Initialize NTP client
*/
void initNTP() {
  timeClient.begin();
}

unsigned long getSntpTime() {
  unsigned long t = 0 ;
  configTime(9 * 3600 , 0, "ntp.nict.jp", NULL, NULL);
  if (timeClient.update()) {
    t = timeClient.getEpochTime();
  }
  return t;
}

void myprintJST() {
  unsigned long tik;
  struct tm tm_now;
  tik = now();
  tm_now = unix_time_to_date(tik);
  DBG2("%d/", tm_now.tm_year);
  DBG2("%d/", tm_now.tm_mon);
  DBG2("%d ", tm_now.tm_yday);
  DBG2("%d:", tm_now.tm_hour);
  DBG2("%d:", tm_now.tm_min);
  DBG2("%d\n", tm_now.tm_sec);
  DBG("\r\n");
}

void TimeSync()
{
  int ii = 0;
  unsigned long sntptime;  //  SNTPで時刻取得
  time_t t_before;
  struct tm *tm_pbefore;
  struct tm tm_before;
  struct tm *tm_pafter;
  struct tm tm_after;
  t_before = now();
  tm_before = unix_time_to_date(t_before);
  do {
    sntptime = getSntpTime();  //  SNTPで時刻取得
    DBG(sntptime);
    DBG("\n");
    tm_after = unix_time_to_date(sntptime);
    ii = ii + 1;
    delay(200);
    //　整時後が2018年前か（200回）、または、整時前と整時後が2018年前の間くりかえし（初回時に整時をする）
  } while ( ((ii <= 200) && (tm_after.tm_year < 2018)) || ((tm_pbefore->tm_year < 2018) && (tm_after.tm_year < 2018)) );

  if (tm_after.tm_year >= 2018) {
    setTime(sntptime);
  }
  myprintJST();
}

//############  FS   ############
void deleteFile() {
  if (SPIFFS.remove("/config.json")) {
    //Serial.println("- file deleted");
  } else {
    //Serial.println("- delete failed");
  }
}

void ReadFS()
{
  DBG("mounting FS...");

  if (SPIFFS.begin()) {
    DBG("mounted file system"); DBG("\n");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      DBG("reading config file"); DBG("\n");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        DBG("opened config file"); DBG("\n");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          DBG("\nparsed json");

          const char* tempssid = json["mqtt_ssid"];
          strcpy( ssid, tempssid);
          const char* temppassword = json["mqtt_password"];
          strcpy( password, temppassword);
          const char* tempwriteKey = json["mqtt_writeKey"];
          strcpy( writeKey, tempwriteKey);
          const char* tempaccountID = json["mqtt_accountID"];
          strcpy( accountID, tempaccountID);
        } else {
          DBG("failed to load json config"); DBG("\n");
        }
      }
    }
  } else {
    DBG("failed to mount FS\n"); DBG("\n");
  }
}

int sendDataTozach(double temp, double humid, double pressure) {
  int i;
  time_t created;
  int vb;
  char vbbuf[12];

  DBG("zach.send() to: accountID = ");  DBG(accountID); DBG("\r\n");
  DBG("writeKey: "); DBG(writeKey); DBG("\r\n");
  zach.begin(60000, writeKey, &client, debug_zach);

  sprintf(&(buffer[0]), "{\"accountID\":\"%s\",\"writeKey\":\"%s\",\"data\":[", accountID , writeKey);

  created = now();
  sprintf(&buffer[strlen(buffer)], "{\"created\":%d,\"time\":1,\"d1\":%3.1f,\"d2\":%3.1f,\"d3\":%3.1f,\"d4\":%3.1f,\"d5\":%3.1f,\"d6\":%3.1f,\"d7\":%3.1f,\"d8\":%3.1f,\"d9\":%3.1f,\"tag\":%d},",
          created,
          temp,    // d1
          humid,   // d2
          pressure,// d3
          0.0,  // d4
          0.0, // d5
          0.0, // d6
          0.0, // d7
          0.0, // d8
          0.0, // d9
          1    // tag
         );

  buffer[strlen(buffer) - 2] = '\0';
  sprintf(&buffer[strlen(buffer)], "}]}", vbbuf);

  DBG("buf: "); DBG(strlen(buffer)); DBG(" bytes\r\n");

  int n = zach.bulk_send(buffer, strlen(buffer));
  DBG("sent: "); DBG(n); DBG("\r\n");
  DBG(buffer); DBG("\r\n");
  return n;
}
void vRecvServerTask( void *pvParameters )
{
  BaseType_t xStatus;
  int ret = 0;
  stTaskServerMessage xMessage = {0};
  stTaskServerMessage *pxMessage;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);// ms
  char buffer[6][32];

  DBG("start vRecvDisplayTask \n");

  //##### read configuration from FS json #####
  ReadFS();
  WiFi.begin(ssid, password);  //  Wi-Fi APに接続
  initNTP();
  // 整時
  TimeSync();
  delay( 100 );
  // SDタスクに整時したので起動してよいと通知する
  DBG("start event_ntp \n");
  xEventGroupSetBits( event_ntp, BIT_0 );
  // BME280タスクに整時したので起動してよいと通知する
  DBG("start event_ntp_bme280 \n");
  xEventGroupSetBits( event_ntp_bme280, BIT_0 );
  // Dispタスクに整時したので起動してよいと通知する
  DBG("start event_ntp_disp \n");
  xEventGroupSetBits( event_ntp_disp, BIT_0 );

  while (1)
  {
    xStatus = xQueueReceive( xQueueServer, &xMessage, xTicksToWait );

    if ( xStatus == pdPASS )
    {
      switch ( xMessage.iMessageID ) {
        case TASK_SERVER_BME280:
          DBG("start vRecvDisplayTask DISP_SBME\n ");

          //===(S)========= Serverに情報を送信する ============
          //  Wi-Fi APに接続
          WiFi.begin(ssid, password);
          //  Wi-Fi AP接続待ち
          while (WiFi.status() != WL_CONNECTED) {
            DBG("*");
            delay(100);
          }
          DBG("\n");
          // 整時
          TimeSync();
          // サーバに送信する
          sendDataTozach(xMessage.Data.BME280.temp, xMessage.Data.BME280.humid, xMessage.Data.BME280.pressure);

          // WiFiはオフにして節電する
          WiFi.disconnect(true);
          //===(E)========= Serverに情報を送信する ============

          //===(S)========= SD カードに情報を書き込むためにタスク間通信 ============
          xMessage.iMessageID = TASK_SD_BME280;
          pxMessage = &xMessage;
          DBG("xQueueSend : SD \n");
          xStatus = xQueueSend(xQueueSD, pxMessage, xTicksToWait);

          if ( xStatus != pdPASS ) {
            while (1) {
              ERR("rtos queue send error, stopped \n");
              delay(1000);
            }
          }
          break;
          //===(E)========= SD カードに情報を書き込むためにタスク間通信 ============
      }
    } else {
      if (uxQueueMessagesWaiting(xQueueServer) != 0) {
        while (1) {
          ERR("rtos queue receive error, stopped \n");
          delay(1000);
        }
      }
    }
    delay(10);
  }
}
