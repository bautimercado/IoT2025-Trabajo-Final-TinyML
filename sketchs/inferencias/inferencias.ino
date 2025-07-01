#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
//#include <ei_run_classifier.h>
#include <iot2025-trabajofinal-ventilador_inferencing.h>

Adafruit_MPU6050 mpu;

#define MIC_PIN 34

// Parametros de muestreo
const uint16_t FREQUENCY_HZ = 100;
const uint16_t INTERVAL_MS = 1000 / FREQUENCY_HZ;
const uint8_t SENSORS_PER_SAMPLE = 4;
const uint16_t NUM_INPUTS = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
const uint16_t ANALYSIS_INTERVAL_MS = 4000;
const uint16_t INFERENCES_PER_WINDOW = 20;
const float CONFIDENCE_THRESHOLD = 0.6f;

// Buffer y variables
float input_buffer[NUM_INPUTS];
uint16_t buffer_index = 0;
unsigned long last_interval = 0;

// Acumuladores para decision final
float category_sums[EI_CLASSIFIER_LABEL_COUNT] = {0};
uint16_t unknown_count = 0;
uint16_t inference_count = 0;
unsigned long last_analysis = 0;

void setup() {
  Serial.begin(115200);
  //Wire.begin(21, 22);
  while (!Serial);
  

  if (!mpu.begin()) {
    Serial.println("❌ No se encontró el MPU6050. Verifica conexión.");
    while(1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);
  //mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  Serial.println("✅ MPU6050 listo para análisis de anomalías.");
  Serial.println("🔊 Micrófono configurado en pin " + String(MIC_PIN));
  Serial.println("📊 Análisis cada 5 segundos a 100Hz");
  Serial.println("🚀 Iniciando captura de datos...\n");

  delay(1000);
}

void loop() {
  if (millis() >= last_interval + INTERVAL_MS) {
    last_interval += INTERVAL_MS;

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    int mic_raw = analogRead(MIC_PIN);
    /*
    if (mic_raw < 0 || mic_raw > 4095) {
      Serial.println("⚠️ Lectura de micrófono fuera de rango");
      mic_raw = 2048; // Valor medio como fallback
    }*/

    input_buffer[buffer_index++] = a.acceleration.x;
    input_buffer[buffer_index++] = a.acceleration.y;
    input_buffer[buffer_index++] = a.acceleration.z;
    input_buffer[buffer_index++] = (float)mic_raw / 4095.0f;

    // Cuando el buffer esta lleno, ejecuta inferencia
    if (buffer_index >= NUM_INPUTS) {
      run_inference();

      // Ventana deslizante: mover datos y seguir
      memmove(input_buffer, input_buffer + SENSORS_PER_SAMPLE, 
              (NUM_INPUTS - SENSORS_PER_SAMPLE) * sizeof(float));
      buffer_index = NUM_INPUTS - SENSORS_PER_SAMPLE;
    }
  }
}

// Callback
int ei_signal_get_data(size_t offset, size_t length, float *out_ptr) {
  for (size_t i = 0; i < length; i++) {
    out_ptr[i] = input_buffer[offset + i];
  }
  return 0;
}

void run_inference() {
  // Senial para clasificador
  signal_t signal;
  signal.total_length = NUM_INPUTS;
  signal.get_data = &ei_signal_get_data;

  // Ejecutar inferencia
  ei_impulse_result_t result = { 0 };
  EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

  if (res != EI_IMPULSE_OK) {
    Serial.print("❌ Error en inferencia: ");
    Serial.println(res);
    return;
  }

  // Encontrar clasificacion con mayor confianza
  float max_confidence = 0;
  size_t max_index = 0;
  for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    if (result.classification[i].value > max_confidence) {
      max_confidence = result.classification[i].value;
      max_index = i;
    }
  }

  // Acumular resutlados si superan el umbral
  if (max_confidence >= CONFIDENCE_THRESHOLD) {
    category_sums[max_index] += 1.0f;

    Serial.print("🔍 Inferencia: ");
    Serial.print(result.classification[max_index].label);
    Serial.print(" (");
    Serial.print(max_confidence, 2);
    Serial.println(")");

    Serial.print("🔍 Debug - Anomally: ");
    Serial.print(result.classification[0].value, 3); // Asumiendo que normal es índice 1
    Serial.print(" | Normal: ");
    Serial.print(result.classification[1].value, 3); // Asumiendo que off es índice 2
    Serial.print(" | Off: ");
    Serial.println(result.classification[2].value, 3);
  } else {
    unknown_count++;
    Serial.println("❓ Inferencia incierta");
  }

  inference_count++;
  // Evaluar resultado final cada 5 segundos
  if (millis() - last_analysis >= ANALYSIS_INTERVAL_MS) {
    if (inference_count > 0) {  // Solo si hay inferencias acumuladas
      evaluate_motor_state();
      last_analysis = millis();
    }
  }
}

void evaluate_motor_state() {
  Serial.println("\n🏭 ===== ANÁLISIS DE ESTADO DEL MOTOR =====");

  float total = static_cast<float>(inference_count);
  float max_avg = 0;
  size_t predicted_state = 0;

  // Calcular promedios y encontrar estado mas probable
  for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    float avg = category_sums[i] / total;
    Serial.print(ei_classifier_inferencing_categories[i]);
    Serial.print(": ");
    Serial.print(avg * 100.0f, 1);
    Serial.println("% | ");

    if (avg > max_avg) {
      max_avg = avg;
      predicted_state = i;
    }
  }

  float unknown_avg = (float)unknown_count / total;
  Serial.print("Incierto: ");
  Serial.print(unknown_avg * 100.0f, 1);
  Serial.println("%");

  // Determinar estado final
  if (unknown_avg > max_avg) {
    Serial.println("🤔 ESTADO: INCIERTO - Requiere verificación manual");
  } else {
    String state_name = String(ei_classifier_inferencing_categories[predicted_state]);
    
    if (state_name.equals("anomally")) {
      Serial.println("🚨 ESTADO: ANOMALÍA DETECTADA - Revisar motor inmediatamente!");
      Serial.println("⚠️  Posibles causas: desbalanceo, desgaste, falla mecánica, etc.");
    } else if (state_name.equals("off")) {
      Serial.println("⏹️  ESTADO: MOTOR APAGADO");
    } else if (state_name.equals("normal")) {
      Serial.println("✅ ESTADO: FUNCIONAMIENTO NORMAL");
    } else {
      Serial.println("❓ ESTADO: " + state_name + " (etiqueta no reconocida)");
    }
    
    Serial.print("🎯 Confianza: ");
    Serial.print(max_avg * 100.0f, 1);
    Serial.println("%");
  }

  Serial.println("==========================================\n");

  // Reiniciar contadores
  memset(category_sums, 0, sizeof(category_sums));
  inference_count = 0;
  unknown_count = 0;
}
