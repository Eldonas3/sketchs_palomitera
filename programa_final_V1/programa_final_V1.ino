// === LIBRERIAS ===
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// === SERVOMOTORES ===
Servo miServo;   // Olla izquierda
Servo miServo2;  // Olla derecha
Servo miServo3;  // Tapa

// === PINES ===
int pinServo = 33;
int pinServo2 = 32;
int pinServo3 = 2;

#define TRIG_PIN 18     // Sensor tapa
#define ECHO_PIN 19
#define TRIG2_PIN 25    // Sensor palomitas
#define ECHO2_PIN 26
#define RELE_PIN 27     // Relevador

// === ANGULOS ===
int anguloInicialServo = 0;
int anguloFinalServo = 110;
int anguloInicialServo2 = 180;
int anguloFinalServo2 = 70;
int anguloInicialServo3 = 0;
int anguloFinalServo3 = 90;

// === WIFI Y API ===
const char* ssid = "uwuntu";
const char* password = "1234567890";
const char* serverGet = "https://aimeetyou.pythonanywhere.com/cola/pendientes/";
const char* serverPost = "https://aimeetyou.pythonanywhere.com/cola/actualizar/";

// === TAREAS ===
void TaskServos(void *);
void TaskSensorTapa(void *);
void TaskRele(void *);
void TaskPedidos(void *);

// === FUNCIONES ===
void procesarPedido();
void actualizarEstadoPedido(int, int);
float medirDistancia(int trig, int echo);

void setup() {
  Serial.begin(115200);

  // Servos
  miServo.attach(pinServo);
  miServo2.attach(pinServo2);
  miServo3.attach(pinServo3);
  miServo.write(anguloInicialServo);
  miServo2.write(anguloInicialServo2);
  miServo3.write(anguloInicialServo3);

  // Pines sensores y relevador
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG2_PIN, OUTPUT);
  pinMode(ECHO2_PIN, INPUT);
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, LOW);

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado: " + WiFi.localIP().toString());

  // TAREAS
  xTaskCreatePinnedToCore(TaskServos, "Servos", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskSensorTapa, "SensorTapa", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskRele, "Rele", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskPedidos, "Pedidos", 10000, NULL, 1, NULL, 1);
}

void loop() {}

// === TAREA: Servos de olla ===
void TaskServos(void * parameter) {
  for (;;) {
    miServo.write(anguloFinalServo);
    miServo2.write(anguloFinalServo2);
    delay(5000);
    miServo.write(anguloInicialServo);
    miServo2.write(anguloInicialServo2);
    delay(5000);
  }
}

// === TAREA: Sensor de tapa ===
void TaskSensorTapa(void * parameter) {
  for (;;) {
    float d = medirDistancia(TRIG_PIN, ECHO_PIN);
    if (d > 0 && d < 5) {
      miServo3.write(anguloFinalServo3);
    } else {
      miServo3.write(anguloInicialServo3);
    }
    delay(200);
  }
}

// === TAREA: Sensor palomitas y rele ===
void TaskRele(void * parameter) {
  bool calentando = false;
  unsigned long inicio = 0;
  for (;;) {
    float d = medirDistancia(TRIG2_PIN, ECHO2_PIN);
    if (d > 4.0) { //distancia minimia para detectar palomitas
      if (!calentando) {
        digitalWrite(RELE_PIN, HIGH);
        calentando = true;
        inicio = millis();
        Serial.println("[RELE] Activado por falta de palomitas");
      }
    } else {
      if (calentando && millis() - inicio >= 20000UL) { // mínimo 20 segundos
        digitalWrite(RELE_PIN, LOW);
        calentando = false;
        Serial.println("[RELE] Desactivado, suficientes palomitas");
      }
    }
    delay(1000);
  }
}

// === TAREA: Procesamiento de pedidos ===
void TaskPedidos(void * parameter) {
  for (;;) {
    procesarPedido();
    delay(20000); // espera antes del siguiente
  }
}

void procesarPedido() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverGet);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String res = http.getString();
      StaticJsonDocument<2048> doc;
      DeserializationError err = deserializeJson(doc, res);
      if (!err && doc.size() > 0) {
        JsonObject pedido = doc[0];
        int id = pedido["id_pedido"];
        int cantidad = pedido["palomitas"];
        Serial.printf("[PEDIDO] ID: %d, Palomitas: %d\n", id, cantidad);

        // Esperar detección por 3s
        int detectados = 0;
        for (int i = 0; i < 15; i++) {
          float d = medirDistancia(TRIG_PIN, ECHO_PIN);
          if (d > 0 && d < 20) detectados++;
          delay(200);
        }
        if (detectados >= 10) {
          actualizarEstadoPedido(id, 2);
          miServo3.write(anguloFinalServo3);
          int tiempo = (cantidad == 1) ? 5000 : (cantidad == 2) ? 6000 : (cantidad == 3) ? 7000 : 5000;
          delay(tiempo);
          miServo3.write(anguloInicialServo3);
          delay(500);
          actualizarEstadoPedido(id, 3);
        } else {
          Serial.println("[PEDIDO] No se detectó presencia durante 3s continuos");
        }
      }
    }
    http.end();
  }
}

void actualizarEstadoPedido(int id, int estado) {
  HTTPClient http;
  http.begin(serverPost);
  http.addHeader("Content-Type", "application/json");
  String json = "{\"id_pedido\":" + String(id) + ",\"estado_pedido\":" + String(estado) + "}";
  int code = http.POST(json);
  String res = http.getString();
  Serial.printf("[POST] Estado %d -> %s\n", estado, res.c_str());
  http.end();
}

float medirDistancia(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duracion = pulseIn(echo, HIGH);
  return duracion * 0.034 / 2;
}
