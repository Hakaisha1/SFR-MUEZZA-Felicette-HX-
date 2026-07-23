import cv2
import numpy as np
import struct
import time
import sys

# ==============================================================================
#  PENGATURAN - Ubah nilai-nilai di bawah ini sesuai kebutuhan
# ==============================================================================

# --- Warna doll dalam format HSV ---
# Ini HARUS dikalibrasi ulang pakai mode --calibrate
# karena tergantung warna doll dan cahaya di venue lomba.
# H = Hue (0-179), S = Saturation (0-255), V = Value/Brightness (0-255)
WARNA_BAWAH   = (0, 80, 60)      # batas bawah warna doll
WARNA_ATAS    = (25, 255, 255)    # batas atas warna doll

# Kalau doll punya warna yang "wrap-around" di HSV (misal merah),
# butuh range kedua. Set None kalau cuma 1 range.
WARNA_BAWAH_2 = (160, 80, 60)
WARNA_ATAS_2  = (179, 255, 255)

# --- Filter ukuran objek ---
# Kontur yang terlalu kecil = noise, terlalu besar = bukan doll
AREA_MINIMUM  = 800    # pixel, di bawah ini diabaikan
AREA_MAKSIMUM = 60000  # pixel, di atas ini diabaikan

# --- Klasifikasi sudut ---
# Doll dimiringkan 45 derajat = VICTIM (korban asli)
# Doll lurus 0/90 derajat     = DUMMY (palsu)
SUDUT_VICTIM    = 45   # derajat target
TOLERANSI_SUDUT = 15   # derajat, jadi victim = 30-60 derajat

# --- Resolusi kamera ---
LEBAR_FRAME  = 640
TINGGI_FRAME = 480

# --- Serial ke STM32 ---
SERIAL_PORT = "/dev/ttyAMA0"  # port serial di Raspberry Pi
SERIAL_BAUD = 115200          # harus sama dengan setting USART2 di STM32


# ==============================================================================
#  BAGIAN 1: BUKA KAMERA
# ==============================================================================

def buka_kamera():
    # Coba Pi Camera dulu
    try:
        from picamera2 import Picamera2
        picam = Picamera2()
        config = picam.create_video_configuration(
            main={"size": (LEBAR_FRAME, TINGGI_FRAME), "format": "RGB888"}
        )
        picam.configure(config)
        picam.start()
        time.sleep(1.0)  # tunggu auto-exposure stabil
        print("[OK] Kamera: Pi Camera (picamera2)")
        return picam, "picamera2"
    except Exception as e:
        print(f"[INFO] Pi Camera tidak tersedia ({e}), pakai webcam biasa...")

    # Fallback ke webcam USB / laptop
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, LEBAR_FRAME)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, TINGGI_FRAME)
    if cap.isOpened():
        print("[OK] Kamera: Webcam (cv2.VideoCapture)")
        return cap, "opencv"
    else:
        print("[ERROR] Tidak bisa buka kamera manapun!")
        sys.exit(1)


def ambil_frame(kamera, jenis):
    """Ambil 1 frame dari kamera. Return gambar BGR atau None."""
    if jenis == "picamera2":
        frame = kamera.capture_array()
        return cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    else:
        ok, frame = kamera.read()
        return frame if ok else None


def tutup_kamera(kamera, jenis):
    """Matikan kamera."""
    if jenis == "picamera2":
        kamera.stop()
    else:
        kamera.release()


# ==============================================================================
#  BAGIAN 2: DETEKSI DOLL
# ==============================================================================

def buat_mask_warna(frame_hsv):
    # Threshold warna pertama
    mask = cv2.inRange(frame_hsv, WARNA_BAWAH, WARNA_ATAS)

    # Threshold warna kedua (kalau ada)
    if WARNA_BAWAH_2 is not None and WARNA_ATAS_2 is not None:
        mask2 = cv2.inRange(frame_hsv, WARNA_BAWAH_2, WARNA_ATAS_2)
        mask = cv2.bitwise_or(mask, mask2)  # gabungkan: putih di salah satu = putih

    # Bersihkan noise
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))
    mask = cv2.erode(mask, kernel, iterations=1)   # hapus titik-titik kecil
    mask = cv2.dilate(mask, kernel, iterations=2)   # perbesar area yang tersisa
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)  # tutup lubang kecil

    return mask


def normalisasi_sudut(sudut_mentah):
    sudut = sudut_mentah % 180
    if sudut < 0:
        sudut += 180
    return round(sudut, 1)


