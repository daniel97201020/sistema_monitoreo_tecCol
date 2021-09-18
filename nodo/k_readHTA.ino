bool readHTA(){
    Serial.println("readHTA");
    bool flag = false;
    float nowTemp = dht.readTemperature();
    float nowHum = dht.readHumidity();
    Serial.print("TEMP: ");
    Serial.println(String(nowTemp));
    Serial.print("HUM: ");
    Serial.println(String(nowHum));
    if(abs(nowTemp - temp_a) >= TOL_T|| abs(nowHum - hum_a) >= TOL_H){
       hum_a = nowHum;
       temp_a = nowTemp;
       flag = true;
    } else {
       hum_a = nowHum;
       temp_a = nowTemp;
    }
    return flag;
}
