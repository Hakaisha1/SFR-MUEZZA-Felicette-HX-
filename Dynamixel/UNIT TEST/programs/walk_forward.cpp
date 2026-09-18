#include <Arduino.h>

HardwareSerial dxl1(PA9);   // Bus Kanan
HardwareSerial dxl2(PA2);   // Bus Kiri

const uint32_t DXL_BAUD = 1000000;
const uint32_t TIMEOUT_MS = 50;
const uint16_t WALK_SPEED = 80; // ponytail: naikkan jika terlalu lambat

#define ADDR_TORQUE_ENABLE    24
#define ADDR_MOVING_SPEED     32
#define ADDR_GOAL_POSITION    30
#define ADDR_PRESENT_POSITION 36

// =================== BATAS GERAK (dari max_move.md) ===================
// ponytail: isi dari max_move.md, 512 = placeholder belum dikalibrasi

struct LegLimits {
  // Coxa: Front / Center / Back
  uint16_t cF, cC, cB;
  // Femur: Up / Center / Down
  uint16_t fU, fC, fD;
  // Tibia: In / Center / Out
  uint16_t tI, tC, tO;
};

struct Leg {
  const char* name;
  uint8_t ids[3];       // coxa, femur, tibia
  LegLimits lim;
  HardwareSerial* port;
};

// Urutan: A=belakang, B=tengah, C=depan
Leg legs[] = {
  //                        IDs           cF   cC   cB    fU   fC   fD    tI   tC   tO
  {"Kiri A (Blk)",  { 1,  2,  3}, {1000, 512, 550,  400, 512, 700,  450, 550, 700}, &dxl2},
  {"Kiri B (Tgh)",  { 4,  5,  6}, { 600, 512, 400,  300, 512, 700,  400, 512, 700}, &dxl2},
  {"Kiri C (Dpn)",  { 7,  8,  9}, { 700, 600, 400,  350, 512, 700,  400, 512,1000}, &dxl2},
  {"Kanan A (Blk)", {10, 11, 12}, { 400, 512, 700,  300, 550, 700,  400, 512,1000}, &dxl1},
  {"Kanan B (Tgh)", {20, 21, 22}, { 400, 512, 700,  300, 512, 700,  400, 512,1000}, &dxl1},
  {"Kanan C (Dpn)", {30, 31, 32}, { 300, 512, 700,  300, 512, 700,  400, 512,1000}, &dxl1},
};
const uint8_t NUM_LEGS = 6;

// Urutan langkah: depan ke belakang (C>B>A), selang-seling kanan-kiri
const uint8_t WALK_ORDER[] = {5, 2, 4, 1, 3, 0};
// KananC(dpn), KiriC(dpn), KananB(tgh), KiriB(tgh), KananA(blk), KiriA(blk)

// Tripod gait: 2 grup diagonal bergerak bergantian
const uint8_t TRIPOD_A[] = {0, 4, 2}; // KiriA + KananB + KiriC
const uint8_t TRIPOD_B[] = {3, 1, 5}; // KananA + KiriB + KananC

// =================== PROTOKOL ===================

uint8_t calcChecksum(uint8_t id, uint8_t length, uint8_t instruction, uint8_t* params, uint8_t paramLen) {
  uint16_t sum = id + length + instruction;
  for (uint8_t i = 0; i < paramLen; i++) sum += params[i];
  return (uint8_t)(~sum);
}

void dxlWrite(HardwareSerial &port, uint8_t id, uint8_t addr, uint16_t value, uint8_t numBytes) {
  uint8_t params[3];
  params[0] = addr;
  params[1] = value & 0xFF;
  if (numBytes == 2) params[2] = (value >> 8) & 0xFF;
  uint8_t paramLen = (numBytes == 2) ? 3 : 2;
  uint8_t length = paramLen + 2;
  uint8_t checksum = calcChecksum(id, length, 0x03, params, paramLen);

  while (port.available()) port.read();
  port.write(0xFF); port.write(0xFF);
  port.write(id); port.write(length); port.write((uint8_t)0x03);
  port.write(params, paramLen);
  port.write(checksum);
  port.flush();
  port.enableHalfDuplexRx();

  uint32_t start = millis();
  while ((millis() - start) < 15) { if (port.available()) port.read(); }
}

bool dxlRead(HardwareSerial &port, uint8_t id, uint8_t addr, uint8_t readLen, uint16_t &outValue) {
  if (readLen > 2) return false;
  uint8_t params[2] = {addr, readLen};
  uint8_t length = 4;
  uint8_t checksum = calcChecksum(id, length, 0x02, params, 2);

  while (port.available()) port.read();
  port.write(0xFF); port.write(0xFF);
  port.write(id); port.write(length); port.write((uint8_t)0x02);
  port.write(params, 2);
  port.write(checksum);
  port.flush();
  port.enableHalfDuplexRx();

  uint8_t resp[8];
  size_t idx = 0;
  uint32_t start = millis();
  size_t expectedLen = 6 + readLen;
  while (idx < expectedLen && (millis() - start) < TIMEOUT_MS) {
    if (port.available()) resp[idx++] = port.read();
  }
  if (idx < expectedLen || resp[0] != 0xFF || resp[1] != 0xFF) return false;
  outValue = (readLen == 2) ? (resp[5] | (resp[6] << 8)) : resp[5];
  return true;
}

