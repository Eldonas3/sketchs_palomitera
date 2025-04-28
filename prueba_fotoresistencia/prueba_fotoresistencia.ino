int ft = 4;
int valorft = 0;
void setup() {
pinMode(ft,INPUT);
Serial.begin(115200);
}

void loop() {
valorft = analogRead(ft);
Serial.println(valorft);
delay(500);
}
