#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configuración de la red
const char* ssid = "INFINITUM8D37";
const char* password = "Li8Ea8Xt1v";

// Metodos de la api, get y post
const char* serverGet = "https://aimeetyou.pythonanywhere.com/cola/pedidos/";
const char* serverPost = "https://aimeetyou.pythonanywhere.com/cola/actualizar/";

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

  // Procesamos el primer pedido de la lista
  procesarPedido();
}

void loop() {
  // No hacemos nada en el loop, todo en setup por ahora
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
