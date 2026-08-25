"""
detector.py — Deteksi objek target (Korban Asli Oranye & Dummy Abu-abu).

Target di arena:
  1. Korban Asli (Riil): Warna ORANYE (Saturasi tinggi).
  2. Korban Dummy: Warna ABU-ABU (Saturasi rendah).

Output: list DetectionResult, diurutkan dari area terbesar.
"""

import cv2
import numpy as np
from dataclasses import dataclass

from config import (
    HSV_ORANGE_LOWER_1,
    HSV_ORANGE_UPPER_1,
    HSV_ORANGE_LOWER_2,
    HSV_ORANGE_UPPER_2,
    HSV_GREY_LOWER,
    HSV_GREY_UPPER,
    MIN_CONTOUR_AREA,
    MORPH_ERODE_ITERATIONS,
    MORPH_DILATE_ITERATIONS,
)
from utils import setup_logger

logger = setup_logger(__name__)


@dataclass
class DetectionResult:
    """Hasil deteksi satu objek dalam frame."""

    bbox: tuple          # (x, y, w, h) bounding box
    center_x: int        # Koordinat X pusat bbox (piksel)
    center_y: int        # Koordinat Y pusat bbox (piksel)
    width: int           # Lebar bbox (piksel) — untuk estimasi jarak
    height: int          # Tinggi bbox (piksel)
    area: float          # Luas kontur (piksel²)
    confidence: float    # Skor kepercayaan deteksi (0.0–1.0)
    source: str          # Sumber warna deteksi: "orange" atau "grey"
    contour: np.ndarray  # Kontur OpenCV objek


class Detector:
    """
    Detektor objek boneka korban berbasis HSV color masking:
      - Oranye (Korban Asli)
      - Abu-abu (Dummy)
    """

    def __init__(
        self,
        min_contour_area=None,
    ):
        """
        Inisialisasi detektor.

        Args:
            min_contour_area: Minimum area kontur (piksel²). Default dari config.
        """
        self.min_contour_area = min_contour_area if min_contour_area is not None else MIN_CONTOUR_AREA

        logger.info("Detector diinisialisasi (mode: Orange Riil + Grey Dummy HSV)")

    def _create_orange_mask(self, hsv_frame):
        """Binary mask untuk warna oranye (Korban Asli)."""
        mask1 = cv2.inRange(hsv_frame, HSV_ORANGE_LOWER_1, HSV_ORANGE_UPPER_1)
        mask2 = cv2.inRange(hsv_frame, HSV_ORANGE_LOWER_2, HSV_ORANGE_UPPER_2)
        mask = cv2.bitwise_or(mask1, mask2)
        mask = cv2.erode(mask, None, iterations=MORPH_ERODE_ITERATIONS)
        mask = cv2.dilate(mask, None, iterations=MORPH_DILATE_ITERATIONS)
        return mask

    def _create_grey_mask(self, hsv_frame):
        """Binary mask untuk warna abu-abu (Dummy)."""
        mask = cv2.inRange(hsv_frame, HSV_GREY_LOWER, HSV_GREY_UPPER)
        mask = cv2.erode(mask, None, iterations=MORPH_ERODE_ITERATIONS)
        mask = cv2.dilate(mask, None, iterations=MORPH_DILATE_ITERATIONS)
        return mask

    def _find_contours_for_mask(self, mask, source_name: str, frame_area: int) -> list:
        """Ekstraksi kontur dari satu binary mask."""
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        results = []
        for contour in contours:
            area = cv2.contourArea(contour)
            if area < self.min_contour_area:
                continue

            x, y, w, h = cv2.boundingRect(contour)
            center_x = x + w // 2
            center_y = y + h // 2
            confidence = min(area / (frame_area * 0.01), 1.0)

            results.append(DetectionResult(
                bbox=(x, y, w, h),
                center_x=center_x,
                center_y=center_y,
                width=w,
                height=h,
                area=area,
                confidence=float(confidence),
                source=source_name,
                contour=contour,
            ))
        return results

    def detect(self, frame) -> list:
        """
        Mendeteksi seluruh target (Oranye & Abu-abu) dalam frame.

        Args:
            frame: Frame BGR dari kamera (numpy array).

        Returns:
            List of DetectionResult, diurutkan dari area terbesar.
        """
        if frame is None or frame.size == 0:
            return []

        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        frame_area = frame.shape[0] * frame.shape[1]

        # 1. Deteksi Oranye (Riil)
        mask_orange = self._create_orange_mask(hsv)
        orange_dets = self._find_contours_for_mask(mask_orange, "orange", frame_area)

        # 2. Deteksi Abu-abu (Dummy)
        mask_grey = self._create_grey_mask(hsv)
        grey_dets = self._find_contours_for_mask(mask_grey, "grey", frame_area)

        # Gabungkan deteksi
        all_detections = orange_dets + grey_dets

        # Urutkan dari area terbesar
        all_detections.sort(key=lambda d: d.area, reverse=True)
        return all_detections

    def detect_largest(self, frame):
        """
        Mendeteksi target terbesar dalam frame.

        Args:
            frame: Frame BGR dari kamera.

        Returns:
            DetectionResult atau None jika tidak ada deteksi.
        """
        detections = self.detect(frame)
        if detections:
            return detections[0]
        return None
