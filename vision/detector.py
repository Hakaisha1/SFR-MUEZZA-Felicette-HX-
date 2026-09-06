"""
detector.py — Deteksi objek target via PixyCam2 USB.

PixyCam2 melakukan semua proses deteksi warna secara onboard
di hardware kamera. Kita hanya perlu membaca "Blocks" yang
sudah terdeteksi melalui USB API.

Signature mapping:
  Signature 1 = Korban Asli (Oranye) → label "riil"
  Signature 2 = Korban Dummy (Abu-abu) → label "dummy"

Output: list DetectionResult, diurutkan dari area terbesar.
"""

import time
import logging
from dataclasses import dataclass

from config import (
    PIXY_SIG_RIIL,
    PIXY_SIG_DUMMY,
    PIXY_SIGMAP,
    PIXY_MAX_BLOCKS,
    PIXY_FRAME_WIDTH,
    PIXY_FRAME_HEIGHT,
    MIN_BLOCK_AREA,
)

logger = logging.getLogger(__name__)


@dataclass
class DetectionResult:
    """Hasil deteksi satu objek dari PixyCam2."""

    center_x: int        # Koordinat X pusat block (piksel Pixy: 0-315)
    center_y: int        # Koordinat Y pusat block (piksel Pixy: 0-207)
    width: int           # Lebar block (piksel)
    height: int          # Tinggi block (piksel)
    area: int            # Luas block (width * height)
    signature: int       # Nomor signature PixyCam (1-7)
    label: str           # "riil", "dummy", atau "tidak_dikenal"
    confidence: float    # Skor kepercayaan deteksi (0.0–1.0)
    tracking_index: int  # Index tracking dari Pixy2 (untuk objek yang sama antar frame)
    age: int             # Jumlah frame objek ini telah di-track oleh Pixy2


# Mapping Signature → Label
_SIG_TO_LABEL = {
    PIXY_SIG_RIIL: "riil",
    PIXY_SIG_DUMMY: "dummy",
}


def _compute_confidence(area: int, frame_area: int) -> float:
    """
    Hitung confidence berdasarkan rasio area blok terhadap frame.

    Semakin besar blok = semakin dekat = semakin yakin.
    PixyCam2 resolusi 316x208, frame_area = 65,728 px².
    """
    # area_ratio 0.01 (1%) → conf ~0.60
    # area_ratio 0.10 (10%) → conf ~0.90
    # area_ratio 0.30 (30%) → conf ~0.99
    ratio = area / frame_area
    conf = min(0.55 + ratio * 4.0, 0.99)
    return round(conf, 4)


class Detector:
    """
    Detektor target berbasis PixyCam2 USB.

    Membaca Blocks dari Pixy2, memfilter berdasarkan area minimum,
    memetakan Signature ke label, dan mengurutkan dari area terbesar.
    """

    def __init__(self, pixy_interface=None, min_block_area=None):
        """
        Inisialisasi detektor.

        Args:
            pixy_interface: Objek PixyUSB yang sudah diinisialisasi.
                           Jika None, akan di-set nanti via set_pixy().
            min_block_area: Minimum area block agar dianggap valid.
        """
        self._pixy = pixy_interface
        self.min_block_area = min_block_area if min_block_area is not None else MIN_BLOCK_AREA
        self._frame_area = PIXY_FRAME_WIDTH * PIXY_FRAME_HEIGHT

        logger.info("Detector diinisialisasi (mode: PixyCam2 USB, sig_riil=%d, sig_dummy=%d)",
                     PIXY_SIG_RIIL, PIXY_SIG_DUMMY)

    def set_pixy(self, pixy_interface):
        """Set atau ganti interface PixyCam setelah inisialisasi."""
        self._pixy = pixy_interface

    def detect(self) -> list:
        """
        Membaca semua Blocks yang terdeteksi PixyCam2.

        Returns:
            List of DetectionResult, diurutkan dari area terbesar.
            Kosong jika tidak ada deteksi atau pixy belum di-set.
        """
        if self._pixy is None:
            return []

        try:
            blocks = self._pixy.get_blocks(PIXY_SIGMAP, PIXY_MAX_BLOCKS)
        except Exception as e:
            logger.warning("PixyCam read error: %s", e)
            return []

        if not blocks:
            return []

        results = []
        for block in blocks:
            area = block.width * block.height

            if area < self.min_block_area:
                continue

            sig = block.sig
            label = _SIG_TO_LABEL.get(sig, "tidak_dikenal")

            if label == "tidak_dikenal":
                # Abaikan signature yang tidak kita kenal
                continue

            confidence = _compute_confidence(area, self._frame_area)

            results.append(DetectionResult(
                center_x=block.x_center,
                center_y=block.y_center,
                width=block.width,
                height=block.height,
                area=area,
                signature=sig,
                label=label,
                confidence=confidence,
                tracking_index=getattr(block, 'tracking_index', 0),
                age=getattr(block, 'age', 0),
            ))

        # Urutkan dari area terbesar
        results.sort(key=lambda d: d.area, reverse=True)
        return results

    def detect_largest(self):
        """
        Deteksi target terbesar.

        Returns:
            DetectionResult atau None.
        """
        detections = self.detect()
        if detections:
            return detections[0]
        return None
