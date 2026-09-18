#include <Arduino.h>
#include <Servo.h>
#include <math.h>

// ==========================================
// PONYTAIL: SINGLE FILE, MINIMAL ABSTRACTION
// ==========================================

HardwareSerial dxlR(PA9); // Kanan
HardwareSerial dxlL(PA2); // Kiri

Servo grip1; // PB8
Servo grip2; // PB9

#define TRIG_PIN PB12
#define ECHO_BACK PB13
#define ECHO_LEFT PB14
#define ECHO_RIGHT PA10
#define ECHO_FRONT PA15
#define BTN_PIN PB3

// --- ULTRASONIC INTERRUPTS ---
volatile uint32_t us_start[4] = {0,0,0,0};
volatile uint16_t us_dist[4] = {9999,9999,9999,9999}; // Front, Back, Left, Right

void exti_front() {
  if (digitalRead(ECHO_FRONT)) us_start[0] = micros();
  else { uint32_t d = micros() - us_start[0]; us_dist[0] = (d > 30000) ? 9999 : d / 58; }
}
void exti_back() {
  if (digitalRead(ECHO_BACK)) us_start[1] = micros();
  else { uint32_t d = micros() - us_start[1]; us_dist[1] = (d > 30000) ? 9999 : d / 58; }
}
void exti_left() {
  if (digitalRead(ECHO_LEFT)) us_start[2] = micros();
  else { uint32_t d = micros() - us_start[2]; us_dist[2] = (d > 30000) ? 9999 : d / 58; }
}
void exti_right() {
  if (digitalRead(ECHO_RIGHT)) us_start[3] = micros();
  else { uint32_t d = micros() - us_start[3]; us_dist[3] = (d > 30000) ? 9999 : d / 58; }
}

struct JointLimits { uint16_t min, center, max; };
struct LegLimits { JointLimits coxa, femur, tibia; };

struct Leg {
  uint8_t ids[3]; 
  HardwareSerial* port;
  LegLimits lim;
};

// Urutan: L_Back, L_Mid, L_Front, R_Back, R_Mid, R_Front
Leg legs[6] = {
  {{1, 2, 3}, &dxlL, {{550, 512, 1000}, {400, 512, 700}, {450, 550, 700}}},
  {{4, 5, 6}, &dxlL, {{400, 512, 600}, {300, 512, 700}, {400, 512, 700}}},
  {{7, 8, 9}, &dxlL, {{400, 600, 700}, {350, 512, 700}, {400, 512, 1000}}},
  {{10, 11, 12}, &dxlR, {{400, 512, 700}, {300, 550, 700}, {400, 512, 1000}}},
  {{20, 21, 22}, &dxlR, {{400, 512, 700}, {300, 512, 700}, {400, 512, 1000}}},
  {{30, 31, 32}, &dxlR, {{300, 512, 700}, {300, 512, 700}, {400, 512, 1000}}}
};

void dxlWrite(HardwareSerial* port, uint8_t id, uint8_t addr, uint16_t value, uint8_t bytes) {
  uint8_t len = (bytes == 2) ? 5 : 4;
  uint8_t checksum = ~(id + len + 3 + addr + (value & 0xFF) + ((bytes == 2) ? (value >> 8) : 0));
  
  while (port->available()) port->read();
  port->write(0xFF); port->write(0xFF);
  port->write(id); port->write(len); port->write(3);
  port->write(addr);
  port->write(value & 0xFF);
  if (bytes == 2) port->write(value >> 8);
  port->write(checksum);
  
  port->flush();
  port->enableHalfDuplexRx();
}

uint16_t clampDxl(float val, JointLimits l) {
  uint16_t raw = (uint16_t)val;
  if (raw < l.min) return l.min;
  if (raw > l.max) return l.max;
  return raw;
}

uint16_t degToDxl(float deg, JointLimits l) {
  float raw = l.center + (deg / 0.2929f);
  return clampDxl(raw, l);
}

// --- KINEMATICS (IK) ---
const float L_COXA = 65.0f;
const float L_FEMUR = 45.0f;
const float L_TIBIA = 70.0f;

