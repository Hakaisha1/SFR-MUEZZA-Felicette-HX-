"""
estimator.py — Estimasi jarak robot ke target.

Menggunakan metode focal length (pinhole camera model):
    jarak = (lebar_objek_nyata × focal_length) / lebar_bbox_px

Akurasi target: error ≤ ±5 cm pada rentang 15–80 cm (M1 deliverable).

KALIBRASI WAJIB:
  1. Letakkan target pada jarak DIKETAHUI (misal 50 cm)
  2. Ukur lebar bounding box (misal 240 px)
  3. FOCAL_LENGTH_PX = (lebar_bbox_px × jarak_cm) / OBJECT_WIDTH_CM
     = (240 × 50) / 20 = 600
"""

from config import (
    FOCAL_LENGTH_PX,
    OBJECT_WIDTH_CM,
    DISTANCE_MIN_CM,
    DISTANCE_MAX_CM,
)
from utils import setup_logger

logger = setup_logger(__name__)


class DistanceEstimator:
    """
    Estimator jarak menggunakan metode focal length.

    Asumsi:
      - Lebar fisik target diketahui dan konsisten.
      - Kamera sudah dikalibrasi (focal length dalam piksel).
      - Target menghadap kamera secara frontal (tidak miring).
    """

    def __init__(
        self,
        focal_length_px: float = None,
        object_width_cm: float = None,
    ):
        """
        Inisialisasi estimator.

        Args:
            focal_length_px: Focal length kamera dalam piksel.
                Harus dikalibrasi dari pengukuran nyata. Default dari config.
            object_width_cm: Lebar fisik target dalam cm. Default dari config.
        """
        self.focal_length = focal_length_px if focal_length_px is not None else FOCAL_LENGTH_PX
        self.object_width = object_width_cm if object_width_cm is not None else OBJECT_WIDTH_CM

        logger.info(
            "DistanceEstimator diinisialisasi: "
            "focal=%.1f px, object_width=%.1f cm",
            self.focal_length, self.object_width,
        )

    def estimate(self, bbox_width_px: float) -> float:
        """
        Menghitung estimasi jarak dari lebar bounding box.

        Args:
            bbox_width_px: Lebar bounding box target dalam piksel.

        Returns:
            Estimasi jarak dalam sentimeter (cm).
            Mengembalikan 0.0 jika:
              - bbox_width_px <= 0
              - Hasil di luar range valid (DISTANCE_MIN_CM – DISTANCE_MAX_CM)
        """
        if bbox_width_px <= 0:
            return 0.0

        distance = (self.object_width * self.focal_length) / bbox_width_px

        # Validasi range
        if distance < DISTANCE_MIN_CM or distance > DISTANCE_MAX_CM:
            logger.warning(
                "Jarak di luar range: %.1f cm (bbox_width=%d px)",
                distance, bbox_width_px,
            )
            return 0.0

        return round(distance, 1)

    @staticmethod
    def calibrate(bbox_width_px: float, known_distance_cm: float, object_width_cm: float) -> float:
        """
        Menghitung focal length dari pengukuran kalibrasi.

        Cara pakai:
          1. Letakkan target pada jarak diketahui
          2. Deteksi target, catat lebar bbox
          3. Panggil method ini untuk dapat focal length
          4. Update config.FOCAL_LENGTH_PX

        Args:
            bbox_width_px: Lebar bounding box yang terukur (piksel).
            known_distance_cm: Jarak nyata target ke kamera (cm).
            object_width_cm: Lebar fisik target (cm).

        Returns:
            Focal length dalam piksel.
        """
        if object_width_cm <= 0:
            raise ValueError("object_width_cm harus > 0")

        focal = (bbox_width_px * known_distance_cm) / object_width_cm

        logger.info(
            "Kalibrasi: bbox=%d px, jarak=%.1f cm, lebar=%.1f cm → focal=%.1f px",
            bbox_width_px, known_distance_cm, object_width_cm, focal,
        )

        return round(focal, 1)
