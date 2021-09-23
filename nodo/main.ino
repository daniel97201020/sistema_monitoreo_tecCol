#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <SPI.h>
#include <time.h>                       // time() ctime()
#ifdef ESP8266
#include <sys/time.h>                   // struct timeval
#endif
#include "CronAlarms.h"
#define TOL_H 1.5
#define TOL_T 1
//*******************
///***RESTART 2 VECES*****
//*******************

//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char* mqtt_server = "broker.mqttdashboard.com";
const int mqtt_port = 1883;
const char* root_topic_publish = "agroTech/Nodo_1";
const char* root_topic_subscribe = "agroTech/Nodo_1/valvulas";

//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "INFINITUM2FA2_2.4";
const char* password =  "UQY4HxhpAu";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(27, DHT11);
CronId id;
char msg[200];
long count=0;

int analogPin = 32;
int lectura;
float hum_t;
float hum_a;
float temp_a;

//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();
bool leerHumedadT();
bool leerTempHum();
void publishNow();
void checkCommand(String);

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  struct tm tm_newtime; // set time to Saturday 8:29:00am Jan 1 2011
  tm_newtime.tm_year = 2011 - 1900;
  tm_newtime.tm_mon = 1 - 1;
  tm_newtime.tm_mday = 1;
  tm_newtime.tm_hour = 8;
  tm_newtime.tm_min = 29;
  tm_newtime.tm_sec = 0;
  tm_newtime.tm_isdst = 0;
#ifdef ESP8266
  timeval tv = { mktime(&tm_newtime), 0 };
  timezone tz = { 0, 0};
  settimeofday(&tv, &tz);
#elif defined(__AVR__)
  set_zone(0);
  set_dst(0);
  set_system_time( mktime(&tm_newtime) );
#endif
  // create timers, to trigger relative to when they're created
  Cron.create("*/15 * * * * *", every15, false);
  Cron.create("*/60 * * * * *", everyMin, false);
  dht.begin();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  #ifdef __AVR__
    system_tick(); // must be implemented at 1Hz
  #endif
  time_t tnow = time(nullptr);
  Cron.delay();
  delay(1000);
  client.loop();
}




//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);
  checkCommand(incoming);
}


//*****************************
//***       LEER HUMEDAD y TEMPERATURA      ***
//*****************************
bool leerTempHum(){
  Serial.println("readHTA - Humedad y Temperatura del Ambiente");
  bool flag = false;
  float nowTemp = dht.readTemperature();
  float nowHum = dht.readHumidity();
   if (isnan(nowTemp) || isnan(nowHum)) {
    Serial.println("Falla al leer el sensor DHT11!");
  }
  Serial.print("TEMP: ");
  Serial.print(nowTemp);
  Serial.println("°C");
  Serial.print("HUM: ");
  Serial.print(nowHum);
  Serial.println("%");
  if (abs(nowTemp - temp_a) >= TOL_T || abs(nowHum - hum_a) >= TOL_H) {
    hum_a = nowHum;
    temp_a = nowTemp;
    Serial.println("Si se envia ahora");
    flag = true;
  } else {
    hum_a = nowHum;
    temp_a = nowTemp;
  }
  return flag;
}

//*****************************
//***       LEER HUMEDAD TIERRA        ***
//*****************************

bool leerHumedadT(){
  bool flag = false;
  Serial.println("readHTA - Humedad de Tierra Relativa %");
  lectura = analogRead(analogPin);
  float nowHumT = map(lectura, 1023, 0, 0, 100);
  Serial.print("HUM-T: ");
  Serial.print(nowHumT);
  Serial.println("%");
  if(abs(nowHumT - hum_t) >= TOL_H){
    hum_t = nowHumT;
    Serial.println("Si se envia ahora");
    flag = true;
  } else {
    hum_t = nowHumT;
  }
  return flag;
}

void publishNow(){
  if (client.connected()){
      Serial.println("Se publica");
      String str = "HT:" + String(hum_t,3)+"; HA: "+String(hum_a,3)+" TA: "+String(temp_a,3);
      str.toCharArray(msg,200);
      client.publish(root_topic_publish,msg);
      delay(1000);
   }
}

void every15() { //CADA 10MIN
  Serial.println("15 second timer");
  bool dato1 = leerTempHum();
  delay(100);
  bool dato2 = leerHumedadT();
  if(dato1 == true || dato2 == true){
    publishNow();
  }
}

void everyMin(){ //CADA HORA
  Serial.println("Every Minute");
  publishNow();
}

void checkCommand(String com){
   if(com == "ON"){
      digitalWrite(LED_BUILTIN,HIGH);
   } else if(com == "OFF") {
      digitalWrite(LED_BUILTIN,LOW);
   } else {
      Serial.println("COMANDO INCORRECTO");
   }
}

//COMENTARIO//