bool calculateIK(float x, float y, float z, float* angles) {
  angles[0] = atan2(x, y) * 180.0f / PI; // coxa
  float L = sqrt(x*x + y*y) - L_COXA;
  float d = sqrt(L*L + z*z);
  
  if (d > (L_FEMUR + L_TIBIA)) return false; 
  
  float alpha = atan2(z, L);
  float beta = acos(constrain((L_FEMUR*L_FEMUR + d*d - L_TIBIA*L_TIBIA) / (2*L_FEMUR*d), -1.0f, 1.0f));
  angles[1] = -(alpha + beta) * 180.0f / PI; // femur (INVERTED: negative math angle = positive raw = DOWN)
  
  float gamma = acos(constrain((L_FEMUR*L_FEMUR + L_TIBIA*L_TIBIA - d*d) / (2*L_FEMUR*L_TIBIA), -1.0f, 1.0f));
  angles[2] = (gamma * 180.0f / PI) - 90.0f; // tibia
  return true;
}

// --- STATE VARIABLES ---
uint8_t robot_state = 0; // 0=IDLE, 1=WALK, 2=EVACUATE, 3=ESTOP
float cmd_vel_x = 0, cmd_vel_y = 0, cmd_vel_yaw = 0;
bool test_mode = false; // Mute binary telemetry during text terminal testing

// --- UART PARSER ---
void parseSerialCDC() {
  static uint8_t state = 0, len = 0, msg_id = 0, chk = 0, p_idx = 0;
  static uint8_t payload[32];

  while (Serial.available()) {
    uint8_t b = Serial.read();
    if (state == 0) {
      if (b == 0xAA) { state = 1; test_mode = false; } // Kembali ke mode Integrasi Biner
      // === MANUAL SERIAL MONITOR OVERRIDE UNTUK TESTING ===
      else if (b == 'w' || b == 'W') { test_mode = true; cmd_vel_x = 0.1f; cmd_vel_y = 0; cmd_vel_yaw = 0; robot_state = 1; Serial.println("TEST: MAJU (X+)"); }
      else if (b == 's' || b == 'S') { test_mode = true; cmd_vel_x = -0.1f; cmd_vel_y = 0; cmd_vel_yaw = 0; robot_state = 1; Serial.println("TEST: MUNDUR (X-)"); }
      else if (b == 'a' || b == 'A') { test_mode = true; cmd_vel_x = 0; cmd_vel_y = 0.1f; cmd_vel_yaw = 0; robot_state = 1; Serial.println("TEST: KIRI (Y+)"); }
      else if (b == 'd' || b == 'D') { test_mode = true; cmd_vel_x = 0; cmd_vel_y = -0.1f; cmd_vel_yaw = 0; robot_state = 1; Serial.println("TEST: KANAN (Y-)"); }
      else if (b == 'q' || b == 'Q') { test_mode = true; cmd_vel_x = 0; cmd_vel_y = 0; cmd_vel_yaw = 0.5f; robot_state = 1; Serial.println("TEST: ROTASI KIRI (Yaw+)"); }
      else if (b == 'e' || b == 'E') { test_mode = true; cmd_vel_x = 0; cmd_vel_y = 0; cmd_vel_yaw = -0.5f; robot_state = 1; Serial.println("TEST: ROTASI KANAN (Yaw-)"); }
      else if (b == ' ')             { test_mode = true; cmd_vel_x = 0; cmd_vel_y = 0; cmd_vel_yaw = 0; robot_state = 1; Serial.println("TEST: STOP (Berdiri IDLE)"); }
      else if (b == 'x' || b == 'X') { test_mode = true; robot_state = 3; Serial.println("TEST: EMERGENCY STOP (Torque Lepas)"); }
      
      // === DEBUG CAPIT (GRIPPER & ARM) ===
      else if (b == 'u' || b == 'U') { 
        test_mode = true; 
        JointLimits arm_lim = {0, 512, 1023};
        dxlWrite(&dxlR, 0, 30, degToDxl(30, arm_lim), 2); // Pitch Up (+30 deg)
        Serial.println("TEST: ARM PITCH UP (ID 0)");
      }
      else if (b == 'j' || b == 'J') { 
        test_mode = true; 
        JointLimits arm_lim = {0, 512, 1023};
        dxlWrite(&dxlR, 0, 30, degToDxl(-30, arm_lim), 2); // Pitch Down (-30 deg)
        Serial.println("TEST: ARM PITCH DOWN (ID 0)");
      }
      else if (b == 'i' || b == 'I') { 
        test_mode = true; 
        JointLimits arm_lim = {0, 512, 1023};
        dxlWrite(&dxlR, 33, 30, degToDxl(30, arm_lim), 2); // Yaw Left (+30 deg)
        Serial.println("TEST: ARM YAW LEFT (ID 33)");
      }
      else if (b == 'k' || b == 'K') { 
        test_mode = true; 
        JointLimits arm_lim = {0, 512, 1023};
        dxlWrite(&dxlR, 33, 30, degToDxl(-30, arm_lim), 2); // Yaw Right (-30 deg)
        Serial.println("TEST: ARM YAW RIGHT (ID 33)");
      }
      else if (b == 'o' || b == 'O') {
        test_mode = true; grip1.write(90); Serial.println("TEST: CLAW OPEN (PB8)");
      }
      else if (b == 'l' || b == 'L') {
        test_mode = true; grip1.write(180); Serial.println("TEST: CLAW CLOSE (PB8)");
      }
    }
    else if (state == 1 && b == 0x55) state = 2;
    else if (state == 2) { len = b; state = 3; }
    else if (state == 3) { msg_id = b; chk = b; p_idx = 0; state = (len > 0) ? 4 : 5; }
    else if (state == 4) {
      payload[p_idx++] = b; chk ^= b;
      if (p_idx >= len) state = 5;
    }
    else if (state == 5) {
      if (b == chk) { // Checksum OK
        if (msg_id == 0x10 && len == 12) {
          memcpy(&cmd_vel_x, &payload[0], 4);
          memcpy(&cmd_vel_y, &payload[4], 4);
          memcpy(&cmd_vel_yaw, &payload[8], 4);
        } else if (msg_id == 0x11 && len == 9) {
          float arm1, arm2; uint8_t grip;
          memcpy(&arm1, &payload[0], 4);
          memcpy(&arm2, &payload[4], 4);
          grip = payload[8];
          
          grip1.write((grip == 1 || grip == 2) ? 180 : 90); // Capit PWM
          
          // Tulis ke Dynamixel Gripper di Bus Kanan (dxlR / PA9)
          JointLimits arm_lim = {0, 512, 1023};
          uint16_t p_raw = degToDxl(arm1, arm_lim);
          uint16_t y_raw = degToDxl(arm2, arm_lim);
          dxlWrite(&dxlR, 0, 30, p_raw, 2);
          dxlWrite(&dxlR, 33, 30, y_raw, 2);
          
        } else if (msg_id == 0x12 && len == 1) {
          robot_state = payload[0];
        }
      }
      state = 0;
    } else state = 0;
  }
}

