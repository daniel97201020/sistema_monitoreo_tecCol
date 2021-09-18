void loop() {
  status = WiFi.status();
  if(status != WL_CONNECTED){
    reconnectWifi();
  }
  if(!client.connected()){
    reconnectClient();
  }
  client.loop();
  #ifdef __AVR__
    system_tick(); // must be implemented at 1Hz
  #endif
  time_t tnow = time(nullptr);
  Serial.println(asctime(gmtime(&tnow)));
  Cron.delay();// if the loop has nothing else to do, delay in ms
               // should be provided as argument to ensure immediate
               // trigger when the time is reached
  delay(1000);// do other things... like waiting one second between clock display
}
