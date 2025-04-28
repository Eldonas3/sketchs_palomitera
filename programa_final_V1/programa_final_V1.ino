#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

//Servomotores
Servo miServo; //servo olla izq
Servo miServo2;//servo olla der
Servo miServo3;//servo tapa

//Pines componentes
int pinServo = 33; 
int pinServo2 = 32;
int pinServo3 = 2;

#define TRIG_PIN 18
#define ECHO_PIN 19

//angulos servomotores
int anguloInicialServo = 0;
int anguloFinalServo = 110;

int anguloInicialServo2 = 180;
int anguloFinalServo2 = 70;

int anguloIncialServo3 = 0;
int anguloFinalServo3 = 90;

// Configuración de la red
const char* ssid = "INFINITUM8D37";
const char* password = "Li8Ea8Xt1v";

// Metodos de la api, get y post
const char* serverGet = "https://aimeetyou.pythonanywhere.com/cola/pedidos/";
const char* serverPost = "https://aimeetyou.pythonanywhere.com/cola/actualizar/";

void setup() {
  //Iniciamos componentes primero
  //Servomotores primero
  miServo.attach(pinServo);
  miServo2.attach(pinServo2);
  miServo3.attach(pinServo3);
  miServo.write(anguloInicialServo);     // Servo 1 inicia en 0°
  miServo2.write(anguloInicialServo2);   // Servo 2 inicia en 180°  
  miServo3.write(anguloInicialServo3);
  
  // Configurar pines del sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  //Iniciamos la conexión wifi
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.println("Conectando a WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Procesamos el primer pedido de la lista
  procesarPedido();
}

void loop() {
  //En el loop se ejuta todo lo relacionado a componentes
  //Giran los servos de la olla
  miServo.write(anguloFinalServo);       // Servo 1 a 110°
  miServo2.write(anguloFinalServo2);     // Servo 2 a 70°
  delay(5000);                           

  miServo.write(anguloInicialServo);     // Servo 1 a 0°
  miServo2.write(anguloInicialServo2);   // Servo 2 a 180°
  delay(5000); 

  // Enviar pulso ultrasónico
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Medir duración del eco
  long duracion = pulseIn(ECHO_PIN, HIGH);
  
  // Calcular distancia en cm
  float distancia = duracion * 0.034 / 2;

  //Si hay un objeto cerca, movemos el servo de la tapa
  if (distancia > 0 && distancia < 20) {
    miServo3.write(anguloFinalServo3);
  } else {
    miServo3.write(anguloInicialServo3);
  }

  delay(500); // Pequeña pausa
}

void procesarPedido() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverGet);  // Obtener pedidos pendientes
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Respuesta del servidor:");
      Serial.println(response);

      StaticJsonDocument<2048> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        if (doc.size() > 0) {
          // Tomamos el primer pedido
          JsonObject pedido = doc[0];
          int id_pedido = pedido["id_pedido"];

          Serial.print("Procesando Pedido ID: ");
          Serial.println(id_pedido);

          // 1. Cambiar a estado 2 (preparando)
          actualizarEstadoPedido(id_pedido, 2);

          // 2. Esperar 5 segundos
          delay(5000);

          // 3. Cambiar a estado 3 (listo)
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

//Actualizamos el pedido de preparado a listo
void actualizarEstadoPedido(int idPedido, int nuevoEstado) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverPost);
    http.addHeader("Content-Type", "application/json");

    // Crear el JSON para enviar
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
