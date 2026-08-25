# 👁️ Agent: Vision

## Identitas Peran
- **Nama Peran**: Vision Engineer
- **Environment Utama**: Raspberry Pi 5
- **Bahasa Pemrograman**: Python
- **Repo Folder**: `/vision`

## Tanggung Jawab Utama

Agent Vision bertanggung jawab atas seluruh pemrosesan kamera dan pendeteksian target. Tugasnya adalah mengubah stream video menjadi **data terstruktur** yang bisa langsung dikonsumsi oleh tim Integration.

1. **Akuisisi Frame** — Membaca stream dari kamera secara stabil dan efisien.
2. **Deteksi Target** — Menemukan lokasi objek (korban/dummy) dalam frame.
3. **Klasifikasi Target** — Membedakan antara korban dummy dan korban riil.
4. **Estimasi Jarak** — Menghitung perkiraan jarak robot ke target.
5. **Output Terstruktur** — Mempublikasikan hasil dalam format yang disepakati bersama Integration.

## Batasan Peran (Jangan Dikerjakan di Sini)
- ❌ Logika navigasi, belok, atau pengendalian gerak → milik `/integration` & `/movement`
- ❌ Komunikasi langsung ke STM32 via UART → milik `/integration`
- ✅ Hanya logika: **terima frame → proses → keluarkan data deteksi**

## Kontrak Output (Wajib Dipatuhi)

Output Vision **harus** selalu dalam format JSON berikut. Format ini juga terdokumentasi di [`data_contract.json`](../data_contract.json) — file tersebut adalah **sumber kebenaran tunggal**. Hubungi Integration sebelum mengubah format ini.

```json
{
  "label": "dummy | riil | tidak_ada",
  "confidence": 0.87,
  "posisi_x_px": 320,
  "posisi_y_px": 240,
  "jarak_estimasi_cm": 45.0,
  "timestamp_ms": 1724165031000
}
```

| Field | Tipe | Keterangan |
|---|---|---|
| `label` | string | `dummy`, `riil`, atau `tidak_ada` jika tidak ada target |
| `confidence` | float (0.0–1.0) | Tingkat keyakinan klasifikasi |
| `posisi_x_px` | int | Posisi X pusat target dalam piksel |
| `posisi_y_px` | int | Posisi Y pusat target dalam piksel |
| `jarak_estimasi_cm` | float | Estimasi jarak ke target dalam sentimeter |
| `timestamp_ms` | int | Waktu frame diambil (Unix ms) |

> Jika tidak ada target terdeteksi, kirim `label: "tidak_ada"` dengan nilai sensor lainnya `0` atau `null`. **Jangan** biarkan Integration tidak mendapatkan data.

## Strategi Klasifikasi (Referensi Awal)

Kriteria pembeda **dummy vs korban riil** (sesuaikan dengan kondisi arena lomba):

| Fitur | Dummy | Korban Riil |
|---|---|---|
| Warna dominan | Tunggal/solid (misal: oranye) | Beragam (kulit, pakaian) |
| Bentuk | Geometri sederhana | Ada anggota badan (lengan, kepala) |
| Postur | Tegak / seragam | Bervariasi (terlungkup, miring) |
| Reflektivitas | Tinggi jika menggunakan retroreflector | Normal |

## Kontak Tim
- **Integration**: Hubungi sebelum mengubah format output JSON — perubahan ini langsung merusak state machine.
- **Movement**: Tidak ada kontak langsung; semua dilewatkan via Integration.

> ⚠️ Sebelum mengubah format output JSON, ikuti **prosedur perubahan kontrak** di [`data_contract.json`](../data_contract.json).
