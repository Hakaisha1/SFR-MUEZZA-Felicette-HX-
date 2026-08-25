"""
test_estimator.py — Unit test untuk modul estimator.

Menguji perhitungan jarak menggunakan metode focal length.

Jalankan: python -m pytest tests/test_estimator.py -v
"""

import sys
import os

import pytest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from estimator import DistanceEstimator


class TestDistanceEstimator:
    """Test suite untuk DistanceEstimator."""

    def setup_method(self):
        """Inisialisasi estimator dengan nilai kalibrasi yang diketahui."""
        # focal_length=600, object_width=20cm
        # Rumus: jarak = (20 × 600) / bbox_width = 12000 / bbox_width
        self.estimator = DistanceEstimator(
            focal_length_px=600.0,
            object_width_cm=20.0,
        )

    def test_known_distance_50cm(self):
        """
        Pada jarak 50 cm, bbox width = (20 × 600) / 50 = 240 px.
        Jadi bbox_width=240 harus menghasilkan ~50 cm.
        """
        distance = self.estimator.estimate(240)
        assert abs(distance - 50.0) < 0.5

    def test_known_distance_100cm(self):
        """
        Pada jarak 100 cm, bbox width = 12000 / 100 = 120 px.
        """
        distance = self.estimator.estimate(120)
        assert abs(distance - 100.0) < 0.5

    def test_known_distance_30cm(self):
        """
        Pada jarak 30 cm, bbox width = 12000 / 30 = 400 px.
        """
        distance = self.estimator.estimate(400)
        assert abs(distance - 30.0) < 0.5

    def test_known_distance_15cm_evacuation(self):
        """
        Pada jarak evakuasi 15 cm (threshold), bbox width = 12000 / 15 = 800 px.
        """
        distance = self.estimator.estimate(800)
        assert abs(distance - 15.0) < 0.5

    def test_zero_width_returns_zero(self):
        """bbox width = 0 → return 0.0 (bukan error/division by zero)."""
        distance = self.estimator.estimate(0)
        assert distance == 0.0

    def test_negative_width_returns_zero(self):
        """bbox width negatif → return 0.0."""
        distance = self.estimator.estimate(-10)
        assert distance == 0.0

    def test_very_small_width_over_max_range(self):
        """
        bbox width sangat kecil → jarak sangat besar → di luar range → 0.0.
        12000 / 1 = 12000 cm > DISTANCE_MAX_CM (500) → return 0.0
        """
        distance = self.estimator.estimate(1)
        assert distance == 0.0

    def test_result_is_float(self):
        """Hasil estimasi harus float."""
        distance = self.estimator.estimate(240)
        assert isinstance(distance, float)

    def test_result_is_rounded(self):
        """Hasil dibulatkan ke 1 desimal."""
        distance = self.estimator.estimate(240)
        assert distance == round(distance, 1)

    def test_distance_decreases_as_bbox_grows(self):
        """Semakin besar bbox → semakin dekat → jarak lebih kecil."""
        d1 = self.estimator.estimate(100)  # jauh
        d2 = self.estimator.estimate(200)  # dekat
        d3 = self.estimator.estimate(400)  # sangat dekat

        # Semua harus valid (> 0)
        assert d1 > 0
        assert d2 > 0
        assert d3 > 0

        # Urutan jarak: d1 > d2 > d3
        assert d1 > d2 > d3

    def test_accuracy_within_5cm_at_50cm(self):
        """
        Akurasi target: error ≤ ±5 cm pada rentang 15–80 cm.
        Test pada jarak 50 cm.
        """
        # Pada 50 cm, bbox = 240 px
        distance = self.estimator.estimate(240)
        assert abs(distance - 50.0) <= 5.0

    def test_accuracy_within_5cm_at_80cm(self):
        """Test akurasi pada batas atas range: 80 cm."""
        # Pada 80 cm, bbox = 12000 / 80 = 150 px
        distance = self.estimator.estimate(150)
        assert abs(distance - 80.0) <= 5.0


class TestCalibration:
    """Test suite untuk method kalibrasi."""

    def test_calibrate_returns_expected_focal(self):
        """Kalibrasi dengan nilai diketahui harus konsisten."""
        # Jika pada 50 cm, bbox = 240 px, object = 20 cm
        # focal = (240 × 50) / 20 = 600
        focal = DistanceEstimator.calibrate(
            bbox_width_px=240,
            known_distance_cm=50.0,
            object_width_cm=20.0,
        )
        assert abs(focal - 600.0) < 0.5

    def test_calibrate_roundtrip(self):
        """Kalibrasi → buat estimator → estimasi harus cocok."""
        # Kalibrasi di 40 cm, bbox 300 px, object 20 cm
        focal = DistanceEstimator.calibrate(
            bbox_width_px=300,
            known_distance_cm=40.0,
            object_width_cm=20.0,
        )

        # Buat estimator baru dengan focal yang dikalibrasi
        estimator = DistanceEstimator(
            focal_length_px=focal,
            object_width_cm=20.0,
        )

        # Estimasi pada bbox 300 px harus ~40 cm
        distance = estimator.estimate(300)
        assert abs(distance - 40.0) < 1.0

    def test_calibrate_invalid_object_width(self):
        """object_width_cm = 0 → ValueError."""
        with pytest.raises(ValueError):
            DistanceEstimator.calibrate(
                bbox_width_px=240,
                known_distance_cm=50.0,
                object_width_cm=0.0,
            )


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