bool dxlPing(HardwareSerial &port, uint8_t id) {
  uint8_t packet[] = {0xFF, 0xFF, id, 0x02, 0x01, (uint8_t)~(id + 0x02 + 0x01)};
  while (port.available()) port.read();
  port.write(packet, 6);
  port.flush();
  port.enableHalfDuplexRx();

  uint8_t resp[6];
  size_t idx = 0;
  uint32_t start = millis();
  while (idx < 6 && (millis() - start) < TIMEOUT_MS) {
    if (port.available()) resp[idx++] = port.read();
  }
  return (idx == 6 && resp[0] == 0xFF && resp[1] == 0xFF && resp[2] == id);
}

// =================== HELPER ===================

String bacaBarisSerial() {
  while (!Serial.available()) delay(50);
  String s = Serial.readStringUntil('\n');
  s.trim();
  while (Serial.available()) Serial.read();
  return s;
}

void setServo(HardwareSerial &port, uint8_t id, uint16_t pos) {
  dxlWrite(port, id, ADDR_MOVING_SPEED, WALK_SPEED, 2);
  dxlWrite(port, id, ADDR_TORQUE_ENABLE, 1, 1);
  delay(20);
  dxlWrite(port, id, ADDR_GOAL_POSITION, pos, 2);
}

// ponytail: fraksi dari center ke batas max, 40% = gerakan aman, naikkan jika langkah kurang jauh
const float WALK_RATIO = 0.4f;

// Hitung posisi antara center dan limit, sesuai WALK_RATIO
uint16_t toward(uint16_t center, uint16_t limit) {
  return center + (int16_t)((float)((int16_t)limit - (int16_t)center) * WALK_RATIO);
}

// =================== GERAKAN KAKI ===================

// Satu siklus langkah maju untuk satu kaki:
//   1. ANGKAT  — femur naik, tibia keluar (kaki terangkat dari tanah)
//   2. AYUN    — coxa ke depan (kaki melayang ke depan)
//   3. TURUN   — femur center, tibia center (kaki mendarat)
void stepForward(uint8_t legIdx) {
  Leg& L = legs[legIdx];
  HardwareSerial& port = *L.port;
  Serial.print("\n>> LANGKAH: "); Serial.println(L.name);

  // 1. Angkat
  Serial.println("   [1] Angkat...");
  setServo(port, L.ids[1], toward(L.lim.fC, L.lim.fU));  // femur up
  setServo(port, L.ids[2], toward(L.lim.tC, L.lim.tO));  // tibia out
  delay(600);

  // 2. Ayun ke depan
  Serial.println("   [2] Ayun ke depan...");
  setServo(port, L.ids[0], toward(L.lim.cC, L.lim.cF));  // coxa front
  delay(600);

  // 3. Turun / mendarat
  Serial.println("   [3] Turun...");
  setServo(port, L.ids[1], L.lim.fC);   // femur center
  setServo(port, L.ids[2], L.lim.tC);   // tibia center
  delay(600);

  Serial.println("   Selesai.");
}

// Dorong badan: semua kaki di tanah, coxa geser ke belakang bersamaan
void pushBody() {
  Serial.println("\n>> DORONG BADAN (coxa -> Back)...");
  for (uint8_t i = 0; i < NUM_LEGS; i++) {
    setServo(*legs[i].port, legs[i].ids[0], toward(legs[i].lim.cC, legs[i].lim.cB));
  }
  delay(1000);
  Serial.println("   Badan maju.");
}

// Posisi berdiri: semua servo ke center
void standAll() {
  Serial.println("\n>> Semua kaki ke posisi BERDIRI (center)...");
  for (uint8_t i = 0; i < NUM_LEGS; i++) {
    Leg& L = legs[i];
    setServo(*L.port, L.ids[0], L.lim.cC);
    setServo(*L.port, L.ids[1], L.lim.fC);
    setServo(*L.port, L.ids[2], L.lim.tC);
  }
  delay(2000);
  Serial.println("   Berdiri.\n");
}

void matikanSemua() {
  Serial.println("\n>> Matikan torque semua...");
  for (uint8_t i = 0; i < NUM_LEGS; i++)
    for (uint8_t j = 0; j < 3; j++)
      dxlWrite(*legs[i].port, legs[i].ids[j], ADDR_TORQUE_ENABLE, 0, 1);
  Serial.println("   Semua lemas.\n");
}

// =================== TRIPOD GAIT ===================

