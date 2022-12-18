#ifndef GRAPHT_TASKCONTROL_H
#define GRAPHT_TASKCONTROL_H

typedef struct _Btn
{
  int iLight;
  int iUP;
  int iDOWN;
  int TempHumi;
  int Enter;
} stBtn;

typedef struct _XYZ
{
  int iX;
  int iY;
  int iZ;
} stXYZ;

typedef struct _stBME280
{
  double temp;
  double humid;
  double pressure;
} stBME280;

typedef struct TaskMessage
{
  int iMessageID;
  union _Data {
    stBtn BTN;
    stXYZ XYZ;
    stBME280 BME280;
    int strID;
  } Data;
} stTaskMessage;

typedef struct TaskServerMessage
{
  int iMessageID;
  union _Data {
    stBME280 BME280;
  } Data;
} stTaskServerMessage;

typedef struct TaskSDMessage
{
  int iMessageID;
  union _Data {
    stBME280 BME280;
  } Data;
} stTaskSDMessage;

typedef struct _stStrMessage
{
  int strID;
  char message[32];
} stStrMessage;

enum DISP_STR {
  DISP_SD_MOUNT_FAIL,
  DISP_SD_ATTACH_FAIL,
  DISP_SD_OPEN_FAIL,
  DISP_RTOS_QUEUE_START_FAIL,
  DISP_SD_FAIL
};


#define IS_NONE 0
#define IS_EXSIST 1

enum TASK_SD {
  TASK_SD_BME280
};

enum TASK_SERVER {
  TASK_SERVER_BME280
};

enum TASK_DISP {
  TASK_XYZ,
  TASK_BTN,
  TASK_BME280,
  TASK_INIT_GRAPH,
  TASK_STRING,
  TASK_GET_TOUCH,
  TASK_RE_TEMP
};

#define BIT_0  ( 1 << 0 )

extern xSemaphoreHandle mux_DBG;
extern xSemaphoreHandle mux_QueueDisp;
extern stStrMessage disp_messege_string[5];
extern char DispDate[7][32];
extern double DispField[7][24][3];
extern QueueHandle_t xQueueServer;
extern QueueHandle_t xQueueSD;
extern EventGroupHandle_t event_ntp;
extern EventGroupHandle_t event_ntp_bme280;
extern EventGroupHandle_t event_ntp_disp;
extern EventGroupHandle_t event_SendToch;

void vRecvDisplayTask( void *pvParameters );
void vSendBtnTask( void *pvParameters );
void vSendTouchTask( void *pvParameters );
void vSendBME280Task( void *pvParameters );
void vRecvServerTask( void *pvParameters );
void vRecvSDTask( void *pvParameters );
void vSendReTempTask( void *pvParameters );
#endif /* GRAPHT_TASKCONTROL_H */
