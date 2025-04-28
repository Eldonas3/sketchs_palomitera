#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configuración de la red
const char* ssid = "INFINITUM8D37";
const char* password = "Li8Ea8Xt1v";

// petición de los pedidos por hacer
const char* api = "https://aimeetyou.pythonanywhere.com/cola/pedidos/";

void setup() {
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

  // Hacer la petición al servidor
  getPedidosPendientes();
}

void loop() {
  // No hacemos nada en el loop, solo una vez al inicio
}

void getPedidosPendientes() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(api);  // Conexión con la api
    int httpResponseCode = http.GET();  // Realiza la petición GET

    if (httpResponseCode > 0) {
      String response = http.getString();  // Obtiene la respuesta como String
      Serial.println("Respuesta del servidor:");
      Serial.println(response);

      // Parsear el JSON
      StaticJsonDocument<2048> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        if (doc.size() > 0) {
          for (JsonObject pedido : doc.as<JsonArray>()) {
            int id_pedido = pedido["id_pedido"];
            int estado_pedido = pedido["estado_pedido"];
            Serial.print("Pedido encontrado - ID: ");
            Serial.print(id_pedido);
            Serial.print(", Estado: ");
            Serial.println(estado_pedido);
          }
        } else {
          Serial.println("No hay pedidos pendientes.");
        }
      } else {
        Serial.print("Error parseando JSON: ");
        Serial.println(error.c_str());
      }

    } else {
      Serial.print("Error en la solicitud GET. Código: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Libera recursos
  } else {
    Serial.println("WiFi desconectado");
  }
}
