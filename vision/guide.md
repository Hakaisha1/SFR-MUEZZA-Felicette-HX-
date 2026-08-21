# 📖 Guide: Vision

Panduan teknis pengembangan untuk folder `/vision`. Baca `agent.md` terlebih dahulu sebelum dokumen ini.

> 📋 **Kontrak Data**: Format output JSON Vision→Integration didefinisikan di [`data_contract.json`](../data_contract.json). Pastikan field, tipe data, dan nilai sentinel sesuai kontrak tersebut.
> 📐 **State Machine**: Lihat [`state_machine.md`](../state_machine.md) untuk memahami bagaimana output Vision mempengaruhi transisi state robot.
> 🔀 **Git Workflow**: Lihat [`flow.md`](../flow.md) untuk aturan branching dan PR.

---

## Setup Environment

### Dependensi Python (RPi5)
```bash
pip install opencv-python ultralytics numpy
```

> Jika menggunakan model YOLO ringan, gunakan `ultralytics`. Jika hanya OpenCV klasik (HSV/contour), tidak perlu `ultralytics`.

### Struktur Folder
```
/vision
├── agent.md              # Definisi peran
├── guide.md              # Dokumen ini
├── main.py               # Entry point, loop kamera utama
├── detector.py           # Logika deteksi objek dalam frame
├── classifier.py         # Logika klasifikasi dummy vs riil
├── estimator.py          # Estimasi jarak (piksel → cm)
├── publisher.py          # Publikasi JSON output ke Integration (socket/pipe)
├── data/
│   ├── dummy/            # Gambar sampel target dummy
│   └── riil/             # Gambar sampel korban riil
└── tests/
    ├── test_detector.py
    └── test_classifier.py
```

---

## Cara Membaca Kamera & Memproses Frame

```python
import cv2
import time
import json

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
cap.set(cv2.CAP_PROP_FPS, 30)

def proses_frame(frame):
    """Mendeteksi dan mengklasifikasi target. Return dict output."""
    # TODO: Implementasi detector & classifier
    return {
        "label": "tidak_ada",
        "confidence": 0.0,
        "posisi_x_px": 0,
        "posisi_y_px": 0,
        "jarak_estimasi_cm": 0.0,
        "timestamp_ms": int(time.time() * 1000)
    }

while True:
    ret, frame = cap.read()
    if not ret:
        break
    hasil = proses_frame(frame)
    print(json.dumps(hasil))  # Ganti dengan publisher ke Integration
```

---

## Deteksi dengan OpenCV (HSV Color Masking)

Pendekatan cepat untuk deteksi berbasis warna — cocok jika dummy berwarna solid dan kondisi pencahayaan terkontrol.

```python
import cv2
import numpy as np

def deteksi_berbasis_warna(frame, hsv_bawah, hsv_atas):
    """
    Deteksi objek berdasarkan rentang warna HSV.
    Contoh: deteksi objek oranye (dummy)
      hsv_bawah = np.array([10, 100, 100])
      hsv_atas  = np.array([25, 255, 255])
    """
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, hsv_bawah, hsv_atas)
    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)
    kontur, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if not kontur:
        return None
    terbesar = max(kontur, key=cv2.contourArea)
    if cv2.contourArea(terbesar) < 500:  # Filter noise kecil
        return None
    M = cv2.moments(terbesar)
    cx = int(M['m10'] / M['m00'])
    cy = int(M['m01'] / M['m00'])
    return cx, cy
```

---

## Estimasi Jarak (Metode Lebar Objek)

Jika ukuran fisik target diketahui, jarak bisa diperkirakan dari lebar bounding box.

```python
LEBAR_OBJEK_NYATA_CM = 20.0   # Estimasi lebar fisik target (cm)
FOCAL_LENGTH_PX = 600.0       # Kalibrasi dari pengukuran nyata

def estimasi_jarak(lebar_bounding_box_px: float) -> float:
    """Menghitung jarak dalam cm menggunakan rumus focal length."""
    if lebar_bounding_box_px == 0:
        return 0.0
    return (LEBAR_OBJEK_NYATA_CM * FOCAL_LENGTH_PX) / lebar_bounding_box_px
```

> **Kalibrasi Focal Length**: Letakkan target pada jarak diketahui (mis. 50 cm), ukur `lebar_bounding_box_px` yang terdeteksi, lalu hitung:
> `FOCAL_LENGTH_PX = (lebar_bbox_px * 50) / LEBAR_OBJEK_NYATA_CM`

---

## Pengujian Offline (Tanpa Robot)

Sebelum mencoba di hardware, validasikan pipeline dengan gambar/video dulu:

```bash
# Jalankan detector pada gambar statis
python detector.py --input data/dummy/sample_01.jpg

# Jalankan classifier pada folder gambar
python classifier.py --folder data/riil/
```

---

## Checklist Pengembangan (Sesuai Milestone)

**M0 — Setup:**
- [ ] Kamera terbaca stabil di RPi5 pada resolusi 640×480 @ 30 FPS

**M1 — Fondasi:**
- [ ] Deteksi target berjalan dengan hasil kotak pembatas (bounding box) akurat
- [ ] Klasifikasi dummy vs riil mencapai akurasi ≥ 85% pada data uji offline
- [ ] Estimasi jarak error ≤ ±5 cm pada rentang 15–80 cm
- [ ] Output JSON terkirim ke Integration secara stabil tanpa putus

**M2 — Tuning:**
- [ ] Diuji dalam variasi pencahayaan: terang, redup, backlit
- [ ] Performa: inferensi ≤ 100ms per frame (10 FPS minimum)

**M3+ — Integrasi:**
- [ ] Output Vision live digunakan oleh Integration untuk transisi state (SEARCHING → APPROACHING → CLASSIFYING)
- [ ] Confidence threshold 0.75 terbukti efektif mengurangi false positive
