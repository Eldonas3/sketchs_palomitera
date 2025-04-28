#include <DHT.h>
#define DHTPIN 15
#define DHTTYPE DHT11

//iniciamos el sensor
DHT dht(DHTPIN,DHTTYPE);

//si
float temperatura;
float humedad;
float indice;

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();

  if(isnan(humedad) || isnan(temperatura)){
   Serial.print(F("error de lectura del sensor"));
   return;  
  }

  indice = dht.computeHeatIndex(temperatura,humedad,false); 

  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.print("% Temperatura:   ");
  Serial.print("°C");
}
