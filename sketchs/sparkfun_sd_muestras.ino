#include <Wire.h>

const int micPin = 34;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  pinMode(micPin, INPUT);
  delay(1000);

  Serial.println("mic_signal");
}

void loop() {
  int micSignal = analogRead(micPin);

  Serial.println(micSignal);
  delay(25); // 40Hz
}
