#include <ESP32Servo.h>

// Pines del sensor ultrasónico
#define TRIG_PIN 18
#define ECHO_PIN 19

// Servo
Servo miServo;
int anguloInicialServo = 0;
int anguloFinalServo = 90;
int pinServo = 2;

void setup() {
  Serial.begin(115200);

  // Configurar pines del sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Iniciar servo
  miServo.attach(pinServo);
  miServo.write(anguloInicialServo);
}

void loop() {
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

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // Si hay un objeto cerca, mover el servo
  if (distancia > 0 && distancia < 20) {
    miServo.write(anguloFinalServo);
  } else {
    miServo.write(anguloInicialServo);
  }

  delay(500); // Pequeña pausa
}
