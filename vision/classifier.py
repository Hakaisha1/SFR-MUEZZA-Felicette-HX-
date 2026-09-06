"""
classifier.py — Klasifikasi target: Korban Asli vs Dummy (PixyCam Mode).

Pada mode PixyCam, klasifikasi sudah otomatis dilakukan oleh hardware
kamera berdasarkan Signature yang di-training di PixyMon:
  Signature 1 → Oranye → "riil"
  Signature 2 → Abu-abu → "dummy"

Modul ini tetap dipertahankan untuk:
  1. Menjaga interface yang konsisten dengan flow di main.py
  2. Menjalankan ClassificationVoter (voting mayoritas multi-frame)
     saat robot berada di state CLASSIFYING
"""

import logging
from dataclasses import dataclass

from config import CLASSIFICATION_VOTE_FRAMES

logger = logging.getLogger(__name__)


@dataclass
class ClassificationResult:
    """Hasil klasifikasi satu objek."""

    label: str           # "riil" (Oranye) atau "dummy" (Abu-abu)
    confidence: float    # Keyakinan klasifikasi (0.0–1.0)
    scores: dict         # Detail skor / metadata


class Classifier:
    """
    Classifier untuk mode PixyCam.

    Di mode PixyCam, classify() hanya meneruskan label yang sudah
    ditentukan oleh detector (berdasarkan Signature). Tidak ada
    komputasi tambahan yang diperlukan.
    """

    def __init__(self):
        logger.info("Classifier diinisialisasi (mode: PixyCam Signature-based)")

    def classify(self, label: str, confidence: float) -> ClassificationResult:
        """
        Klasifikasi berdasarkan label dari PixyCam detector.

        Args:
            label: Label dari detector ("riil" atau "dummy").
            confidence: Confidence dari detector.

        Returns:
            ClassificationResult.
        """
        scores = {
            "source": "pixycam_signature",
            "original_label": label,
        }

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

    def __init__(self, required_frames: int = None):
        self.required_frames = required_frames or CLASSIFICATION_VOTE_FRAMES
        self.votes = []
        logger.info("ClassificationVoter: butuh %d frame", self.required_frames)

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
