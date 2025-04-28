#include <ESP8266WiFi.h>
#include <Servo.h>


const char* ssid = "MAPACHE5";     // Cambia esto por tu red WiFi
const char* password = "aaaaaaaa"; // Cambia esto por tu contraseña

WiFiServer server(80);

const int ledPin = 12; // GPIO5 (D1 en NodeMCU)
const int servoPin = 13; // GPIO4 (D2 en NodeMCU)
Servo servoMotor;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  servoMotor.attach(servoPin);
  servoMotor.write(90);

  // Conectar a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("Cliente conectado");
  String request = client.readStringUntil('\r');
  client.flush();

  // Control LED
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
  } 
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, LOW);
  }

  // Control Servo
  if (request.indexOf("/SERVO=") != -1) {
    int pos = request.substring(request.indexOf("=") + 1).toInt();
    servoMotor.write(pos);
  }

  // Enviar página web con AJAX
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html lang='es'><head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<title>ESP8266 AJAX</title>");
  
  // JavaScript con AJAX para evitar recargar la página
  client.println("<script>");
  client.println("function sendCommand(command) {");
  client.println("  var xhttp = new XMLHttpRequest();");
  client.println("  xhttp.open('GET', command, true);");
  client.println("  xhttp.send();");
  client.println("}");
  client.println("</script>");

  client.println("</head><body>");
  client.println("<h1>Control de LED y Servo</h1>");
  
  // Botones de control del LED con AJAX
  client.println("<button onclick=\"sendCommand('/LED=ON')\">Encender LED</button>");
  client.println("<button onclick=\"sendCommand('/LED=OFF')\">Apagar LED</button>");

  // Control del Servo con AJAX
  client.println("<p>Mover Servo:</p>");
  client.println("<button onclick=\"sendCommand('/SERVO=0')\">0°</button>");
  client.println("<button onclick=\"sendCommand('/SERVO=90')\">90°</button>");
  client.println("<button onclick=\"sendCommand('/SERVO=180')\">180°</button>");

  client.println("</body></html>");

  client.stop();
  Serial.println("Cliente desconectado");
}
