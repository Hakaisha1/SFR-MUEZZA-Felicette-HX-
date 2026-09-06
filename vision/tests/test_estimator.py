"""
test_estimator.py — Unit test untuk modul estimator.py.

Menguji:
  - Estimasi jarak berdasarkan lebar block PixyCam
  - Kalibrasi focal length
  - Edge cases (width 0, negatif, sangat kecil)

Catatan: estimator.py tidak bergantung pada hardware PixyCam
atau OpenCV. Ia hanya menerima angka lebar (piksel) dan
menghitung jarak (cm). Unit test ini tetap valid.
"""

import unittest
import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from estimator import DistanceEstimator


class TestDistanceEstimator(unittest.TestCase):
    """Test estimasi jarak dari lebar block PixyCam."""

    def setUp(self):
        # focal=300, object_width=20cm (default PixyCam config)
        self.est = DistanceEstimator(focal_length_px=300.0, object_width_cm=20.0)

    def test_known_distance_50cm(self):
        """
        Pada jarak 50cm, lebar block = (20 * 300) / 50 = 120 px.
        Estimasi: (20 * 300) / 120 = 50.0 cm.
        """
        result = self.est.estimate(120)
        self.assertAlmostEqual(result, 50.0, places=1)

    def test_known_distance_100cm(self):
        """
        Pada jarak 100cm, lebar block = (20 * 300) / 100 = 60 px.
        """
        result = self.est.estimate(60)
        self.assertAlmostEqual(result, 100.0, places=1)

    def test_known_distance_30cm(self):
        """
        Pada jarak 30cm, lebar block = (20 * 300) / 30 = 200 px.
        """
        result = self.est.estimate(200)
        self.assertAlmostEqual(result, 30.0, places=1)

    def test_known_distance_15cm_evacuation(self):
        """
        Pada jarak evakuasi 15cm, lebar block = (20 * 300) / 15 = 400 px.
        Catatan: Pixy2 resolusi 316px, jadi ini di luar frame — but math works.
        """
        result = self.est.estimate(400)
        self.assertAlmostEqual(result, 15.0, places=1)

    def test_zero_width_returns_zero(self):
        """Lebar 0 → return 0.0 (hindari division by zero)."""
        result = self.est.estimate(0)
        self.assertEqual(result, 0.0)

    def test_negative_width_returns_zero(self):
        """Lebar negatif → return 0.0."""
        result = self.est.estimate(-10)
        self.assertEqual(result, 0.0)

    def test_very_small_width_over_max_range(self):
        """
        Lebar sangat kecil → jarak melebihi DISTANCE_MAX_CM → return 0.0.
        width=1 → distance = 6000 cm → over max 500 cm.
        """
        result = self.est.estimate(1)
        self.assertEqual(result, 0.0)

    def test_result_is_float(self):
        """Hasil selalu float."""
        result = self.est.estimate(120)
        self.assertIsInstance(result, float)

    def test_result_is_rounded(self):
        """Hasil dibulatkan ke 1 desimal."""
        result = self.est.estimate(77)
        # 6000 / 77 = 77.922... → 77.9
        self.assertEqual(result, round(6000.0 / 77, 1))

    def test_distance_decreases_as_block_grows(self):
        """Semakin besar block (dekat) → jarak semakin kecil."""
        d_far = self.est.estimate(30)
        d_near = self.est.estimate(150)
        self.assertGreater(d_far, d_near)

    def test_accuracy_within_5cm_at_50cm(self):
        """Di jarak 50cm, estimasi error ≤ ±5 cm."""
        # Seharusnya tepat di 50 cm
        result = self.est.estimate(120)
        self.assertAlmostEqual(result, 50.0, delta=5.0)

    def test_accuracy_within_5cm_at_80cm(self):
        """Di jarak 80cm, estimasi error ≤ ±5 cm."""
        # width = 6000/80 = 75 px
        result = self.est.estimate(75)
        self.assertAlmostEqual(result, 80.0, delta=5.0)


class TestCalibration(unittest.TestCase):
    """Test kalibrasi focal length."""

    def test_calibrate_returns_expected_focal(self):
        """Kalibrasi: bbox=120px, jarak=50cm, lebar=20cm → focal=300."""
        focal = DistanceEstimator.calibrate(120, 50.0, 20.0)
        self.assertEqual(focal, 300.0)

    def test_calibrate_roundtrip(self):
        """Kalibrasi → estimasi harus konsisten."""
        focal = DistanceEstimator.calibrate(120, 50.0, 20.0)
        est = DistanceEstimator(focal_length_px=focal, object_width_cm=20.0)
        result = est.estimate(120)
        self.assertAlmostEqual(result, 50.0, places=1)

    def test_calibrate_invalid_object_width(self):
        """object_width_cm ≤ 0 → ValueError."""
        with self.assertRaises(ValueError):
            DistanceEstimator.calibrate(120, 50.0, 0.0)


if __name__ == "__main__":
    unittest.main()
