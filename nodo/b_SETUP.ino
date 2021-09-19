void setup() {

  pinMode(LED_ENC,OUTPUT);
  pinMode(LED_ESP8266,OUTPUT);
  pinMode(LED_PUBLISH,OUTPUT);
  pinMode(LED_SUBSCRIBE,OUTPUT);
  pinMode(LED_ACTIVATE,OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(PIN_VALVE,OUTPUT);
  digitalWrite(LED_ENC, LOW);
  digitalWrite(LED_ESP8266, LOW);
  digitalWrite(LED_PUBLISH, LOW);
  digitalWrite(LED_SUBSCRIBE, LOW);
  digitalWrite(LED_ACTIVATE, LOW);
  digitalWrite(LED_WIFI, LOW);
  digitalWrite(PIN_VALVE, LOW);
  Serial.begin(9600);
  //CRON SETUP
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

  Cron.create("* * */1 * * *", everyHour, false);
  Cron.create("* */10 * * * *", everyTen, false);
  digitalWrite(LED_ENC,HIGH);
  initWifi();
  client.setServer(server, 1883);
  client.setCallback(readSuscribe);
  //Inicialización de Tarjeta
  if (!SD.begin(SSpin)) {
    Serial.println("Fallo en iniciar la Tarjeta SD");
    return;
  }
  Serial.println("Inicialización Correcta de Tarjeta");
  startTime = millis();
  dht.begin();
}
