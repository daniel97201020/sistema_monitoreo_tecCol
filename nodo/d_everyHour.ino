void everyHour(){
    Serial.print("everyHour PAYLOAD: ");
    if(!saveSSD()){
       errores = errores + "|NO se pudo guardar los datos";
    }
    String payload = "{ temp_a: "+String(temp_a,3)+", temp_t:"+String(temp_t,3)+", hum_a: "+String(hum_a,3)+", hum_t: "+String(hum_t,3)+", wind:"+String(wind,3)+", ph: "+String(ph,3)+", errors: "+errores+" }";
    Serial.println(payload);
    publishNow(payload);
 }
