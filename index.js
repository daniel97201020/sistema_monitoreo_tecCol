var mqtt = require('mqtt')
var client = mqtt.connect('mqtt://broker.mqttdashboard.com')

function resepcionDeMensaje(mensaje){
    console.log("RECEPCION DE MENSAJE")
    var salida = 0;
    var array = JSON.parse(mensaje)
    var temp = array.temp_a
    var hum_a = array.hum_a
    var hum_t = array.hum_t
    var ph = array.ph
    var errores = array.errores
    if(parseInt(hum_t) < 30){
        salida = 10
      if(parseInt(hum_a) < 30 && parseInt(temp) > 30){
        salida = salida + 5
      } else if(parseInt(hum_a) < 30 && parseInt(temp) <= 30){
        salida = salida + 3
      } else {
        salida = salida + 2
      }
    } else {
      if(parseInt(hum_a) < 30 && parseInt(temp) > 30){
        salida = 5
      } else {
        salida = 0
      }
    }
    console.log(salida)
    return salida
}

function EventoConectar() {
  client.suscribe('agroTech/Nodo_1', function(err){
    if(!err){
      console.log("NO HUBO ERRORES")
    }
  })
}

function EventoMensaje(topic, message){
  console.log(message.toString())
  var respuesta = resepcionDeMensaje(message.toString())
  if(parseInt(respuesta) > 0){
    client.publish('agroTech/Nodo_1/valvulas', respuesta.toString())
  }
  client.end()
}

client.on('connect',EventoConectar)
client.on('message',EventoMensaje)
