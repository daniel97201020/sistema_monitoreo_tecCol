void reconnectWifi()
{
  Serial.println("Iniciar conexión a la red WIFI");
  while (status != WL_CONNECTED) {
    Serial.print("Intentando conectarse a WPA SSID: ");
    Serial.println(WIFI_AP);
    //Conectar a red WPA/WPA2
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Conectado a la red WIFI");
  digitalWrite(LED_WIFI, HIGH);
}
