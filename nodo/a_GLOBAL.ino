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
void readSuscribe();
void activateValves();


#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <DHT.h>


#include <time.h>                       // time() ctime()
#ifdef ESP8266
#include <sys/time.h>                   // struct timeval
#endif
#include "CronAlarms.h"

#define WIFI_AP "INFINITUM_AA"
#define WIFI_PASSWORD "contraseña"
#define TOPIC_ID  "agroTech/Node_1"
#define SSpin 10
#define DHTPIN 2
#define DHTTYPE DHT11
#define TOL_T 1.5
#define TOL_H 5.3
#define TOL_W 3.0
#define TOL_PH 0.5

char server[50] = "192.168.0.6";

WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial mySerial(1, 2);

CronId id;

File archivo;

DHT dht(DHTPIN, DHTTYPE);

unsigned long startTime;

int status = WL_IDLE_STATUS;
float temp_a  = 0.0;
float hum_a = 0.0;
float hum_t = 0.0;
float wind = 0.0;
float ph  = 0.0;
String errores = "";
