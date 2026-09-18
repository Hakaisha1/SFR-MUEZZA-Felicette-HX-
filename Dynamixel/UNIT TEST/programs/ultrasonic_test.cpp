#include <Arduino.h>

#define TRIG PB12
#define ECHO PB13

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT_PULLDOWN);
  digitalWrite(TRIG, LOW);
  delay(2000);
  Serial.println("=== ULTRASONIC DEBUG ===");
  Serial.println("TRIG=PB12 ECHO=PB13");
  Serial.print("ECHO pin sekarang: "); Serial.println(digitalRead(ECHO) ? "HIGH" : "LOW");
  Serial.println();
}

void loop() {
  // Baca state ECHO sebelum trigger
  bool pre = digitalRead(ECHO);

  // Trigger
  digitalWrite(TRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // Tunggu ECHO naik (max 10ms)
  uint32_t t0 = micros();
  while (digitalRead(ECHO) == LOW) {
    if (micros() - t0 > 10000) break;  // 10ms timeout menunggu rising edge
  }

  if (digitalRead(ECHO) == LOW) {
    // ECHO tidak pernah naik
    Serial.print("pre="); Serial.print(pre ? "H" : "L");
    Serial.println("  ECHO TIDAK NAIK — sensor tidak merespon trigger");
    delay(500);
    return;
  }

  // Ukur berapa lama ECHO HIGH
  uint32_t start = micros();
  while (digitalRead(ECHO) == HIGH) {
    if (micros() - start > 30000) break;  // 30ms max
  }
  uint32_t dur = micros() - start;

  // Tampilkan
  Serial.print("pre="); Serial.print(pre ? "H" : "L");
  Serial.print("  dur="); Serial.print(dur); Serial.print("us");
  if (dur < 30000) {
    Serial.print("  jarak="); Serial.print(dur / 58); Serial.print("cm");
  } else {
    Serial.print("  TIMEOUT");
  }
  Serial.println();

  delay(200);
}
