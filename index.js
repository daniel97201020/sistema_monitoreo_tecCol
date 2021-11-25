const mqtt = require('mqtt');
const mysql = require('mysql');
const { MongoClient } = require('mongodb');

const uri = "mongodb+srv://agroTech:itcolima6@cluster0.qmrmc.mongodb.net/myFirstDatabase?retryWrites=true&w=majority";
const clientM = new MongoClient(uri,{useNewUrlParser: true, useUnifiedTopology: true });
var settings = {
  clientId: "mqtt-control-access-server-348324098",
  keepalive: 6000,
  clean: false,
  reconnectPeriod: 1000 * 1
}
const client = mqtt.connect('mqtt://broker.mqttdashboard.com', settings);
function parseJSON(mensaje){
    var array = JSON.parse(mensaje);
    return array;
}
async function connectDBB(data) {
  // connect to database
  const database =  await clientM.connect().catch(err => {
    console.log("ERROR while connecting to database at: connectDBB");
    console.log(err);
    clientM.close();
  });

  // database connection failed
  if (!database) {
    return false;
  }

  // connection successful => find user
  let user;
  try {
    user = await database.db("agroTech").collection("registers").insertOne(data);
    clientM.close();
    return true;
  } catch(err) {
    log("ERROR while inserting user in database at: connectDBB");
    console.log(err);
    clientM.close();
    return false;
  }
}
function resepcionDeMensaje(array, topic){
    console.log("RECEPCION DE MENSAJE")
    var salida = 0;
    var temp = array.temp_a;
    var hum_a = array.hum_a;
    var hum_t = array.hum_t;
    var ph = array.ph;
    var errores = array.errores;
    if(parseInt(hum_t) < 50){
        salida = 10;
      if(parseInt(hum_a) < 30 && parseInt(temp) > 30){
        salida = salida + 5;
      } else if(parseInt(hum_a) < 30 && parseInt(temp) <= 30){
        salida = salida + 3;
      } else {

      }
    } else {
      if(parseInt(hum_a) < 30 && parseInt(temp) > 30){
        salida = 5;
      } else {
        salida = 0;
      }
    }
    var topics = topic.split('/');
    var node = topics[1];
    var str = "";
    var currentTime = new Date();
    var hours = currentTime.getHours();
    var minutes = currentTime.getMinutes();
    var seconds = currentTime.getSeconds();
    var month = currentTime.getMonth()+1;
    var year = currentTime.getFullYear();
    var day = currentTime.getDate();
    if (minutes < 10) {
        minutes = "0" + minutes
    }
    if (seconds < 10) {
        seconds = "0" + seconds
    }
    if(month < 10){
      month = "0"+month;
    }
    if(day < 10){
        day = "0"+day;
    }
    str += day +"-"+month+"-"+year+ "  "+hours + ":" + minutes + ":" + seconds+ " ";
    if(hours > 11){
        str += "PM"
    } else {
        str += "AM"
    }
    let myObj =
      {
        node: node,
        salida: salida,
        temp_a: temp,
        hum_a: hum_a,
        hum_t: hum_t,
        ph: ph,
        errores: errores,
        date: str
      };
    const ready = connectDBB(myObj);
    return salida;

}

function EventoConectar() {
  client.subscribe('NodeLime/Nodo_1', function(err){
    if(!err){
      console.log("NO HUBO ERRORES");
    }
  });
}

function EventoMensaje(topic, message){
  console.log(message.toString());
  var arrayJSON = parseJSON(message.toString());
  var respuesta = resepcionDeMensaje(arrayJSON,topic);
  if(parseInt(respuesta) > 0){
    client.publish('NodeLime/Nodo_1/valvulas', respuesta.toString());
  }
}

client.on('connect',EventoConectar);
client.on('message',EventoMensaje);
