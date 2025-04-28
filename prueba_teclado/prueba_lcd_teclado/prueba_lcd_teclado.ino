#include <Keypad.h>
#include <LiquidCrystal.h>

// Configuración de la pantalla LCD (RS, EN, D4, D5, D6, D7)
LiquidCrystal My_LCD(13, 12, 14, 27, 26, 25);

// Configuración del teclado matricial
const byte FILAS = 4;
const byte COLUMNAS = 4;

// Mapeo de teclas en el teclado 4x4
char teclas[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Pines de la ESP32 conectados al teclado
byte pinesFilas[FILAS] = {17, 16, 4, 15}; 
byte pinesColumnas[COLUMNAS] = {21, 19, 18, 5}; 

// Inicializar teclado
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

String codigo = "";  // Almacena la secuencia de números

void setup() {
  Serial.begin(115200);
  My_LCD.begin(16, 2);  // Inicializar LCD
  My_LCD.clear();
  My_LCD.print("Codigo:");
  My_LCD.setCursor(0, 1);
}

void loop() {
  char tecla = teclado.getKey(); // Leer la tecla presionada

  if (tecla) { // Si hay una tecla presionada
    if (isdigit(tecla)) { // Solo aceptar números
      if (codigo.length() < 16) { // No desbordar la pantalla
        codigo += tecla;
      } else { 
        codigo = tecla; // Si llena la pantalla, reiniciar
      }
      
      Serial.println(codigo); // Mostrar en el monitor serial
      
      My_LCD.clear();
      My_LCD.setCursor(0, 0);
      My_LCD.print("Codigo:");
      My_LCD.setCursor(0, 1);
      My_LCD.print(codigo);
    }
  }
}
