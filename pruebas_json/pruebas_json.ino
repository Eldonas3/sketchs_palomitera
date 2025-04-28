#include <WiFi.h>
#include <HTTPClient.h>

// Configuración de la red
const char* ssid = "INFINITUM8D37";
const char* password = "Li8Ea8Xt1v";    

// petición de los pedidos por hacer
const char* api = "https://aimeetyou.pythonanywhere.com/cola/pedidos/";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conectar a WiFi
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

  // Hacer la petición a la api
  getPedidosPendientes();
}

void loop() {
  // No hacemos nada en el loop, solo una vez al inicio
}

void getPedidosPendientes() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(api);  // Comienza la conexión
    int httpResponseCode = http.GET();  // Realiza la petición GET

    if (httpResponseCode > 0) {
      String response = http.getString();  // Obtiene la respuesta como String
      Serial.println("Respuesta del servidor:");
      Serial.println(response);
    } else {
      Serial.print("Error en la solicitud GET. Código: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Libera recursos
  } else {
    Serial.println("WiFi desconectado");
  }
}
