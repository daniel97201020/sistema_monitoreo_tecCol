const mqtt = require('mqtt');
const mysql = require('mysql');
const { MongoClient } = require('mongodb');
const uri = "mongodb+srv://agroTech:itcolima6@cluster0.qmrmc.mongodb.net/agroTech?retryWrites=true&w=majority";
const clientM = new MongoClient(uri,{useNewUrlParser: true, useUnifiedTopology:true});
const client = mqtt.connect('mqtt://broker.mqttdashboard.com');
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
    if(parseInt(hum_t) < 30){
        salida = 10;
      if(parseInt(hum_a) < 30 && parseInt(temp) > 30){
        salida = salida + 5;
      } else if(parseInt(hum_a) < 30 && parseInt(temp) <= 30){
        salida = salida + 3;
      } else {
        salida = salida + 2;
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
    var today = new Date();
    let myObj =
      {
        node: node,
        salida: salida,
        temp_a: temp,
        hum_a: hum_a,
        hum_t: hum_t,
        ph: ph,
        errores: errores,
        date: today.getTime()
      };
    const ready = connectDBB(myObj);
    return salida;

}

function EventoConectar() {
  client.subscribe('agroTech/Nodo_1', function(err){
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
    client.publish('agroTech/Nodo_1/valvulas', respuesta.toString());
  }
  client.end();
}

client.on('connect',EventoConectar);
client.on('message',EventoMensaje);
