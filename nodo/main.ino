#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <SPI.h>
#include <time.h>                       // time() ctime()
#ifdef ESP8266
#include <sys/time.h>                   // struct timeval
#endif
#include "CronAlarms.h"
#include <Arduino.h>
#include <Ticker.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

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
const char* root_topic_publish = "NodeLime/Nodo_1";
const char* root_topic_subscribe = "NodeLime/Nodo_1/valvulas";

//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "INFINITUM2FA2_2.4";
const char* password =  "UQY4HxhpAu"; //UQY4HxhpAu



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(27, DHT11);
CronId id;
Ticker timer;
long count=0;

const int analogPin = 34;
int lectura;
float hum_t;
float hum_a;
float temp_a;
const float period = 1; //seconds
int val;
int duration;

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

void timer1() {
  val++;
  Serial.println(val);
  if(val == duration){
      val = 0;
      digitalWrite(32,LOW);
      digitalWrite(25,LOW);
      timer.detach();
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.println(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void setup() {
  pinMode(32,OUTPUT);
  pinMode(25,OUTPUT); //ENC
  pinMode(26,OUTPUT); //WIFI
  pinMode(14,OUTPUT); //BROKER
  pinMode(12,OUTPUT); //PUB/SUB
  pinMode(13,OUTPUT);//VALVE
  digitalWrite(25,HIGH);
  Serial.begin(115200);
  //SD CARD SETUP
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
     Serial.println("No SD card attached");
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
        Serial.println("MMC");
  } else if(cardType == CARD_SD){
        Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
  } else {
        Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  listDir(SD, "/", 0);
  createDir(SD, "/NodeLime");
  listDir(SD, "/", 2);
  writeFile(SD, "/NodeLime/registers.csv", "NODO_1,30.1,70.35,30.23,N/A,N/A,23-07-2021");
  //WIFI CLIENT SETUP
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  struct tm tm_newtime; // set time to Saturday 8:29:00am Jan 1 2011
  tm_newtime.tm_year = 2021 - 1970;
  tm_newtime.tm_mon = 9;
  tm_newtime.tm_mday = 28;
  tm_newtime.tm_hour = 11;
  tm_newtime.tm_min = 41;
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
  Cron.create("0 */1 * * * *",every15,false);
  Cron.create("0 */5 * * * *",everyMin,false);
  dht.begin();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  #ifdef __AVR__
    system_tick(); // must be implemented at 1Hz
  #endif
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
  digitalWrite(26,HIGH);
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
        digitalWrite(14,HIGH);
      }else{
        Serial.println("fallo Suscripciión");
        digitalWrite(14,LOW);
      }
    } else {
      digitalWrite(14,LOW);
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
  float nowHumT = map(lectura, 4095, 0, 0, 100);
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
      digitalWrite(12,HIGH);
      Serial.println("Se publica");
      String str = "{\"temp_a\":\""+String(temp_a,3)+"\",\"hum_a\":\""+String(hum_a,3)+"\",\"hum_t\":\""+String(hum_t,3)+"\",\"ph\":\"N/A\",\"errores\":\"N/A\",\"date\":\"N/A\"}";
      int largo = str.length()+2;
      Serial.println(str);
      Serial.println(largo);
      char msg[largo];
      str.toCharArray(msg,largo);
      client.publish(root_topic_publish,msg);
      delay(500);
   }
    digitalWrite(12,LOW);
    time_t tnow = time(nullptr);
    String dateNow = asctime(gmtime(&tnow));
    String myString = String(temp_a,3)+","+String(hum_a,3)+","+String(hum_t,3)+",N/A,N/A,"+dateNow;
    int tamano = myString.length()+2;
    char message[tamano];
    myString.toCharArray(message, tamano);
    appendFile(SD, "/NodeLime/registers.csv",message);
    readFile(SD, "/NodeLime/registers.csv");
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
  /*f(com == "ON"){
      digitalWrite(LED_BUILTIN,HIGH);
   } else if(com == "OFF") {
      digitalWrite(LED_BUILTIN,LOW);
   } else {
      Serial.println("COMANDO INCORRECTO");
   } */
   if(com.length() > 0 && com.toInt() > 0){
        duration = com.toInt();
        timer.attach(period, timer1);
        digitalWrite(32,HIGH);
        digitalWrite(25,HIGH);
    }
}
