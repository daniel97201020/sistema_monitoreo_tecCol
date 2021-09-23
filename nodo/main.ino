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
void readSuscribe(char*,byte*,int unsigned);
void activateValves();
bool checkCommand(String);


#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>
#include <DHT.h>
#define mySerial Serial2


#include <time.h>                       // time() ctime()
#ifdef ESP8266
#include <sys/time.h>                   // struct timeval
#endif
#include "CronAlarms.h"
#include <RTClock.h>

#define WIFI_AP "INFINITUM_AA"
#define WIFI_PASSWORD "contraseña"
#define TOPIC_ID  "agroTech/Node_1"
#define SSpin 40
#define DHTPIN 46
#define DHTTYPE DHT11
#define TOL_T 1.5
#define TOL_H 5.3
#define TOL_W 3.0
#define TOL_PH 0.5
#define LED_WIFI 15
#define LED_ENC 14
#define LED_ESP8266 16
#define LED_PUBLISH 17
#define LED_SUBSCRIBE 18
#define LED_ACTIVATE 19
#define PIN_VALVE 11
#define A0 10

char server[50] = "192.168.0.6";

WiFiEspClient espClient;

PubSubClient client(espClient);

CronId id;

File archivo;

DHT dht(DHTPIN, DHTTYPE);

RTClock rtclock (RTCSEL_LSE);

unsigned long startTime;

int status = WL_IDLE_STATUS;
float temp_a  = 0.0;
float hum_a = 0.0;
float hum_t = 0.0;
float wind = 0.0;
float ph  = 0.0;
String errores = "";
int a = 0;
