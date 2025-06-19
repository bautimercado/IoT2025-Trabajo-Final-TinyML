# include <Wire.h>
# include <Adafruit_MPU6050.h>
# include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  if (!mpu.begin()) {
    Serial.println("No se encontro el MPU6050, revisar conexion");
    while (1) yield();
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(1000);

  // Header para EI
  Serial.println("ax,ay,az");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Imprimir aceleracion en CSV
  Serial.print(a.acceleration.x); Serial.print(",");
  Serial.print(a.acceleration.y); Serial.print(",");
  Serial.println(a.acceleration.z);

  delay(100); // 10Hz
}
