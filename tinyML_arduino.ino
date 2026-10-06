#include <Wire.h>
#include <SparkFun_MMA8452Q.h>
#include <gestures0_inferencing.h> 

MMA8452Q accel;

// Configuración del modelo
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    
    if (accel.begin(Wire, 0x1C) == false) {
        Serial.println("Sensor no encontrado");
        while (1);
    }
    Serial.println("Sistema de Reconocimiento de Gestos Listo!");
}

void loop() {
    // 1. Llenado de buffer con datos del acelerómetro
    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 3) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);
        
        accel.read();
        features[ix + 0] = accel.cx;
        features[ix + 1] = accel.cy;
        features[ix + 2] = accel.cz;

        // Espera para mantener la frecuencia de muestreo exacta del modelo
        while (micros() < next_tick) { /* delay preciso */ }
    }

    // 2. Ejecución de la inferencia (el modelo analiza el buffer)
    ei_impulse_result_t result = { 0 };
    signal_t signal;
    numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
    if (res != EI_IMPULSE_OK) return;

    // 3. Imprime el resultado con mayor probabilidad
    Serial.print("Gesto detectado: ");
    int mejor_indice = 0;
    float mejor_score = 0;

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if (result.classification[ix].value > mejor_score) {
            mejor_score = result.classification[ix].value;
            mejor_indice = ix;
        }
    }
    
    // Solo se muestra si la confianza es alta (más del 70%)
    if (mejor_score > 0.7) {
        Serial.print(result.classification[mejor_indice].label);
        Serial.print(" (Confianza: ");
        Serial.print(mejor_score);
        Serial.println(")");
    } else {
        Serial.println("Incierto...");
    }

    delay(500); // Pausa breve antes de la siguiente detección
}