def apakah_victim(sudut):
    sudut_efektif = sudut % 90  # 0-90 range

    # Hitung jarak ke 45 derajat
    jarak = min(abs(sudut_efektif - SUDUT_VICTIM),
                abs(sudut_efektif - (SUDUT_VICTIM - 90)))

    is_victim = jarak <= TOLERANSI_SUDUT

    # Confidence: makin dekat ke 45° makin tinggi (1.0 = tepat 45°)
    confidence = max(0.0, 1.0 - (jarak / max(TOLERANSI_SUDUT, 1)))
    confidence = round(min(confidence, 1.0), 2)

    return is_victim, confidence


def deteksi_doll(frame):
    # 1. Convert BGR ke HSV
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # 2. Buat mask
    mask = buat_mask_warna(hsv)

    # 3. Cari kontur
    konturs, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Siapkan frame debug (copy supaya frame asli tidak kegambar)
    debug_frame = frame.copy()
    hasil_deteksi = []

    # 4 & 5. Proses tiap kontur
    for kontur in konturs:
        area = cv2.contourArea(kontur)

        # Skip kalau terlalu kecil atau besar
        if area < AREA_MINIMUM or area > AREA_MAKSIMUM:
            continue

        # Hitung kotak miring terkecil yang membungkus kontur
        # Hasilnya: titik tengah (cx,cy), ukuran (w,h), dan sudut rotasi
        kotak = cv2.minAreaRect(kontur)
        (cx, cy), (w, h), sudut_mentah = kotak

        # Normalisasi sudut ke 0-180
        sudut = normalisasi_sudut(sudut_mentah)

        # Klasifikasi: victim atau dummy?
        is_victim, confidence = apakah_victim(sudut)

        # Simpan hasil
        hasil_deteksi.append({
            "cx": int(cx),
            "cy": int(cy),
            "sudut": sudut,
            "is_victim": is_victim,
            "confidence": confidence,
            "area": round(area, 1),
        })

        # --- Gambar overlay di debug frame ---
        titik_kotak = cv2.boxPoints(kotak)
        titik_kotak = np.intp(titik_kotak)
        warna = (0, 255, 0) if is_victim else (0, 0, 255)  # hijau=victim, merah=dummy
        cv2.drawContours(debug_frame, [titik_kotak], 0, warna, 2)

        label = f"{'VICTIM' if is_victim else 'DUMMY'} {sudut:.0f}deg c={confidence:.2f}"
        cv2.putText(debug_frame, label, (int(cx) - 60, int(cy) - 15),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, warna, 2)
        cv2.circle(debug_frame, (int(cx), int(cy)), 4, warna, -1)

    return hasil_deteksi, debug_frame


# ==============================================================================
#  BAGIAN 3: KIRIM DATA KE STM32 VIA SERIAL
# ==============================================================================

def buka_serial(enabled=True):
    if not enabled:
        print("[INFO] Serial dimatikan (mode testing)")
        return None

    try:
        import serial
        ser = serial.Serial(SERIAL_PORT, SERIAL_BAUD, timeout=0.1)
        print(f"[OK] Serial terbuka: {SERIAL_PORT} @ {SERIAL_BAUD} baud")
        return ser
    except Exception as e:
        print(f"[WARNING] Gagal buka serial: {e}")
        print("[INFO] Lanjut tanpa serial (data cuma di-print)")
        return None


def bikin_paket(deteksi_list):
    """
    Ubah list deteksi jadi paket bytes siap kirim ke STM32.
    Lihat format paket di komentar di atas.
    """
    paket = bytearray()
    paket.append(0xAA)  # STX (start)
    paket.append(0x01)  # tipe pesan
    paket.append(min(len(deteksi_list), 255))  # jumlah deteksi

    for det in deteksi_list[:255]:
        cx = max(0, min(det["cx"], 65535))
        cy = max(0, min(det["cy"], 65535))
        sudut = max(0, min(int(round(det["sudut"])), 180))
        victim = 1 if det["is_victim"] else 0
        conf = max(0, min(int(det["confidence"] * 100), 100))

        # Pack: 2 byte cx + 2 byte cy + 1 byte sudut + 1 byte victim + 1 byte conf
        paket += struct.pack(">HHBBB", cx, cy, sudut, victim, conf)

    # Checksum: XOR semua byte
    checksum = 0
    for b in paket:
        checksum ^= b
    paket.append(checksum)

    paket.append(0x55)  # ETX (end)
    return bytes(paket)


def kirim_ke_stm32(ser, deteksi_list):
    """Kirim data deteksi ke STM32 lewat serial."""
    paket = bikin_paket(deteksi_list)

    if ser is not None:
        try:
            ser.write(paket)
        except Exception as e:
            print(f"[ERROR] Gagal kirim serial: {e}")
    else:
        # Mode simulasi: print hex paket
        print(f"  [SIM] Paket ({len(paket)} bytes): {paket.hex()}")


