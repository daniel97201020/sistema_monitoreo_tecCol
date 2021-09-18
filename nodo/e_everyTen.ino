void everyTen(){
   bool hta = readHTA();
   delay(100);
   bool htt = readHTT();
   delay(100);
   bool wind = readWind();
   delay(100);
   bool ph = readPH();
   delay(100);
   if(hta == true || htt == true || wind == true || ph == true){
     String payload = obtenerJSON();
     Serial.println(payload);
     publishNow(payload);
   }
}
