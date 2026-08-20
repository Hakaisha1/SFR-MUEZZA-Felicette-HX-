# 📖 Guide: Movement

Panduan teknis pengembangan untuk folder `/movement`. Baca `agent.md` terlebih dahulu sebelum dokumen ini.

---

## Setup Environment

- **IDE**: STM32CubeMX (generasi kode HAL) + PlatformIO (build & upload)
- **Framework**: STM32 HAL + FreeRTOS (untuk multitasking concurrent)
- **Papan**: STM32F405RGT6 — WeAct Black Pill

Project PlatformIO utama berada di folder `/Dynamixel`. Folder `/movement` berisi dokumentasi, algoritma referensi, dan pseudocode yang diturunkan ke kode C aktual di sana.

### Struktur Folder
```
/movement
├── agent.md                  # Definisi peran
├── guide.md                  # Dokumen ini
└── referensi/
    ├── ik_hexapod.md         # Derivasi Inverse Kinematics kaki
    ├── gait_tripod.md        # Penjelasan tripod gait pattern
    └── protokol_uart_stm32.md# Implementasi UART parser di sisi STM32
```

---

## Geometri Kaki Hexapod (3 DOF)

Setiap kaki memiliki 3 sendi yang dikendalikan oleh 3 servo Dynamixel:

```
  [COXA]──(θ1)──[FEMUR]──(θ2)──[TIBIA]──(θ3)──[Ujung Kaki]
    │              │               │
  Rotasi YAW    Rotasi Pitch    Rotasi Pitch
  (sumbu Z)     (sumbu Y)       (sumbu Y)
```

| Parameter | Simbol | Keterangan |
|---|---|---|
| Panjang Coxa | `L1` | Panjang segmen pertama (mm) |
| Panjang Femur | `L2` | Panjang segmen kedua (mm) |
| Panjang Tibia | `L3` | Panjang segmen ketiga (mm) |
| Sudut Coxa | `θ1` | Rotasi horizontal kaki |
| Sudut Femur | `θ2` | Rotasi naik/turun femur |
| Sudut Tibia | `θ3` | Rotasi naik/turun tibia |

---

## Inverse Kinematics (Satu Kaki)

Diberikan target posisi ujung kaki `(x, y, z)` relatif terhadap sendi coxa, hitung `θ1`, `θ2`, `θ3`:

### Langkah 1 — Hitung θ1 (Coxa, rotasi horizontal)
```c
theta1 = atan2f(y, x);  // dalam radian
```

### Langkah 2 — Proyeksikan ke bidang vertikal
```c
float R = sqrtf(x*x + y*y) - L1;  // Jarak horizontal setelah coxa
float D = sqrtf(R*R + z*z);        // Jarak lurus femur ke ujung kaki
```

### Langkah 3 — Hitung θ3 (Tibia) via Cosine Rule
```c
float cos_theta3 = (D*D - L2*L2 - L3*L3) / (2.0f * L2 * L3);
cos_theta3 = fmaxf(-1.0f, fminf(1.0f, cos_theta3)); // Clamp [-1, 1]
theta3 = acosf(cos_theta3);
```

### Langkah 4 — Hitung θ2 (Femur)
```c
float alpha = atan2f(-z, R);
float beta  = atan2f(L3 * sinf(theta3), L2 + L3 * cosf(theta3));
theta2 = alpha - beta;
```

> ⚠️ Perhatikan tanda positif/negatif `z` sesuai konvensi koordinat mekanik robot kalian.

---

## Tripod Gait

Pada **tripod gait**, 6 kaki dibagi menjadi 2 kelompok yang bergantian:
- **Kelompok A** (Kiri Depan, Kanan Tengah, Kiri Belakang): Langkah sambil Kelompok B menopang.
- **Kelompok B** (Kanan Depan, Kiri Tengah, Kanan Belakang): Langkah sambil Kelompok A menopang.

```c
typedef enum { STANCE, SWING } LegPhase;

// Cycle gait berulang dengan periode T (ms)
void update_gait(uint32_t t_ms, float vx, float vy, float vyaw) {
    float phase = fmodf((float)t_ms, GAIT_PERIOD_MS) / GAIT_PERIOD_MS;
    for (int i = 0; i < 6; i++) {
        // Kaki ganjil (0,2,4) dan genap (1,3,5) punya offset 0.5
        float leg_phase = fmodf(phase + (i % 2) * 0.5f, 1.0f);
        if (leg_phase < 0.5f) {
            set_leg_stance(i, vx, vy, vyaw, leg_phase); // Menopang & mendorong
        } else {
            set_leg_swing(i, vx, vy, vyaw, leg_phase);  // Mengangkat & maju
        }
    }
}
```

---

## Driver Dynamixel (TTL Half-Duplex)

STM32F405 berkomunikasi dengan servo Dynamixel melalui satu jalur UART dalam mode **half-duplex** (TX dan RX bergantian pada pin yang sama).

### Konfigurasi CubeMX
- Enable `USART` yang diinginkan
- Set mode **Single Wire (Half-Duplex)**
- Gunakan satu GPIO sebagai **Direction Control** (DE/RE pin pada driver RS485/level shifter)

```c
// Sebelum kirim perintah: aktifkan TX
HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);
HAL_UART_Transmit(&huart1, paket, panjang_paket, 10);

// Setelah selesai kirim: aktifkan RX untuk baca respons
HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);
HAL_UART_Receive(&huart1, buffer_rx, panjang_respons, 10);
```

---

## Failsafe di STM32

```c
// Di FreeRTOS Task atau interrupt timer
static uint32_t terakhir_terima_command = 0;

void on_uart_command_diterima() {
    terakhir_terima_command = HAL_GetTick();
}

void cek_watchdog_uart() {
    if (HAL_GetTick() - terakhir_terima_command > 500) {
        emergency_stop(); // Bekukan semua servo, set posisi aman
    }
}

void cek_obstacle_reflex() {
    uint16_t tof_depan = baca_tof(TOF_DEPAN);
    if (tof_depan < 80) { // < 8 cm
        hentikan_gerak_maju();
    }
}
```

---

## Checklist Pengembangan

- [ ] 1 servo Dynamixel bisa digerakkan & membaca posisi balikan
- [ ] 18 servo kaki bisa digerakkan serentak via broadcast/sync write
- [ ] Inverse Kinematics 1 kaki tervalidasi secara matematis (plot atau simulasi)
- [ ] Tripod gait berjalan lurus di lantai datar
- [ ] Gait belok kiri/kanan berfungsi
- [ ] IMU terbaca, roll & pitch akurat ≤ ±2°
- [ ] Koreksi postur aktif saat kaki berada di medan miring
- [ ] Gripper Dynamixel + MG90S berfungsi (buka/tutup terkontrol)
- [ ] 4 sensor ToF VL53L1X terbaca via TCA9548A
- [ ] Obstacle reflex aktif di < 8 cm (tanpa menunggu RPi5)
- [ ] Watchdog UART 500ms aktif dan terbukti masuk EMERGENCY_STOP
- [ ] Telemetri TLM_ROBOT_STATUS & TLM_SENSOR_DATA terkirim ke RPi5 stabil
