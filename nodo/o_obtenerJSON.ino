String obtenerJSON() {
  String payload = "{ temp_a: " + String(temp_a, 3) + ", hum_a: " + String(hum_a, 3) + ", hum_t: " + String(hum_t, 3) + ", wind:" + String(wind, 3) + ", ph: " + String(ph, 3) + ", errors: " + errores + " }";
  return payload;
}
