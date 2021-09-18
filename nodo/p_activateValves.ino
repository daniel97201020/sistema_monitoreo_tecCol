void activateValves(){
  if(a == 30){
    digitalWrite(PIN_VALVE, LOW);
    digitalWrite(LED_ACTIVATE, LOW);
    a = 0;
  } else {
    a++;
    digitalWrite(PIN_VALVE, HIGH);
    digitalWrite(LED_ACTIVATE, HIGH);
  }
}
