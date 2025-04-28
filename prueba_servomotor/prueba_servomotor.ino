#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <DHT.h>
#define DHTPIN 15
#define DHTTYPE DHT11

//Configuracion de la red wifi
const char* ssid = "INFINITUMFAAF"; //nombre de la red
const char* password = "TywfmwWX9M";

Servo servo; //servomotor

//iniciamos el sensor de temperatura
DHT dht(DHTPIN,DHTTYPE);

//configuracion para el servo
int servoPin = 13;
int ledPin = 14;

//configuracion sensor temperatura
float temperatura;
float humedad;
float indice;

//puerto a utilizar
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
    <p>Control Servomotor</p>
    <button onclick="fetch('/servo/0')">0</button>
    <button onclick="fetch('/servo/90')">90</button>
    <button onclick="fetch('/servo/180')">180</button>
    <p>Control Led</p>
    <button onclick="fetch('/led/encender')">Encender</button>
    <button onclick="fetch('/led/apagar')">Apagar</button>
</body>
</html>
)rawliteral";

//funciones para las peticiones
void root(){
  server.send(200,"text/html",paginahtml);
}

//Funcion para encender LED
void encenderLed(){
digitalWrite(ledPin,HIGH);
server.send(200,"text/plain","LED encendido");  
}

//Funcion para apagar LED
void apagarLed(){
digitalWrite(ledPin,LOW);
server.send(200,"text/plain","LED apagado");
}

//Funcion para girar el servo a 0°
void girarCero(){
servo.write(0);
delay(600);
server.send(200,"text/plain","Giro de 0");
}

//Funcion para girar el servo a 90°
void girarNoventa(){
servo.write(90);
delay(600);
//servo.write(0);
//delay(600);
server.send(200,"text/plain","Giro de 90");
}

//Funcion para girar el servo a 180°
void girarCientoOchenta(){
servo.write(180);
delay(600);
//servo.write(0);
//delay(600);
server.send(200,"text/plain","Giro de 180");  
}

//retorna valor de temperatura
void medirTemperatura(){
temperatura = dht.readTemperature();
}

void setup() {
  Serial.begin(115200);

  //Conectamos a la red wifi
  WiFi.begin(ssid,password);
  Serial.print("Conectando a la red...");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    }
//  Mostramos la IP asignada
   Serial.print("\nConectado a la red");
   Serial.print("Dirección IP: ");
   Serial.print(WiFi.localIP());

  //Rutas del servidor
  server.on("/",root);
  server.on("/servo/0",girarCero);
  server.on("/servo/90",girarNoventa);
  server.on("/servo/180",girarCientoOchenta);
  server.on("/led/encender",encenderLed);
  server.on("/led/apagar",apagarLed);

  // Configurar el pin del LED como salida
  pinMode(ledPin, OUTPUT);

  //Iniciar el servidor
  server.begin();
  
  //configuramos el servo como entrada
  servo.attach(servoPin);

  //Inciamos el sensor de temperatura
  dht.begin();
}

void loop() {
server.handleClient();
}
