void initWifi();
void reconnectWifi();
void reconnectClient();
void publishNow(String);
void everyHour();
void everyTen();
bool saveSSD();
bool readHTA();
bool readHTT();
bool readWind();
bool readPH();
String obtenerJSON();


#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>


#include <time.h>                       // time() ctime()
#ifdef ESP8266
#include <sys/time.h>                   // struct timeval
#endif
#include "CronAlarms.h"

#define WIFI_AP "INFINITUM_AA"
#define WIFI_PASSWORD "contraseña"
#define TOPIC_ID  "agroTech/Node_1"
#define SSpin 10

char server[50] = "192.168.0.6";

int count_esp = 0;

WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial mySerial(1,2);

CronId id;

File archivo;

unsigned long startTime;

int status = WL_IDLE_STATUS;
float temp_a  = 0.0;
float temp_t  = 0.0;
float hum_a = 0.0;
float hum_t = 0.0;
float wind = 0.0;
float ph  = 0.0;
String errores = "";
