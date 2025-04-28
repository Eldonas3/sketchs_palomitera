#include <WiFi.h>
#include <WebServer.h>
#include <Servo.h>

//Configuración de la red Wifi
const char* ssid = "MAPACHE5"; //nombre de la red
const char* password = "aaaaaaaa"; // constraseña

Servo servo;

int ledPin = 13;
int servoPin = 12;
int pulsoMinimo = 544;
int pulsoMaximo = 2400;
int angulo = 0;

//puerto a utlizar
WebServer server(80);

//pagina html
const char paginahtml[] PROGMEM =  R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        h1 { color: #007BFF; }
        button { padding: 10px; font-size: 20px; }
    </style>
</head>
<body>
    <h1>ESP32 Web Server</h1>
    <p>ESP32 funcionando como servidor web.</p>
    <button onclick="fetch('/led/on')">Encender LED</button>
    <button onclick="fetch('/led/off')">Apagar LED</button>
    <button onclick="fetch('/servo/girar')">Girar servo</button>
</body>
</html>
)rawliteral";


//Funciones para las peticiones
void root(){
  server.send(200,"text/html",paginahtml);
}

void encenderLed(){
//Encendemos el led del Esp32 (gpio2)
digitalWrite(ledPin,HIGH);
server.send(200,"text/plain","LED encendido");  
}  

void apagarLed(){
//apagamos el led
digitalWrite(ledPin,LOW);
server.send(200,"text/plain","LED apagado");
}

void girarServo(){
  servo.write(0);
  delay(600);
  servo.write(180);
  delay(600);
  }

void setup() {
Serial.begin(115200);

//Configuramos el gpio2 como salida
pinMode(2,OUTPUT);

//Conectamos a la red wifi
WiFi.begin(ssid,password);
Serial.print("Conectando a la red...");
while(WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print(".");
  }

  //Mostramos la ip asignada
  Serial.print("\nConectado a la red");
  Serial.print("Dirección IP: ");
  Serial.print(WiFi.localIP());

  //rutas del servidor
  server.on("/",root);
  server.on("/led/on",encenderLed);
  server.on("/led/off",apagarLed);
  server.on("/servo/girar",girarServo);

  //Iniciar el servidor
  server.begin();

  servo.attach(servoPin,pulsoMinimo,pulsoMaximo);
  
}

void loop() {
server.handleClient();
}
