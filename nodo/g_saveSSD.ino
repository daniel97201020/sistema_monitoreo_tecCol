bool saveSSD() {
  bool flag = false;
  archivo = SD.open("data.txt", FILE_WRITE);
  long unsigned timeNow = millis() - startTime;
  if (archivo) {
    archivo.println(temp_a);
    archivo.print(",");
    archivo.print(hum_a);
    archivo.print(",");
    archivo.print(hum_t);
    archivo.print(",");
    archivo.print(wind);
    archivo.print(",");
    archivo.print(ph);
    archivo.print(",");
    archivo.print(errores);
    archivo.print(",");
    archivo.print(String(timeNow));
    archivo.close();
    flag = true;
  }
  return flag;
}
