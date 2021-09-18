void initWifi()
{
  mySerial.begin(9600);
  WiFi.init(&mySerial);
  if(WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("El modulo Wifi no está presente");
    while(true);
  }
  reconnectWifi();
}
