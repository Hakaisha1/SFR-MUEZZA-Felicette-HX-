"""
test_detector.py — Unit test untuk modul detector.py (PixyCam mode).

Menguji:
  - Deteksi block dari PixyCam (via DummyPixy)
  - Filtering area minimum
  - Mapping signature → label
  - Prioritas target (terbesar duluan)
"""

import unittest
import sys
import os

# Tambahkan parent dir ke path agar bisa import modul vision
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from detector import Detector, DetectionResult
from pixy_usb import DummyPixy, PixyBlock
from config import PIXY_SIG_RIIL, PIXY_SIG_DUMMY, MIN_BLOCK_AREA


class TestDetector(unittest.TestCase):
    """Test deteksi PixyCam2 blocks."""

    def setUp(self):
        self.pixy = DummyPixy()
        self.pixy.init()
        self.detector = Detector(pixy_interface=self.pixy)

    def test_detect_no_blocks_returns_empty(self):
        """Tidak ada block → return list kosong."""
        self.pixy.set_blocks([])
        result = self.detector.detect()
        self.assertEqual(result, [])

    def test_detect_no_pixy_returns_empty(self):
        """Pixy belum di-set → return list kosong."""
        detector = Detector(pixy_interface=None)
        result = detector.detect()
        self.assertEqual(result, [])

    def test_detect_orange_as_riil(self):
        """Block signature 1 (oranye) → label 'riil'."""
        self.pixy.set_blocks([
            PixyBlock(sig=PIXY_SIG_RIIL, x_center=160, y_center=100,
                      width=80, height=60, angle=0, tracking_index=0, age=5),
        ])
        result = self.detector.detect()
        self.assertEqual(len(result), 1)
        self.assertEqual(result[0].label, "riil")
        self.assertEqual(result[0].signature, PIXY_SIG_RIIL)

    def test_detect_grey_as_dummy(self):
        """Block signature 2 (abu-abu) → label 'dummy'."""
        self.pixy.set_blocks([
            PixyBlock(sig=PIXY_SIG_DUMMY, x_center=100, y_center=80,
                      width=60, height=50, angle=0, tracking_index=1, age=3),
        ])
        result = self.detector.detect()
        self.assertEqual(len(result), 1)
        self.assertEqual(result[0].label, "dummy")

    def test_detect_unknown_signature_filtered(self):
        """Block dengan signature tidak dikenal (misal 5) → dibuang."""
        self.pixy.set_blocks([
            PixyBlock(sig=5, x_center=100, y_center=80,
                      width=60, height=50, angle=0, tracking_index=0, age=1),
        ])
        result = self.detector.detect()
        self.assertEqual(len(result), 0)

    def test_tiny_block_filtered_out(self):
        """Block dengan area < MIN_BLOCK_AREA → dibuang."""
        small_size = int(MIN_BLOCK_AREA ** 0.5) - 1  # Lebih kecil dari minimum
        self.pixy.set_blocks([
            PixyBlock(sig=PIXY_SIG_RIIL, x_center=100, y_center=80,
                      width=small_size, height=small_size, angle=0,
                      tracking_index=0, age=1),
        ])
        result = self.detector.detect()
        self.assertEqual(len(result), 0)

    def test_detect_sorted_by_area_descending(self):
        """Hasil deteksi diurutkan dari area terbesar."""
        self.pixy.set_blocks([
            PixyBlock(sig=PIXY_SIG_DUMMY, x_center=50, y_center=50,
                      width=30, height=20, angle=0, tracking_index=0, age=1),
            PixyBlock(sig=PIXY_SIG_RIIL, x_center=160, y_center=100,
                      width=100, height=80, angle=0, tracking_index=1, age=5),
        ])
        result = self.detector.detect()
        self.assertEqual(len(result), 2)
        self.assertGreater(result[0].area, result[1].area)
        self.assertEqual(result[0].label, "riil")

    def test_detect_largest_returns_biggest(self):
        """detect_largest() mengembalikan block terbesar."""
        self.pixy.set_blocks([
            PixyBlock(sig=PIXY_SIG_DUMMY, x_center=50, y_center=50,
                      width=30, height=20, angle=0, tracking_index=0, age=1),
            PixyBlock(sig=PIXY_SIG_RIIL, x_center=160, y_center=100,
                      width=100, height=80, angle=0, tracking_index=1, age=5),
        ])
        result = self.detector.detect_largest()
        self.assertIsNotNone(result)
        self.assertEqual(result.label, "riil")
        self.assertEqual(result.area, 100 * 80)

    def test_detect_largest_no_blocks_returns_none(self):
        """detect_largest() tanpa block → None."""
        self.pixy.set_blocks([])
        result = self.detector.detect_largest()
        self.assertIsNone(result)

    def test_confidence_in_valid_range(self):
        """Confidence selalu dalam rentang 0.0–1.0."""
        self.pixy.set_blocks([
            PixyBlock(sig=PIXY_SIG_RIIL, x_center=160, y_center=100,
                      width=200, height=150, angle=0, tracking_index=0, age=10),
        ])
        result = self.detector.detect()
        self.assertEqual(len(result), 1)
        self.assertGreaterEqual(result[0].confidence, 0.0)
        self.assertLessEqual(result[0].confidence, 1.0)

    def test_detection_result_fields(self):
        """DetectionResult memiliki semua field yang diperlukan."""
        self.pixy.set_blocks([
            PixyBlock(sig=PIXY_SIG_RIIL, x_center=160, y_center=100,
                      width=80, height=60, angle=0, tracking_index=3, age=7),
        ])
        result = self.detector.detect()
        det = result[0]
        self.assertEqual(det.center_x, 160)
        self.assertEqual(det.center_y, 100)
        self.assertEqual(det.width, 80)
        self.assertEqual(det.height, 60)
        self.assertEqual(det.area, 80 * 60)
        self.assertEqual(det.tracking_index, 3)
        self.assertEqual(det.age, 7)


if __name__ == "__main__":
    unittest.main()
