bool readHTA() {
  Serial.println("readHTA - Humedad y Temperatura del Ambiente");
  bool flag = false;
  float nowTemp = dht.readTemperature();
  float nowHum = dht.readHumidity();
  Serial.print("TEMP: ");
  Serial.println(String(nowTemp, 3));
  Serial.print("HUM: ");
  Serial.println(String(nowHum, 3));
  if (abs(nowTemp - temp_a) >= TOL_T || abs(nowHum - hum_a) >= TOL_H) {
    hum_a = nowHum;
    temp_a = nowTemp;
    flag = true;
  } else {
    hum_a = nowHum;
    temp_a = nowTemp;
  }
  return flag;
}
