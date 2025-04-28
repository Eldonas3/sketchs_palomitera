#include <ESP32Servo.h>

Servo miServo;

int anguloInicialServo = 0;
int anguloFinalServo = 90;
int pinServo = 2;
void setup() {
  miServo.attach(pinServo);
  miServo.write(anguloInicialServo);
}

void loop() {
  miServo.write(anguloFinalServo);
  delay(3000); 
  miServo.write(anguloInicialServo);
  delay(3000);
}
