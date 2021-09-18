void everyTen(){
  if(!readHTA()){
    temp_a = -1;
    hum_a  = -1;
  }
  if(!readHTT()){
    temp_t = -1;
    hum_t  = -1;
  }
  if(!readWind()){
    wind = -1;
  }
  if(!readWind()){
    ph = -1;
  }
}
