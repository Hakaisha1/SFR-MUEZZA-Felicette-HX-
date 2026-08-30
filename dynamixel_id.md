# Konfigurasi ID Servo Dynamixel

Dokumen ini berisi pemetaan ID Dynamixel untuk masing-masing kaki robot (Hexapod / Quadruped) agar mempermudah pemrograman Inverse Kinematics.

### Tabel Pemetaan Kaki

| Posisi Bagian | ID Coxa (Pinggul) | ID Femur (Paha) | ID Tibia (Betis) | Status & Catatan |
| :--- | :---: | :---: | :---: | :--- |
| **Kaki Kiri A** | `4` | `10` | `16` | ✅ **Normal.** Sukses dites IK. *(Catatan: Engsel Tibia sempat mentok mekanis)* |
| **Kaki Kiri B (Pertama)** | `6` | `12` | `18` | ⚠️ **Kendala Kabel.** ID 18 normal jika dicolok langsung (bebas dari bentrok). |
| **Kaki Kiri C** | `3` | `9` | `15` | ✅ **Normal.** Ketiga ID berhasil terdeteksi dari hasil scan (bebas dari bentrok). |
| **Kaki Kanan A** | `1` | `7` | `13` | ✅ **Normal.** Ketiga ID berhasil terdeteksi dari hasil scan. |
| **Kaki Kanan B** | `2` *(Direncanakan)* | `8` | `14` | ⚠️ **Coxa sedang dibersihkan.** Femur & Tibia sukses diubah ke 8 & 14 (Bebas bentrok!). |
| **Kaki Kanan C** | `17` | `11` | `5` | ✅ **Normal.** *(Catatan: Urutan memutar; Coxa 17, Tibia 5 karena injeksi kabel dicolok dari telapak/Tibia).* |

### Tabel Pemetaan Komponen Lain

| Posisi Bagian | ID Servo | Status & Catatan |
| :--- | :---: | :--- |
| **Gripper (Capit)** | `0` | ✅ **Normal.** ID 0 berhasil terdeteksi sendirian dari hasil scan. |

---
*Catatan Tambahan:*
*   *Coxa*: Mengontrol ayunan kaki maju-mundur (X-axis).
*   *Femur & Tibia*: Mengontrol langkah naik-turun (Z-axis).