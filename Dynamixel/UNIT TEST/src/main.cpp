#include <Arduino.h>

#define TRIG PB12
#define ECHO PB13

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT_PULLDOWN);
  digitalWrite(TRIG, LOW);
  delay(2000);
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  uint32_t dur = pulseIn(ECHO, HIGH, 25000);

  if (dur == 0) Serial.println("TIMEOUT");
  else { Serial.print(dur / 58); Serial.print(" cm  ("); Serial.print(dur); Serial.println(" us)"); }

  delay(200);
}
