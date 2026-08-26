"""
utils.py — Utilitas umum untuk pipeline Vision.

Berisi:
  - setup_logger: logging terformat
  - validate_output: validasi output sesuai kontrak data
  - draw_overlay: gambar bounding box & info ke frame untuk debugging
"""

import logging
import time
import cv2

from config import (
    VALID_LABELS,
    COLOR_BBOX_DUMMY,
    COLOR_BBOX_RIIL,
    COLOR_BBOX_UNKNOWN,
    COLOR_TEXT,
)


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


def draw_overlay(
    frame,
    bbox: tuple = None,
    label: str = "tidak_ada",
    confidence: float = 0.0,
    distance_cm: float = 0.0,
    is_primary: bool = True,
):
    """
    Menggambar satu bounding box dan informasi deteksi ke frame.
    """
    if bbox is None:
        status_text = "Target: Tidak ada objek terdeteksi"
        cv2.putText(
            frame, status_text, (10, 25),
            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA,
        )
        return frame

    # Pilih warna berdasarkan label
    if label == "riil":
        color = COLOR_BBOX_RIIL       # Hijau terang
    elif label == "dummy":
        color = COLOR_BBOX_DUMMY      # Oranye
    else:
        color = COLOR_BBOX_UNKNOWN

    x, y, w, h = bbox
    thickness = 3 if is_primary else 1

    # Gambar bounding box
    cv2.rectangle(frame, (x, y), (x + w, y + h), color, thickness)

    # Label teks
    tag = "[TARGET] " if is_primary and label == "riil" else ""
    text_label = f"{tag}{label} ({confidence:.0%})"
    text_dist = f"{distance_cm:.1f} cm"

    # Background untuk teks
    font_scale = 0.55 if is_primary else 0.45
    (tw, th), _ = cv2.getTextSize(text_label, cv2.FONT_HERSHEY_SIMPLEX, font_scale, 1)
    cv2.rectangle(frame, (x, max(0, y - th - 8)), (x + tw + 4, max(th + 8, y)), color, -1)
    cv2.putText(
        frame, text_label, (x + 2, max(th + 2, y - 4)),
        cv2.FONT_HERSHEY_SIMPLEX, font_scale, (0, 0, 0) if label == "riil" else COLOR_TEXT, 1, cv2.LINE_AA,
    )

    # Jarak di bawah bbox
    cv2.putText(
        frame, text_dist, (x + 2, min(frame.shape[0] - 5, y + h + 16)),
        cv2.FONT_HERSHEY_SIMPLEX, 0.45, color, 1, cv2.LINE_AA,
    )

    return frame


def draw_multiple_overlays(frame, objects_info: list, primary_index: int = 0):
    """
    Menggambar semua bounding box objek yang terdeteksi dalam frame.

    Args:
        frame: Frame OpenCV.
        objects_info: List dict [{'bbox', 'label', 'confidence', 'dist', ...}].
        primary_index: Index objek yang dipilih sebagai target utama.

    Returns:
        Frame dengan semua overlay.
    """
    if not objects_info:
        return draw_overlay(frame)

    # Gambar semua objek
    for i, obj in enumerate(objects_info):
        is_pri = (i == primary_index)
        draw_overlay(
            frame,
            bbox=obj["bbox"],
            label=obj["label"],
            confidence=obj["confidence"],
            distance_cm=obj["dist"],
            is_primary=is_pri,
        )

    # Summary bar di bagian atas frame
    pri_obj = objects_info[primary_index] if 0 <= primary_index < len(objects_info) else None
    riil_count = sum(1 for o in objects_info if o["label"] == "riil")
    dummy_count = sum(1 for o in objects_info if o["label"] == "dummy")

    top_text = f"Deteksi: {len(objects_info)} obj (Riil: {riil_count}, Dummy: {dummy_count})"
    if pri_obj:
        top_text += f" | Target: {pri_obj['label'].upper()} ({pri_obj['dist']:.1f}cm)"

    cv2.rectangle(frame, (0, 0), (frame.shape[1], 28), (0, 0, 0), -1)
    cv2.putText(
        frame, top_text, (8, 20),
        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 255), 1, cv2.LINE_AA,
    )

    return frame
