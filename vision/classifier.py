"""
classifier.py — Klasifikasi target: Korban Asli (Oranye) vs Dummy (Abu-abu).

Strategi klasifikasi warna:
  - Korban Asli (Riil): Warna ORANYE (Saturasi tinggi S >= 70, H = 0-25 / 170-180).
  - Korban Dummy: Warna ABU-ABU (Saturasi rendah S <= 55, V = 40-200).
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
    COLOR_MATCH_THRESHOLD,
)
from utils import setup_logger

logger = setup_logger(__name__)


@dataclass
class ClassificationResult:
    """Hasil klasifikasi satu objek."""

    label: str           # "riil" (Oranye) atau "dummy" (Abu-abu)
    confidence: float    # Keyakinan klasifikasi (0.0–1.0)
    scores: dict         # Detail skor perbandingan warna


class Classifier:
    """
    Pengklasifikasi target berdasarkan analisis warna (Oranye Asli vs Abu-abu Dummy).
    """

    def __init__(self, color_match_threshold=None):
        self.color_match_threshold = (
            color_match_threshold
            if color_match_threshold is not None
            else COLOR_MATCH_THRESHOLD
        )
        logger.info("Classifier diinisialisasi (mode: Color Differentiation Oranye vs Abu-abu)")

    def _get_orange_mask(self, roi_hsv):
        """Mask piksel oranye dalam ROI."""
        mask1 = cv2.inRange(roi_hsv, HSV_ORANGE_LOWER_1, HSV_ORANGE_UPPER_1)
        mask2 = cv2.inRange(roi_hsv, HSV_ORANGE_LOWER_2, HSV_ORANGE_UPPER_2)
        return cv2.bitwise_or(mask1, mask2)

    def _get_grey_mask(self, roi_hsv):
        """Mask piksel abu-abu dalam ROI."""
        return cv2.inRange(roi_hsv, HSV_GREY_LOWER, HSV_GREY_UPPER)

    def classify(self, frame, bbox: tuple, contour=None) -> ClassificationResult:
        """
        Mengklasifikasi objek: Korban Asli ('riil') atau Dummy ('dummy').

        Args:
            frame: Frame BGR dari kamera.
            bbox: Tuple (x, y, w, h) bounding box objek.
            contour: Kontur OpenCV objek (opsional).

        Returns:
            ClassificationResult dengan label, confidence, dan rincian skor.
        """
        x, y, w, h = bbox

        frame_h, frame_w = frame.shape[:2]
        x = max(0, x)
        y = max(0, y)
        w = min(w, frame_w - x)
        h = min(h, frame_h - y)

        if w <= 0 or h <= 0:
            return ClassificationResult(
                label="tidak_ada",
                confidence=0.0,
                scores={"error": "invalid_bbox"},
            )

        # Ekstrak ROI dalam HSV
        roi_bgr = frame[y:y + h, x:x + w]
        roi_hsv = cv2.cvtColor(roi_bgr, cv2.COLOR_BGR2HSV)
        total_pixels = float(w * h)

        # Hitung rasio piksel oranye dan abu-abu
        orange_mask = self._get_orange_mask(roi_hsv)
        grey_mask = self._get_grey_mask(roi_hsv)

        orange_pixels = float(np.count_nonzero(orange_mask))
        grey_pixels = float(np.count_nonzero(grey_mask))

        orange_ratio = orange_pixels / total_pixels
        grey_ratio = grey_pixels / total_pixels

        mean_sat = float(np.mean(roi_hsv[:, :, 1]))
        mean_val = float(np.mean(roi_hsv[:, :, 2]))

        # Logika Keputusan:
        # 1. Jika rasio oranye dominan dan saturasi tinggi -> RIIL
        if orange_ratio > grey_ratio and orange_ratio >= 0.10:
            label = "riil"
            confidence = min(0.60 + (orange_ratio * 0.40), 0.99)
        # 2. Jika rasio abu-abu dominan atau saturasi rendah -> DUMMY
        elif grey_ratio > orange_ratio and grey_ratio >= 0.10:
            label = "dummy"
            confidence = min(0.60 + (grey_ratio * 0.40), 0.99)
        # 3. Fallback berdasarkan rata-rata saturasi ROI
        elif mean_sat >= 65:
            label = "riil"
            confidence = min(0.50 + (mean_sat / 255.0) * 0.40, 0.95)
        else:
            label = "dummy"
            confidence = min(0.50 + ((255.0 - mean_sat) / 255.0) * 0.40, 0.95)

        scores = {
            "orange_ratio": round(orange_ratio, 4),
            "grey_ratio": round(grey_ratio, 4),
            "mean_saturation": round(mean_sat, 2),
            "mean_value": round(mean_val, 2),
        }

        logger.debug(
            "Klasifikasi warna: %s (conf=%.2f) scores=%s",
            label, confidence, scores,
        )

        return ClassificationResult(
            label=label,
            confidence=round(confidence, 4),
            scores=scores,
        )


class ClassificationVoter:
    """
    Melakukan voting dari beberapa frame untuk keputusan final di state CLASSIFYING.
    Sesuai state_machine.md: ambil minimal 5 frame dan voting mayoritas.
    """

    def __init__(self, required_frames: int = 5):
        self.required_frames = required_frames
        self.votes = []
        logger.info("ClassificationVoter: butuh %d frame", required_frames)

    def add_vote(self, result: ClassificationResult):
        self.votes.append(result)

    def is_ready(self) -> bool:
        return len(self.votes) >= self.required_frames

    def get_verdict(self) -> ClassificationResult:
        if not self.votes:
            return ClassificationResult(
                label="tidak_ada", confidence=0.0, scores={"votes": 0}
            )

        dummy_votes = [v for v in self.votes if v.label == "dummy"]
        riil_votes = [v for v in self.votes if v.label == "riil"]

        total = len(self.votes)

        if len(dummy_votes) > len(riil_votes):
            label = "dummy"
            winner_votes = dummy_votes
        elif len(riil_votes) > len(dummy_votes):
            label = "riil"
            winner_votes = riil_votes
        else:
            avg_dummy = (
                sum(v.confidence for v in dummy_votes) / len(dummy_votes)
                if dummy_votes else 0.0
            )
            avg_riil = (
                sum(v.confidence for v in riil_votes) / len(riil_votes)
                if riil_votes else 0.0
            )
            if avg_dummy >= avg_riil:
                label = "dummy"
                winner_votes = dummy_votes
            else:
                label = "riil"
                winner_votes = riil_votes

        vote_ratio = len(winner_votes) / total
        avg_conf = sum(v.confidence for v in winner_votes) / len(winner_votes)
        final_confidence = vote_ratio * avg_conf

        scores = {
            "dummy_count": len(dummy_votes),
            "riil_count": len(riil_votes),
            "total_frames": total,
            "vote_ratio": round(vote_ratio, 4),
            "avg_winner_confidence": round(avg_conf, 4),
        }

        logger.info(
            "Voting: %s (%d/%d, conf=%.2f)",
            label, len(winner_votes), total, final_confidence,
        )

        return ClassificationResult(
            label=label,
            confidence=min(final_confidence, 1.0),
            scores=scores,
        )

    def reset(self):
        self.votes.clear()
