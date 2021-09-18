void everyHour(){
    Serial.print("everyHour PAYLOAD: ");
    if(!saveSSD()){
       errores = errores + "|NO se pudo guardar los datos";
    }
    String payload = obtenerJSON();
    Serial.println(payload);
    publishNow(payload);
 }