// --- TELEMETRY SENDER ---
void sendTelemetry() {
  if (test_mode) return; // Mute binary spam selama testing manual via terminal ASCII
  uint8_t buf[32];
  
  buf[0] = 0xAA; buf[1] = 0x55; buf[2] = 6; buf[3] = 0x20;
  buf[4] = robot_state; 
  float vbat = 11.5f; memcpy(&buf[5], &vbat, 4); 
  buf[9] = 0; 
  
  uint8_t chk = buf[3];
  for(int i=4; i<10; i++) chk ^= buf[i];
  buf[10] = chk;
  Serial.write(buf, 11);

  buf[2] = 16; buf[3] = 0x21;
  
  // Masukkan data ultrasonik yang sesungguhnya ke dalam paket biner
  memcpy(&buf[4], (void*)&us_dist[0], 2); // Front
  memcpy(&buf[6], (void*)&us_dist[2], 2); // Left
  memcpy(&buf[8], (void*)&us_dist[3], 2); // Right
  memcpy(&buf[10], (void*)&us_dist[1], 2); // Back
  
  float imu = 0.0f; 
  memcpy(&buf[12], &imu, 4); memcpy(&buf[16], &imu, 4);
  
  chk = buf[3];
  for(int i=4; i<20; i++) chk ^= buf[i];
  buf[20] = chk;
  Serial.write(buf, 21);
}

