#include <Wire.h>
#include "SparkFun_MMA8452Q.h"

MMA8452Q accel;

void setup() {
  Serial.begin(115200); // Velocidad estándar para Edge Impulse
  Wire.begin(21, 22);
  if (accel.begin(Wire, 0x1C) == false) {
    while (1); 
  }
}

void loop() {
  if (accel.available()) {
    accel.read();
    // Formato simple: X,Y,Z
    Serial.print(accel.cx, 4);
    Serial.print(",");
    Serial.print(accel.cy, 4);
    Serial.print(",");
    Serial.println(accel.cz, 4);
  }
  delay(10); // Frecuencia de 100Hz (ideal para gestos rápidos)
}