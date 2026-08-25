"""
test_detector.py — Unit test untuk modul detector.

Menguji deteksi objek target Oranye (Riil) dan Abu-abu (Dummy).

Jalankan: python -m pytest tests/test_detector.py -v
"""

import sys
import os

import cv2
import numpy as np
import pytest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from detector import Detector, DetectionResult


def create_blank_frame(width=640, height=480):
    """Buat frame hitam kosong."""
    return np.zeros((height, width, 3), dtype=np.uint8)


def create_frame_with_orange_object(width=640, height=480, obj_size=100):
    """Buat frame dengan objek oranye (Korban Asli) di tengah."""
    frame = create_blank_frame(width, height)
    cx, cy = width // 2, height // 2
    x1, y1 = cx - obj_size // 2, cy - obj_size // 2
    x2, y2 = cx + obj_size // 2, cy + obj_size // 2

    cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 140, 255), -1)
    return frame


def create_frame_with_grey_object(width=640, height=480, obj_size=100):
    """Buat frame dengan objek abu-abu (Dummy) di tengah."""
    frame = create_blank_frame(width, height)
    cx, cy = width // 2, height // 2
    x1, y1 = cx - obj_size // 2, cy - obj_size // 2
    x2, y2 = cx + obj_size // 2, cy + obj_size // 2

    cv2.rectangle(frame, (x1, y1), (x2, y2), (128, 128, 128), -1)
    return frame


def create_frame_with_tiny_noise(width=640, height=480):
    """Buat frame dengan noise kecil (area < MIN_CONTOUR_AREA)."""
    frame = create_blank_frame(width, height)
    cv2.rectangle(frame, (300, 200), (305, 205), (0, 140, 255), -1)
    return frame


class TestDetector:
    """Test suite untuk Detector."""

    def setup_method(self):
        self.detector = Detector()

    def test_detect_blank_frame_returns_empty(self):
        """Frame kosong -> tidak ada deteksi."""
        frame = create_blank_frame()
        results = self.detector.detect(frame)
        assert results == []

    def test_detect_none_frame_returns_empty(self):
        """Frame None -> tidak ada deteksi."""
        results = self.detector.detect(None)
        assert results == []

    def test_detect_orange_object(self):
        """Frame dengan objek oranye -> terdeteksi."""
        frame = create_frame_with_orange_object(obj_size=100)
        results = self.detector.detect(frame)

        assert len(results) >= 1
        detection = results[0]

        assert isinstance(detection, DetectionResult)
        assert detection.source == "orange"
        assert detection.area >= 500
        assert detection.confidence > 0.0

    def test_detect_grey_object(self):
        """Frame dengan objek abu-abu -> terdeteksi."""
        frame = create_frame_with_grey_object(obj_size=100)
        results = self.detector.detect(frame)

        assert len(results) >= 1
        detection = results[0]

        assert isinstance(detection, DetectionResult)
        assert detection.source == "grey"
        assert detection.area >= 500
        assert detection.confidence > 0.0

    def test_detect_orange_object_center(self):
        """Pusat deteksi objek oranye harus dekat pusat frame."""
        frame = create_frame_with_orange_object(width=640, height=480, obj_size=100)
        detection = self.detector.detect_largest(frame)

        assert detection is not None
        assert abs(detection.center_x - 320) <= 10
        assert abs(detection.center_y - 240) <= 10

    def test_tiny_noise_filtered_out(self):
        """Objek terlalu kecil (di bawah MIN_CONTOUR_AREA) -> diabaikan."""
        frame = create_frame_with_tiny_noise()
        results = self.detector.detect(frame)
        assert results == []

    def test_detect_largest_returns_biggest(self):
        """Jika ada 2 objek, detect_largest() mengembalikan yang terbesar."""
        frame = create_blank_frame()
        cv2.rectangle(frame, (50, 50), (200, 200), (0, 140, 255), -1)
        cv2.rectangle(frame, (400, 200), (450, 250), (128, 128, 128), -1)

        detection = self.detector.detect_largest(frame)
        assert detection is not None
        assert detection.center_x < 320

    def test_detect_largest_no_object_returns_none(self):
        """detect_largest() pada frame kosong -> None."""
        frame = create_blank_frame()
        result = self.detector.detect_largest(frame)
        assert result is None

    def test_detection_result_fields(self):
        """Verifikasi semua field DetectionResult terisi."""
        frame = create_frame_with_orange_object(obj_size=100)
        detection = self.detector.detect_largest(frame)

        assert detection is not None
        assert isinstance(detection.bbox, tuple)
        assert len(detection.bbox) == 4
        assert isinstance(detection.center_x, int)
        assert isinstance(detection.center_y, int)
        assert isinstance(detection.width, int)
        assert isinstance(detection.height, int)
        assert isinstance(detection.area, (int, float))
        assert isinstance(detection.confidence, float)
        assert isinstance(detection.source, str)


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