// --- GAIT GENERATOR (OMNIDIRECTIONAL TRIPOD) ---
const float R_BODY = 118.5f; // Radius (Diameter 237/2)
const float DEFAULT_Y = 90.0f; // Jarak horizontal ujung kaki (dipersempit agar tidak mengangkang)
const float MOUNT_ANGLES[6] = { 135.0f, 90.0f, 45.0f, -135.0f, -90.0f, -45.0f }; // L_B, L_M, L_F, R_B, R_M, R_F
const float STAND_Z = -50.0f; // Ketinggian berdiri default (5cm)
const float STEP_HEIGHT = 40.0f; // Berapa mm kaki diangkat (Lift 40mm)

float gait_phase = 0.0f;
uint32_t last_gait_time = 0;

void solveLegIK(int i, float dx_body, float dy_body, float dz_body) {
  float rad = MOUNT_ANGLES[i] * PI / 180.0f;
  float mount_X = R_BODY * cos(rad);
  float mount_Y = R_BODY * sin(rad);
  
  // Posisi absolut kaki (Body Coordinate)
  float foot_X = (R_BODY + DEFAULT_Y) * cos(rad) + dx_body;
  float foot_Y = (R_BODY + DEFAULT_Y) * sin(rad) + dy_body;
  
  // Posisi relatif dari coxa joint
  float diff_X = foot_X - mount_X;
  float diff_Y = foot_Y - mount_Y;
  
  // Transformasi ke koordinat lokal kaki (Y adalah arah keluar, X adalah tangensial)
  float local_y = diff_X * cos(rad) + diff_Y * sin(rad); 
  float local_x = -diff_X * sin(rad) + diff_Y * cos(rad);
  
  float angles[3];
  if (calculateIK(local_x, local_y, dz_body, angles)) {
    Leg& L = legs[i];
    
    // Inversi derajat coxa untuk SEMUA kaki agar seragam dengan orientasi max_move.md
    angles[0] = -angles[0]; 

    uint16_t c_raw = degToDxl(angles[0], L.lim.coxa);
    uint16_t f_raw = degToDxl(angles[1], L.lim.femur);
    uint16_t t_raw = degToDxl(angles[2], L.lim.tibia);
    
    // Hentikan servo (kendur) jika ESTOP
    if (robot_state == 3) {
      for (int j=0; j<3; j++) dxlWrite(L.port, L.ids[j], 24, 0, 1);
      return; 
    }

    dxlWrite(L.port, L.ids[0], 30, c_raw, 2);
    dxlWrite(L.port, L.ids[1], 30, f_raw, 2);
    dxlWrite(L.port, L.ids[2], 30, t_raw, 2);
  }
}

void updateGait() {
  uint32_t now = millis();
  float dt = (now - last_gait_time) / 1000.0f;
  last_gait_time = now;
  
  // Recovery: Jika sebelumnya ESTOP (3) lalu disuruh jalan lagi, nyalakan torsi motor
  static uint8_t last_state = 0;
  if (last_state == 3 && robot_state != 3) {
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 3; j++) dxlWrite(legs[i].port, legs[i].ids[j], 24, 1, 1); 
    }
  }
  last_state = robot_state;

  // Asumsi cmd_vel dalam m/s, ubah ke mm/s
  float vx = cmd_vel_x * 1000.0f; 
  float vy = cmd_vel_y * 1000.0f;
  float vyaw = cmd_vel_yaw; 
  
  float speed = sqrt(vx*vx + vy*vy) + abs(vyaw) * R_BODY;
  
  if (speed < 5.0f || robot_state != 1) { 
    // Berhenti / IDLE: Kembalikan kaki ke pijakan semula
    for (int i=0; i<6; i++) solveLegIK(i, 0, 0, STAND_Z);
    gait_phase = 0;
    return;
  }
  
  // Majukan fase (Dikembalikan ke 150.0f agar frekuensi ayunan/temponya kembali santai seperti semula)
  gait_phase += (speed / 150.0f) * dt; 
  if (gait_phase > 1.0f) gait_phase -= 1.0f;
  
  for (int i=0; i<6; i++) {
    // Tripod A: Kiri Depan (2), Kanan Tengah (4), Kiri Belakang (0)
    bool is_tripod_A = (i == 0 || i == 2 || i == 4);
    
    float local_phase = is_tripod_A ? gait_phase : (gait_phase + 0.5f);
    if (local_phase > 1.0f) local_phase -= 1.0f;
    
    float factor = 0.0f;
    float z = STAND_Z;
    
    if (local_phase < 0.5f) { // Fase SWING (Angkat dan maju)
      float p = local_phase * 2.0f; // 0 to 1
      factor = p - 0.5f; // Ayun kaki maju (+ offset)
      z = STAND_Z + sin(p * PI) * STEP_HEIGHT;
    } else { // Fase STANCE (Injak dan dorong)
      float p = (local_phase - 0.5f) * 2.0f; // 0 to 1
      factor = 0.5f - p; // Dorong badan (kaki digeser mundur, - offset)
      z = STAND_Z;
    }
    
    // Matriks Transformasi Kinematik (Gerak Translasi + Rotasi Yaw)
    float rad = MOUNT_ANGLES[i] * PI / 180.0f;
    float stride_x = vx + vyaw * (-R_BODY * sin(rad));
    float stride_y = vy + vyaw * ( R_BODY * cos(rad));
    
    // Rasio stride
    float dx = stride_x * factor * 1.0f; 
    float dy = stride_y * factor * 1.0f;
    
    solveLegIK(i, dx, dy, z);
  }
}

