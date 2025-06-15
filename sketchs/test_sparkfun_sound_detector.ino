const int micPin = 34;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int micValue = analogRead(micPin);
  Serial.print("Nivel de sonido (ENVELOPE): ");
  Serial.println(micValue);
  delay(1000);
}