// Angkat + ayun 3 kaki sekaligus
void tripodLift(const uint8_t* group) {
  for (uint8_t i = 0; i < 3; i++) {
    Leg& L = legs[group[i]];
    setServo(*L.port, L.ids[1], toward(L.lim.fC, L.lim.fU));  // femur up
    setServo(*L.port, L.ids[2], toward(L.lim.tC, L.lim.tO));  // tibia out
  }
  delay(600);
  for (uint8_t i = 0; i < 3; i++) {
    Leg& L = legs[group[i]];
    setServo(*L.port, L.ids[0], toward(L.lim.cC, L.lim.cF));  // coxa front
  }
  delay(600);
}

// Turunkan 3 kaki sekaligus
void tripodPlant(const uint8_t* group) {
  for (uint8_t i = 0; i < 3; i++) {
    Leg& L = legs[group[i]];
    setServo(*L.port, L.ids[1], L.lim.fC);  // femur center
    setServo(*L.port, L.ids[2], L.lim.tC);  // tibia center
  }
  delay(600);
}

// Dorong 3 kaki ke belakang (grup yang di tanah mendorong badan)
void tripodPush(const uint8_t* group) {
  for (uint8_t i = 0; i < 3; i++) {
    Leg& L = legs[group[i]];
    setServo(*L.port, L.ids[0], toward(L.lim.cC, L.lim.cB));  // coxa back
  }
  delay(600);
}

void tripodOneCycle() {
  Serial.println("\n=== TRIPOD GAIT (1 SIKLUS) ===");

  // Fase 1: Grup A angkat+ayun, Grup B dorong
  Serial.println("\n[1/4] Grup A (KiriA+KananB+KiriC) ANGKAT — tekan ENTER...");
  bacaBarisSerial();
  Serial.println("   Angkat + ayun grup A, dorong grup B...");
  tripodLift(TRIPOD_A);
  tripodPush(TRIPOD_B);

  // Fase 2: Grup A turun
  Serial.println("[2/4] Grup A TURUN — tekan ENTER...");
  bacaBarisSerial();
  tripodPlant(TRIPOD_A);

  // Fase 3: Grup B angkat+ayun, Grup A dorong
  Serial.println("[3/4] Grup B (KananA+KiriB+KananC) ANGKAT — tekan ENTER...");
  bacaBarisSerial();
  Serial.println("   Angkat + ayun grup B, dorong grup A...");
  tripodLift(TRIPOD_B);
  tripodPush(TRIPOD_A);

  // Fase 4: Grup B turun
  Serial.println("[4/4] Grup B TURUN — tekan ENTER...");
  bacaBarisSerial();
  tripodPlant(TRIPOD_B);

  Serial.println("\n=== SIKLUS TRIPOD SELESAI ===\n");
}

// =================== MENU ===================

void tampilkanMenu() {
  Serial.println("\n======= WALK FORWARD TEST =======");
  Serial.println("  w -> Wave walk (C>B>A, ENTER tiap kaki)");
  Serial.println("  t -> Tripod gait (2 grup, ENTER tiap fase)");
  Serial.println("  b -> Berdiri (semua center)");
  Serial.println("  x -> Matikan semua torque");
  Serial.println("  h -> Menu");
  Serial.println("=================================\n");
}

void walkOneCycle() {
  Serial.println("\n=== MULAI 1 SIKLUS JALAN ===");
  Serial.println("Tekan ENTER untuk mulai tiap langkah.\n");

  for (uint8_t i = 0; i < 6; i++) {
    uint8_t idx = WALK_ORDER[i];
    Serial.print("["); Serial.print(i + 1); Serial.print("/6] Giliran: ");
    Serial.print(legs[idx].name); Serial.println(" — tekan ENTER...");
    bacaBarisSerial();
    stepForward(idx);
  }

  Serial.println("\nSemua kaki sudah melangkah. Tekan ENTER untuk PUSH BADAN...");
  bacaBarisSerial();
  pushBody();

  Serial.println("\n=== SIKLUS SELESAI ===\n");
}

void prosesPerintah(char cmd) {
  switch (cmd) {
    case 'w': case 'W': walkOneCycle(); break;
    case 't': case 'T': tripodOneCycle(); break;
    case 'b': case 'B': standAll(); break;
    case 'x': case 'X': matikanSemua(); break;
    case 'h': case 'H': tampilkanMenu(); break;
    default: Serial.println("Ketik 'h' untuk menu.");
  }
}

// =================== SETUP & LOOP ===================

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  dxl1.setHalfDuplex(); dxl1.begin(DXL_BAUD);
  dxl2.setHalfDuplex(); dxl2.begin(DXL_BAUD);

  delay(3000);
  Serial.println("=== WALK FORWARD TEST ===");
  tampilkanMenu();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    while (Serial.available()) Serial.read();
    if (input.length() > 0) prosesPerintah(input[0]);
  }
}