# ==============================================================================
#  BAGIAN 4: MODE KALIBRASI (cari warna HSV yang pas)
# ==============================================================================

def mode_kalibrasi():
    print("=" * 50)
    print("MODE KALIBRASI HSV")
    print("Atur slider sampai cuma doll yang putih di Mask.")
    print("Catat angka H/S/V lalu update variabel di atas.")
    print("Tekan 'q' untuk keluar.")
    print("=" * 50)

    kamera, jenis = buka_kamera()

    cv2.namedWindow("Kalibrasi")
    cv2.createTrackbar("H min", "Kalibrasi", WARNA_BAWAH[0], 179, lambda x: None)
    cv2.createTrackbar("S min", "Kalibrasi", WARNA_BAWAH[1], 255, lambda x: None)
    cv2.createTrackbar("V min", "Kalibrasi", WARNA_BAWAH[2], 255, lambda x: None)
    cv2.createTrackbar("H max", "Kalibrasi", WARNA_ATAS[0], 179, lambda x: None)
    cv2.createTrackbar("S max", "Kalibrasi", WARNA_ATAS[1], 255, lambda x: None)
    cv2.createTrackbar("V max", "Kalibrasi", WARNA_ATAS[2], 255, lambda x: None)

    try:
        while True:
            frame = ambil_frame(kamera, jenis)
            if frame is None:
                continue

            # Baca nilai slider
            h_min = cv2.getTrackbarPos("H min", "Kalibrasi")
            s_min = cv2.getTrackbarPos("S min", "Kalibrasi")
            v_min = cv2.getTrackbarPos("V min", "Kalibrasi")
            h_max = cv2.getTrackbarPos("H max", "Kalibrasi")
            s_max = cv2.getTrackbarPos("S max", "Kalibrasi")
            v_max = cv2.getTrackbarPos("V max", "Kalibrasi")

            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            mask = cv2.inRange(hsv, (h_min, s_min, v_min), (h_max, s_max, v_max))

            cv2.imshow("Frame Asli", frame)
            cv2.imshow("Mask (putih = terdeteksi)", mask)

            key = cv2.waitKey(30) & 0xFF
            if key == ord("q"):
                print(f"\nHasil kalibrasi:")
                print(f"  WARNA_BAWAH = ({h_min}, {s_min}, {v_min})")
                print(f"  WARNA_ATAS  = ({h_max}, {s_max}, {v_max})")
                print("Copy-paste angka di atas ke bagian PENGATURAN di file ini.")
                break
    finally:
        tutup_kamera(kamera, jenis)
        cv2.destroyAllWindows()


# ==============================================================================
#  BAGIAN 5: LOOP UTAMA
# ==============================================================================

def mode_deteksi(pakai_serial=True):
    print("=" * 50)
    print("MODE DETEKSI")
    print("Ctrl+C untuk berhenti")
    print("=" * 50)

    # Buka kamera dan serial
    kamera, jenis = buka_kamera()
    ser = buka_serial(enabled=pakai_serial)

    print("\nMulai deteksi...\n")

    try:
        while True:
            # 1. Ambil frame
            frame = ambil_frame(kamera, jenis)
            if frame is None:
                continue

            # 2. Deteksi doll
            deteksi_list, debug_frame = deteksi_doll(frame)

            # 3. Kalau ada yang terdeteksi, print dan kirim
            if deteksi_list:
                for d in deteksi_list:
                    status = "VICTIM" if d["is_victim"] else "DUMMY"
                    print(f"  [{status}] posisi=({d['cx']},{d['cy']}) "
                          f"sudut={d['sudut']}° confidence={d['confidence']}")

                kirim_ke_stm32(ser, deteksi_list)

            # 4. Tampilkan window debug (tekan 'q' untuk keluar)
            cv2.imshow("Deteksi Doll", debug_frame)
            if cv2.waitKey(1) & 0xFF == ord("q"):
                break

    except KeyboardInterrupt:
        print("\n\nDihentikan (Ctrl+C)")
    finally:
        tutup_kamera(kamera, jenis)
        if ser is not None:
            ser.close()
        cv2.destroyAllWindows()
        print("Selesai, semua resource ditutup.")


# ==============================================================================
#  JALANKAN PROGRAM
# ==============================================================================

if __name__ == "__main__":
    # Cek argumen command line
    if "--calibrate" in sys.argv:
        mode_kalibrasi()
    elif "--no-serial" in sys.argv:
        mode_deteksi(pakai_serial=False)
    else:
        mode_deteksi(pakai_serial=True)
