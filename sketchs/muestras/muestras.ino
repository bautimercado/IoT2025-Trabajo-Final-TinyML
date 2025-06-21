# include <Wire.h>
# include <Adafruit_MPU6050.h>
# include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

# define MIC_PIN 34

unsigned long prevMillis = 0;
const int sampleInterval = 10; // 100Hz

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!mpu.begin()) {
    Serial.println("Error al detectar MPU6050. Verificar conexion.");
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  delay(100);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis >= sampleInterval) {
    prevMillis = currentMillis;

    sensors_event_t a, g, temp;
    bool success = mpu.getEvent(&a, &g, &temp);

    if (!success) {
      Serial.println("⚠️ Error al leer el MPU6050. Reintentando...");
      Wire.end();
      delay(100);
      Wire.begin();
      mpu.begin();
    }

    int mic_raw = analogRead(MIC_PIN);

    // accX, accY, accZ, mic_signal
    Serial.print(a.acceleration.x);
    Serial.print(',');
    Serial.print(a.acceleration.y);
    Serial.print(',');
    Serial.print(a.acceleration.z);
    Serial.print(',');
    Serial.println(mic_raw);
  }
}
