## 🧠 IoT 2025 - Trabajo Final Integrador

**Proyecto:** Detección de Anomalías en Motores usando TinyML con ESP32  
**Materia:** Internet de las Cosas – Facultad de Informática, UNLP  
**Alumno:** Mercado Bautista – Legajo 18477/0

---

### 📄 Descripción

Este proyecto tiene como objetivo detectar el estado de funcionamiento de un motor (apagado, normal o anómalo) usando sensores físicos (MPU6050 + SparkFun Sound Detector) y un modelo de Machine Learning embebido en un ESP32. El modelo fue entrenado con Edge Impulse y se ejecuta localmente, sin necesidad de conexión a Internet.


### 🔗 Enlaces útiles

- 📊 [Presentación del proyecto](https://docs.google.com/presentation/d/1oroHeTBnVjyxfnDnKWNkgLF05Cyge2w4DXz2BgdgEfI/edit?usp=sharing)
- 🧠 [Proyecto en Edge Impulse](https://studio.edgeimpulse.com/public/727889/live)
- 📄 [Informe en PDF](./informe.pdf)


### 📁 Contenido del repositorio

- `/sketchs/` – Código fuente en Arduino (recolección de muestras + inferencias)
- `/muestras/` – Muestras utilizadas para entrenamiento y validación
- `/model/` – Exportación del modelo TinyML desde Edge Impulse
- `informe.pdf` – Informe completo del trabajo
- `presentacion.pdf` - Presentación usada en la defensa y exposición del proyecto

### 🛠️ Tecnologías utilizadas

- ESP32 + Arduino IDE  
- Sensor MPU6050 (I2C)  
- SparkFun Sound Detector (analógico)  
- Edge Impulse (plataforma para el entrenamiento del modelo)  
- edge-impulse-data-forwarder

### 📌 Notas

- El modelo embebido infiere a 100Hz, acumula los datos sensados en un buffer circular y clasifica en base a umbral de confianza (60%).
- El sistema fue validado con un ventilador común simulado con distintas anomalías mecánicas.
