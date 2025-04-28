#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <DHT.h>

// Definición de pines
#define DHTPIN 15      // Sensor DHT11
#define DHTTYPE DHT11
#define FOTO_PIN 34    // Fotorresistencia (debe ser un pin ADC)

// Configuración WiFi
const char* ssid = "INFINITUM8D37"; 
const char* password = "Li8Ea8Xt1v";

// Instancias
Servo servo; 
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Pines de control
int servoPin = 13;
int ledPin = 14;

// Variables globales
float temperatura, humedad;
int valorFoto = 0;

// Página HTML
const char paginahtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        h1 { color: #007BFF; }
        button { padding: 10px; font-size: 20px; margin: 5px; }
    </style>
</head>
<body>
    <h1>ESP32 Web Server</h1>
    <p>ESP32 funcionando como servidor web.</p>
    
    <h2>Control Servomotor</h2>
    <button onclick="fetch('/servo/0')">0°</button>
    <button onclick="fetch('/servo/90')">90°</button>
    <button onclick="fetch('/servo/180')">180°</button>

    <h2>Control Led</h2>
    <button onclick="fetch('/led/encender')">Encender</button>
    <button onclick="fetch('/led/apagar')">Apagar</button>

    <h2>Sensores</h2>
    <button onclick="leerTemperatura()">Leer Temperatura</button>
    <p id="temp">Temperatura: -- °C</p>

    <button onclick="leerHumedad()">Leer Humedad</button>
    <p id="humedad">Humedad: -- %</p>

    <button onclick="leerLuz()">Leer Luz</button>
    <p id="luz">Luz: --</p>

    <script>
        function leerTemperatura() {
            fetch('/sensor/temperatura')
            .then(response => response.json())
            .then(data => document.getElementById('temp').innerText = "Temperatura: " + data.temperatura + " °C");
        }

        function leerHumedad() {
            fetch('/sensor/humedad')
            .then(response => response.json())
            .then(data => document.getElementById('humedad').innerText = "Humedad: " + data.humedad + " %");
        }

        function leerLuz() {
            fetch('/sensor/luz')
            .then(response => response.json())
            .then(data => document.getElementById('luz').innerText = "Luz: " + data.luz);
        }
    </script>
</body>
</html>
)rawliteral";

// Función para manejar la ruta raíz
void root() {
    server.send(200, "text/html", paginahtml);
}

// Funciones para el LED
void encenderLed() {
    digitalWrite(ledPin, HIGH);
    server.send(200, "text/plain", "LED encendido");
}

void apagarLed() {
    digitalWrite(ledPin, LOW);
    server.send(200, "text/plain", "LED apagado");
}

// Funciones para el servomotor
void girarCero() {
    servo.write(0);
    delay(600);
    server.send(200, "text/plain", "Giro de 0°");
}

void girarNoventa() {
    servo.write(90);
    delay(600);
    server.send(200, "text/plain", "Giro de 90°");
}

void girarCientoOchenta() {
    servo.write(180);
    delay(600);
    server.send(200, "text/plain", "Giro de 180°");
}

// Funciones para los sensores
void leerTemperatura() {
    temperatura = dht.readTemperature();
    server.send(200, "application/json", "{\"temperatura\": " + String(temperatura) + "}");
}

void leerHumedad() {
    humedad = dht.readHumidity();
    server.send(200, "application/json", "{\"humedad\": " + String(humedad) + "}");
}

void leerLuz() {
    valorFoto = analogRead(FOTO_PIN);
    server.send(200, "application/json", "{\"luz\": " + String(valorFoto) + "}");
}

void setup() {
    Serial.begin(115200);

    // Conexión WiFi
    WiFi.begin(ssid, password);
    Serial.print("Conectando a la red...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado a la red");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Configurar rutas del servidor
    server.on("/", root);
    server.on("/servo/0", girarCero);
    server.on("/servo/90", girarNoventa);
    server.on("/servo/180", girarCientoOchenta);
    server.on("/led/encender", encenderLed);
    server.on("/led/apagar", apagarLed);
    server.on("/sensor/temperatura", leerTemperatura);
    server.on("/sensor/humedad", leerHumedad);
    server.on("/sensor/luz", leerLuz);

    // Configurar pines
    pinMode(ledPin, OUTPUT);
    pinMode(FOTO_PIN, INPUT);

    // Iniciar dispositivos
    server.begin();
    servo.attach(servoPin);
    dht.begin();
}

void loop() {
    server.handleClient();
}
