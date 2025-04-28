#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// === Servomotores ===
Servo miServo; // Servo olla izquierda
Servo miServo2; // Servo olla derecha
Servo miServo3; // Servo tapa

// Pines
int pinServo = 33; 
int pinServo2 = 32;
int pinServo3 = 2;

#define TRIG_PIN 18
#define ECHO_PIN 19

// Ángulos servomotores
int anguloInicialServo = 0;
int anguloFinalServo = 110;

int anguloInicialServo2 = 180;
int anguloFinalServo2 = 70;

int anguloInicialServo3 = 0;
int anguloFinalServo3 = 90;

// Configuración de WiFi
const char* ssid = "queti";
const char* password = "queti";

// Endpoints de la API
const char* serverGet = "https://aimeetyou.pythonanywhere.com/cola/pedidos/";
const char* serverPost = "https://aimeetyou.pythonanywhere.com/cola/actualizar/";

// === Tareas ===
void TaskServos(void * parameter);
void TaskSensor(void * parameter);
void TaskPedidos(void * parameter);

// === Setup ===
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Iniciar servos
  miServo.attach(pinServo);
  miServo2.attach(pinServo2);
  miServo3.attach(pinServo3);
  miServo.write(anguloInicialServo);
  miServo2.write(anguloInicialServo2);
  miServo3.write(anguloInicialServo3);

  // Iniciar sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Iniciar conexión WiFi
  WiFi.begin(ssid, password);
  Serial.println("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Crear tareas
  xTaskCreatePinnedToCore(TaskServos, "TareaServos", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskSensor, "TareaSensor", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskPedidos, "TareaPedidos", 10000, NULL, 1, NULL, 1);
}

void loop() {
  // Loop vacío porque todo corre en tareas
}

// === Tarea: Girar servos de la olla ===
void TaskServos(void * parameter) {
  for (;;) {
    miServo.write(anguloFinalServo);       // Servo 1 a 110°
    miServo2.write(anguloFinalServo2);      // Servo 2 a 70°
    delay(5000);

    miServo.write(anguloInicialServo);      // Servo 1 a 0°
    miServo2.write(anguloInicialServo2);    // Servo 2 a 180°
    delay(5000);
  }
}

// === Tarea: Leer sensor y mover tapa ===
void TaskSensor(void * parameter) {
  for (;;) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duracion = pulseIn(ECHO_PIN, HIGH);
    float distancia = duracion * 0.034 / 2;

    if (distancia > 0 && distancia < 20) {
      miServo3.write(anguloFinalServo3);
    } else {
      miServo3.write(anguloInicialServo3);
    }

    delay(200); // Verificación rápida
  }
}

// === Tarea: Procesar pedidos ===
void TaskPedidos(void * parameter) {
  for (;;) {
    procesarPedido();
    delay(10000); // Espera antes de consultar nuevos pedidos
  }
}

// === Función: procesarPedido ===
void procesarPedido() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverGet);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Respuesta del servidor:");
      Serial.println(response);

      StaticJsonDocument<2048> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        if (doc.size() > 0) {
          JsonObject pedido = doc[0];
          int id_pedido = pedido["id_pedido"];

          Serial.print("Procesando Pedido ID: ");
          Serial.println(id_pedido);

          // Cambiar a estado 2
          actualizarEstadoPedido(id_pedido, 2);

          // Esperar 5 segundos
          delay(5000);

          // Cambiar a estado 3
          actualizarEstadoPedido(id_pedido, 3);

          Serial.println("Pedido procesado completamente.");
        } else {
          Serial.println("No hay pedidos pendientes.");
        }
      } else {
        Serial.print("Error parseando JSON: ");
        Serial.println(error.c_str());
      }

    } else {
      Serial.print("Error en solicitud GET. Código: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi desconectado");
  }
}

// === Función: actualizarEstadoPedido ===
void actualizarEstadoPedido(int idPedido, int nuevoEstado) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverPost);
    http.addHeader("Content-Type", "application/json");

    String postData = "{\"id_pedido\":" + String(idPedido) + ",\"estado_pedido\":" + String(nuevoEstado) + "}";

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Respuesta POST: ");
      Serial.println(response);
    } else {
      Serial.print("Error en solicitud POST. Código: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi desconectado");
  }
}
