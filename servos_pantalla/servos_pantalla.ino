#include <ESP32Servo.h>
#include <LiquidCrystal.h>
 //Configuración de la pantalla LCD (RS,EN,D4,D5,D6,D7)
 LiquidCrystal My_LCD(13,12,14,27,26,25);
static const int servoPin = 33;
Servo servo;

void setup() {
  Serial.begin(115200);
  My_LCD.begin(16,2);
  My_LCD.clear();
  My_LCD.print("Grados:");
  My_LCD.setCursor(0,1);
  servo.attach(servoPin);
}

void loop() {
  for(int posDegress = 0; posDegress <= 180; posDegress += 20){
    servo.write(posDegress);
    My_LCD.clear();
    My_LCD.setCursor(0,0);
    My_LCD.print("Codigo:");
    My_LCD.setCursor(0,1);
    My_LCD.print(posDegress);
    delay(500);
    }
   
    for(int posDegress = 180; posDegress >= 0; posDegress -= 20){
    servo.write(posDegress);
    My_LCD.clear();
    My_LCD.setCursor(0,0);
    My_LCD.print("Codigo:");
    My_LCD.setCursor(0,1);
    My_LCD.print(posDegress);
    delay(500);
    }
}
