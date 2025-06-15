// Sketch de prueba para el MPU6050
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  if (!mpu.begin()) {
    Serial.println("No se encontro el MPU6050 :(");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 encontrado!");
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Acelerómetro: X="); Serial.print(a.acceleration.x);
  Serial.print(" m/s^2, Y="); Serial.print(a.acceleration.y);
  Serial.print(" m/s^2, Z="); Serial.println(a.acceleration.z);

  delay(500);
}
