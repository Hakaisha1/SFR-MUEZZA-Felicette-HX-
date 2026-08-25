"""
config.py — Konfigurasi terpusat untuk pipeline Vision.

Semua parameter yang perlu di-tuning ada di sini.
Tidak perlu edit file lain untuk mengubah threshold, warna, atau setting kamera.

Nilai default mengacu pada:
  - data_contract.json (threshold confidence, jarak, dsb.)
  - guide.md (resolusi, FPS, metode deteksi)
"""

import numpy as np


# ============================================================
# KAMERA
# ============================================================

CAMERA_INDEX = 0               # Index kamera (0 = default USB/CSI)
CAMERA_WIDTH = 640             # Lebar frame (piksel)
CAMERA_HEIGHT = 480            # Tinggi frame (piksel)
CAMERA_FPS = 30                # Target FPS kamera


# ============================================================
# DETEKSI & KLASIFIKASI WARNA
# - Korban Asli: Warna ORANYE (Saturasi tinggi S >= 70, H = 0-25 / 170-180)
# - Dummy: Warna ABU-ABU (Saturasi rendah S <= 55, V = 40-200)
# ============================================================

# Mode deteksi: "hsv" (saat ini) atau "yolo" (nanti kalau RPi5 kuat)
DETECTION_MODE = "hsv"

# Minimum area kontur (piksel²) agar dianggap deteksi valid.
MIN_CONTOUR_AREA = 500

# --- Rentang HSV untuk KORBAN ASLI (ORANYE) ---
HSV_ORANGE_LOWER_1 = np.array([0, 70, 70])
HSV_ORANGE_UPPER_1 = np.array([25, 255, 255])
HSV_ORANGE_LOWER_2 = np.array([170, 70, 70])
HSV_ORANGE_UPPER_2 = np.array([180, 255, 255])

# --- Rentang HSV untuk DUMMY (ABU-ABU) ---
HSV_GREY_LOWER = np.array([0, 0, 40])
HSV_GREY_UPPER = np.array([180, 55, 200])

# Morphological operations: iterasi erode & dilate untuk bersihkan mask
MORPH_ERODE_ITERATIONS = 2
MORPH_DILATE_ITERATIONS = 2

# Threshold rasio dominansi warna pada ROI kontur
COLOR_MATCH_THRESHOLD = 0.40

# Jumlah frame untuk voting di state CLASSIFYING (saran: 5 frame)
CLASSIFICATION_VOTE_FRAMES = 5


# ============================================================
# ESTIMASI JARAK
# ============================================================

# Lebar fisik target dalam cm (estimasi rata-rata dummy & korban riil)
# Sesuaikan setelah ukur objek nyata di arena
OBJECT_WIDTH_CM = 20.0

# Focal length kamera dalam piksel — HARUS dikalibrasi!
# Cara kalibrasi:
#   1. Letakkan target pada jarak 50 cm dari kamera
#   2. Ukur lebar bounding box yang terdeteksi (misal 240 px)
#   3. FOCAL_LENGTH_PX = (240 * 50) / 20.0 = 600.0
FOCAL_LENGTH_PX = 600.0

# Batas jarak yang dianggap valid (cm)
DISTANCE_MIN_CM = 0.0
DISTANCE_MAX_CM = 500.0


# ============================================================
# PUBLISHER — Output ke Integration
# ============================================================

# Mode publisher:
#   "stdout"  → print JSON ke terminal (default, untuk testing mandiri)
#   "socket"  → kirim via TCP socket ke Integration
PUBLISHER_MODE = "stdout"

# Konfigurasi socket (hanya dipakai jika PUBLISHER_MODE = "socket")
PUBLISHER_HOST = "localhost"
PUBLISHER_PORT = 5555

# Reconnect settings untuk mode socket
PUBLISHER_RECONNECT_DELAY_S = 1.0    # Delay awal reconnect (detik)
PUBLISHER_RECONNECT_MAX_DELAY_S = 10.0  # Delay maksimal reconnect
PUBLISHER_RECONNECT_MAX_ATTEMPTS = 0    # 0 = unlimited


# ============================================================
# DATA CONTRACT — Threshold dari data_contract.json
# ============================================================

# Confidence minimum agar Integration menerima deteksi
# (di bawah ini, Integration akan mengabaikan)
CONFIDENCE_THRESHOLD = 0.75

# Jarak evakuasi — jika jarak_estimasi < ini, robot masuk CLASSIFYING
EVACUATION_DISTANCE_CM = 15.0

# Timeout data Vision dianggap stale oleh Integration (ms)
VISION_STALE_TIMEOUT_MS = 500

# Label valid sesuai kontrak
VALID_LABELS = ["dummy", "riil", "tidak_ada"]


# ============================================================
# DEBUG & DISPLAY
# ============================================================

# Tampilkan jendela OpenCV dengan overlay bounding box + info?
# Set False jika jalan headless di RPi5 tanpa monitor
SHOW_DISPLAY = True

# Warna overlay (BGR format)
COLOR_BBOX_DUMMY = (160, 160, 160)    # Abu-abu (Dummy)
COLOR_BBOX_RIIL = (0, 255, 0)          # Hijau terang (Korban Asli / Target Utama)
COLOR_BBOX_UNKNOWN = (200, 200, 200)   # Abu-abu muda
COLOR_TEXT = (255, 255, 255)         # Putih

# Target FPS output ke Integration (sesuai data_contract: 10 Hz)
OUTPUT_FPS = 10
