#include <Keypad.h>

const byte FILAS = 4;  // Número de filas del teclado
const byte COLUMNAS = 4;  // Número de columnas del teclado

// Definir la disposición de teclas en el teclado 4x4
char teclas[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Pines de la ESP32 conectados a las filas y columnas del teclado
byte pinesFilas[FILAS] = {15, 2, 4, 5}; 
byte pinesColumnas[COLUMNAS] = {18, 19, 21, 22}; 

// Inicializar el objeto Keypad
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

void setup() {
  Serial.begin(115200);  // Iniciar comunicación serial
}

void loop() {
  char tecla = teclado.getKey();  // Leer la tecla presionada
  if (tecla) {  // Si se presiona una tecla
    Serial.print("Tecla presionada: ");
    Serial.println(tecla);
  }
}
