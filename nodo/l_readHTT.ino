bool readHTT() {
  bool flag2 = false;
  Serial.println("readHTA - Humedad de Tierra Relativa %");
  int lectura = analogRead(A0);
  float nowHumT = map(lectura, 1023, 0, 0, 100);
  Serial.print("HUM-T: ");
  Serial.println(String(nowHumT, 3));
  if(abs(nowHumT - hum_t) >= TOL_H){
    hum_t = nowHumT;
    flag2 = true;
  } else {
    hum_t = nowHumT;
  }
  return flag2;
}
