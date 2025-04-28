#include <WiFi.h> 
#include <WebServer.h>
const char* ssid = "MAPACHE5";
const char* password = "aaaaaaaa";

const int ledPins[] = {5,18,19,21};

WebServer server(80);

void handleLedControl(){
if(!server.hasArg("led")){
server.send(400, "text/plain", "Falta el parámetro 'led'");
return;
}

int numLeds = server.arg("led").toInt();
if(numLeds < 0 || numLeds >4){
server.send(400,"text/plain","Número inválido, debe estar entre 0 y 4");
return;
}

for(int i=0; i<4; i++){
digitalWrite(ledPins[1],LOW);
}

for(int i=0; i digitalWrite(ledPins[1],HIGH));
}

server.send(200, "text/plain","LEDs actualizados: " + String(numLeds));
}

void setup() {
Serial.begin(115200);

for (int i=0; i<4;i++){
pinMode(ledPins[i],OUTPUT);
digitalWrite(ledPins[i],LOW);
}

WiFi.begin(ssid,password);
Serial.print("Conectando a Wifi...");

while(WiFi.status() != WL_CONNECTED){
delay(500);
Serial.print(".");
}

Serial.println("\nConectado a WiFi!");
Serial.print("Dirección IP:");
Serial.println(WiFi.localIP());

server.on("/led",handleLedControl);
server.begin();
Serial.println("Servidor web iniciado!");
}

void loop() {
server.handleClient();
}
