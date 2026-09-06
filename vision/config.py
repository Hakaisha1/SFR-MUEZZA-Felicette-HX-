"""
config.py — Konfigurasi terpusat untuk pipeline Vision (PixyCam Mode).

Mode deteksi: PixyCam2 via USB (libpixyusb2).
PixyCam melakukan deteksi warna secara onboard di hardware kamera,
sehingga RPi5 hanya menerima data blok (posisi, ukuran, signature)
tanpa perlu memproses frame gambar sama sekali.

Signature warna harus di-training lebih dulu menggunakan software
PixyMon di laptop Windows/Mac.

Nilai default mengacu pada:
  - data_contract.json (threshold confidence, jarak, dsb.)
  - guide.md (frekuensi output, metode deteksi)
"""


# ============================================================
# PIXYCAM2
# ============================================================

# Mode deteksi: "pixy" (PixyCam2 USB) — default
DETECTION_MODE = "pixy"

# Resolusi internal PixyCam2 (tetap, tidak bisa diubah)
# Pixy2 CCC mode: 316 x 208 piksel
PIXY_FRAME_WIDTH = 316
PIXY_FRAME_HEIGHT = 208

# Mapping Signature PixyCam → Label kontrak data
# Training signature di PixyMon:
#   Signature 1 = Korban Asli (ORANYE)
#   Signature 2 = Korban Dummy (ABU-ABU)
PIXY_SIG_RIIL = 1
PIXY_SIG_DUMMY = 2

# Sigmap bitmask: ambil blocks dari signature 1 dan 2
# sigmap = (1 << (sig-1)) untuk tiap signature, lalu OR-kan
# sig1 = 0b01 = 1, sig2 = 0b10 = 2 → sigmap = 0b11 = 3
PIXY_SIGMAP = 3

# Jumlah block maksimal yang diminta dari Pixy2 per frame
PIXY_MAX_BLOCKS = 10

# Minimum area blok (piksel²) agar dianggap deteksi valid
# Pixy2 resolusi 316x208, area minimum ~200 px² sudah cukup kecil
MIN_BLOCK_AREA = 200

# Jumlah frame untuk voting di state CLASSIFYING (saran: 5 frame)
CLASSIFICATION_VOTE_FRAMES = 5


# ============================================================
# ESTIMASI JARAK
# ============================================================

# Lebar fisik target dalam cm (estimasi rata-rata dummy & korban riil)
# Sesuaikan setelah ukur objek nyata di arena
OBJECT_WIDTH_CM = 20.0

# Focal length PixyCam2 dalam piksel — HARUS dikalibrasi!
# Cara kalibrasi (sama seperti sebelumnya):
#   1. Letakkan target pada jarak 50 cm dari PixyCam
#   2. Catat lebar block yang dilaporkan PixyCam (misal 120 px)
#   3. FOCAL_LENGTH_PX = (120 * 50) / 20.0 = 300.0
#
# Catatan: Lensa PixyCam2 berbeda dari webcam USB biasa.
# Nilai default 300.0 adalah tebakan awal, WAJIB dikalibrasi.
FOCAL_LENGTH_PX = 300.0

# Batas jarak yang dianggap valid (cm)
DISTANCE_MIN_CM = 0.0
DISTANCE_MAX_CM = 500.0


# ============================================================
# PUBLISHER — Output ke Integration
# ============================================================

# Mode publisher:
#   "stdout"  → print JSON ke terminal (default, untuk testing mandiri)
#   "socket"  → kirim via TCP socket ke Integration
PUBLISHER_MODE = "socket"

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

# PixyCam mode berjalan headless (tanpa frame gambar), jadi
# display OpenCV tidak relevan. Set False.
SHOW_DISPLAY = False

# Target FPS output ke Integration (sesuai data_contract: 10 Hz)
OUTPUT_FPS = 10