// --- SETUP & LOOP ---
void setup() {
  Serial.begin(115200); 
  
  dxlR.setHalfDuplex(); dxlR.begin(1000000);
  dxlL.setHalfDuplex(); dxlL.begin(1000000);
  
  grip1.attach(PB8);
  grip2.attach(PB9);
  grip1.write(90); 
  grip2.write(90);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(ECHO_BACK, INPUT);
  pinMode(ECHO_LEFT, INPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  
  // Attach interrupt untuk Ultrasonik (Non-Blocking)
  attachInterrupt(digitalPinToInterrupt(ECHO_FRONT), exti_front, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_BACK), exti_back, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_LEFT), exti_left, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_RIGHT), exti_right, CHANGE);
  
  // --- SAFETY DELAY & SEQUENTIAL WAKEUP ---
  // Tunggu 3 detik agar pengguna punya waktu menarik tangan setelah mencolok baterai
  delay(3000);
  
  // Nyalakan torsi lengan capit (ID 0 dan 33) di Bus Kanan
  dxlWrite(&dxlR, 0, 32, 100, 2); dxlWrite(&dxlR, 0, 24, 1, 1);
  dxlWrite(&dxlR, 33, 32, 100, 2); dxlWrite(&dxlR, 33, 24, 1, 1);
  
  // Nyalakan torsi dan posisikan kaki SATU PER SATU (tiap 500ms)
  // Mencegah robot menghentak sekaligus yang bisa menyebabkan baterai drop / restart (Brownout)
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 3; j++) {
      dxlWrite(legs[i].port, legs[i].ids[j], 32, 100, 2); // Set Moving Speed (32) jadi 100 (Slow) agar tidak menghentak
      dxlWrite(legs[i].port, legs[i].ids[j], 24, 1, 1);   // Baru nyalakan Torque (24)
    }
    // Langsung paksa kaki ini ke posisi berdiri
    solveLegIK(i, 0, 0, STAND_Z);
    delay(500); 
  }
  
  last_gait_time = millis();
}

void loop() {
  parseSerialCDC();

  // 1. Tembak PING Ultrasonik tiap 50ms (Non-blocking)
  static uint32_t last_ping = 0;
  if (millis() - last_ping >= 50) {
    last_ping = millis();
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
  }

  // 2. Kirim Telemetri (Biner) ke RPi5 tiap 50ms
  static uint32_t last_telemetry = 0;
  if (millis() - last_telemetry >= 50) {
    last_telemetry = millis();
    sendTelemetry();
  }
  
  // 3. Print Data Debug ke Serial Terminal (Hanya jika manual test_mode) tiap 500ms
  static uint32_t last_debug = 0;
  if (test_mode && millis() - last_debug >= 500) {
    last_debug = millis();
    Serial.print("[DEBUG SENSOR] Jarak (mm) -> Depan: "); Serial.print(us_dist[0]);
    Serial.print(" | Belakang: "); Serial.print(us_dist[1]);
    Serial.print(" | Kiri: "); Serial.print(us_dist[2]);
    Serial.print(" | Kanan: "); Serial.println(us_dist[3]);
  }
  
  // 4. Update Gait Kinematik tiap 20ms (50Hz)
  static uint32_t last_gait = 0;
  if (millis() - last_gait >= 20) {
    last_gait = millis();
    updateGait();
  }
}
