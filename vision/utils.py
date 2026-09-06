"""
utils.py — Utilitas umum untuk pipeline Vision (PixyCam Mode).

Berisi:
  - setup_logger: logging terformat
  - validate_output: validasi output sesuai kontrak data
  - build_output: membangun dict output sesuai kontrak

Catatan: Tidak ada lagi fungsi draw_overlay karena PixyCam
tidak menyediakan frame gambar mentah. Semua debug dilakukan
via log teks.
"""

import logging
import time

from config import VALID_LABELS


def setup_logger(name: str, level: int = logging.INFO) -> logging.Logger:
    """
    Membuat logger dengan format terstruktur.

    Args:
        name: Nama logger (biasanya __name__ dari modul pemanggil).
        level: Level logging (default: INFO).

    Returns:
        Instance logging.Logger yang sudah dikonfigurasi.
    """
    logger = logging.getLogger(name)

    # Hindari duplikasi handler jika dipanggil berkali-kali
    if not logger.handlers:
        handler = logging.StreamHandler()
        formatter = logging.Formatter(
            fmt="[%(asctime)s] %(levelname)-8s %(name)s: %(message)s",
            datefmt="%H:%M:%S",
        )
        handler.setFormatter(formatter)
        logger.addHandler(handler)

    logger.setLevel(level)
    return logger


def validate_output(data: dict) -> bool:
    """
    Validasi bahwa dict output sesuai dengan kontrak data Vision → Integration.

    Kontrak (dari data_contract.json):
      - label: string, salah satu dari ["dummy", "riil", "tidak_ada"]
      - confidence: float, 0.0 – 1.0
      - posisi_x_px: int
      - posisi_y_px: int
      - jarak_estimasi_cm: float
      - timestamp_ms: int

    Args:
        data: Dictionary output Vision.

    Returns:
        True jika valid, False jika ada field yang salah.
    """
    required_fields = {
        "label": str,
        "confidence": (int, float),
        "posisi_x_px": int,
        "posisi_y_px": int,
        "jarak_estimasi_cm": (int, float),
        "timestamp_ms": int,
    }

    for field, expected_type in required_fields.items():
        if field not in data:
            return False
        if not isinstance(data[field], expected_type):
            return False

    # Validasi nilai
    if data["label"] not in VALID_LABELS:
        return False
    if not (0.0 <= data["confidence"] <= 1.0):
        return False
    if data["posisi_x_px"] < 0 or data["posisi_y_px"] < 0:
        return False

    return True


def build_output(
    label: str = "tidak_ada",
    confidence: float = 0.0,
    posisi_x_px: int = 0,
    posisi_y_px: int = 0,
    jarak_estimasi_cm: float = 0.0,
) -> dict:
    """
    Membangun dict output sesuai kontrak data.
    Otomatis mengisi timestamp_ms.

    Args:
        label: Hasil klasifikasi ("dummy", "riil", "tidak_ada").
        confidence: Keyakinan klasifikasi (0.0–1.0).
        posisi_x_px: Koordinat X pusat bounding box (piksel).
        posisi_y_px: Koordinat Y pusat bounding box (piksel).
        jarak_estimasi_cm: Estimasi jarak ke target (cm).

    Returns:
        Dictionary output sesuai format kontrak.
    """
    return {
        "label": str(label),
        "confidence": round(float(confidence), 4),
        "posisi_x_px": int(posisi_x_px),
        "posisi_y_px": int(posisi_y_px),
        "jarak_estimasi_cm": round(float(jarak_estimasi_cm), 1),
        "timestamp_ms": int(time.time() * 1000),
    }
