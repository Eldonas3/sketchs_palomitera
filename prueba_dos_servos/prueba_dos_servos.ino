#include <ESP32Servo.h>

Servo miServo;
Servo miServo2;
//Servo miServo3;

int anguloInicialServo = 0;
int anguloFinalServo = 110;

int anguloInicialServo2 = 180;
int anguloFinalServo2 = 70;

//int anguloInicialServo3 = 0;
//int anguloFinalServo3 = 90;

int pinServo = 33; 
int pinServo2 = 32;

void setup() {
  miServo.attach(pinServo);
  miServo2.attach(pinServo2);
  
  miServo.write(anguloInicialServo);     // Servo 1 inicia en 0°
  miServo2.write(anguloInicialServo2);   // Servo 2 inicia en 180°
}

void loop() {
  miServo.write(anguloFinalServo);       // Servo 1 a 110°
  miServo2.write(anguloFinalServo2);     // Servo 2 a 70°
  delay(5000);                           

  miServo.write(anguloInicialServo);     // Servo 1 a 0°
  miServo2.write(anguloInicialServo2);   // Servo 2 a 180°
  delay(5000);                           
}
