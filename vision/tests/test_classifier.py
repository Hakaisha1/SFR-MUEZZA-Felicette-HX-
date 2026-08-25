"""
test_classifier.py — Unit test untuk modul classifier.

Menguji klasifikasi warna Korban Asli (Oranye) vs Dummy (Abu-abu).

Jalankan: python -m pytest tests/test_classifier.py -v
"""

import sys
import os

import cv2
import numpy as np
import pytest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from classifier import Classifier, ClassificationResult, ClassificationVoter


def create_orange_object_frame(size=200):
    """Buat frame dengan objek warna oranye (Korban Asli)."""
    frame = np.zeros((size, size, 3), dtype=np.uint8)
    orange = (0, 140, 255)
    cv2.rectangle(frame, (50, 50), (150, 150), orange, -1)
    return frame


def create_grey_object_frame(size=200):
    """Buat frame dengan objek warna abu-abu (Dummy)."""
    frame = np.zeros((size, size, 3), dtype=np.uint8)
    grey = (128, 128, 128)
    cv2.rectangle(frame, (50, 50), (150, 150), grey, -1)
    return frame


class TestClassifier:
    """Test suite untuk Classifier warna Oranye vs Abu-abu."""

    def setup_method(self):
        self.classifier = Classifier()

    def test_classify_orange_as_riil(self):
        """Objek warna oranye -> label 'riil'."""
        frame = create_orange_object_frame()
        result = self.classifier.classify(frame, (50, 50, 100, 100))

        assert isinstance(result, ClassificationResult)
        assert result.label == "riil"
        assert result.confidence >= 0.70

    def test_classify_grey_as_dummy(self):
        """Objek warna abu-abu -> label 'dummy'."""
        frame = create_grey_object_frame()
        result = self.classifier.classify(frame, (50, 50, 100, 100))

        assert isinstance(result, ClassificationResult)
        assert result.label == "dummy"
        assert result.confidence >= 0.70

    def test_classify_returns_scores(self):
        """Hasil klasifikasi menyertakan skor warna."""
        frame = create_orange_object_frame()
        result = self.classifier.classify(frame, (50, 50, 100, 100))

        assert "orange_ratio" in result.scores
        assert "grey_ratio" in result.scores
        assert "mean_saturation" in result.scores

    def test_confidence_in_valid_range(self):
        """Confidence harus antara 0.0 dan 1.0."""
        frame = create_orange_object_frame()
        result = self.classifier.classify(frame, (50, 50, 100, 100))
        assert 0.0 <= result.confidence <= 1.0

    def test_invalid_bbox_returns_tidak_ada(self):
        """Bounding box tidak valid (w=0) -> label 'tidak_ada'."""
        frame = create_orange_object_frame()
        result = self.classifier.classify(frame, (100, 100, 0, 0))
        assert result.label == "tidak_ada"

    def test_bbox_out_of_bounds_clamped(self):
        """Bounding box di luar frame -> di-clamp, tidak error."""
        frame = create_orange_object_frame()
        result = self.classifier.classify(frame, (-50, -50, 300, 300))
        assert result.label in ["dummy", "riil", "tidak_ada"]


class TestClassificationVoter:
    """Test suite untuk ClassificationVoter (voting multi-frame)."""

    def test_voter_not_ready_with_fewer_frames(self):
        voter = ClassificationVoter(required_frames=5)
        voter.add_vote(ClassificationResult("dummy", 0.8, {}))
        voter.add_vote(ClassificationResult("dummy", 0.7, {}))
        assert not voter.is_ready()

    def test_voter_ready_with_enough_frames(self):
        voter = ClassificationVoter(required_frames=3)
        for _ in range(3):
            voter.add_vote(ClassificationResult("dummy", 0.8, {}))
        assert voter.is_ready()

    def test_voter_majority_dummy(self):
        voter = ClassificationVoter(required_frames=5)
        voter.add_vote(ClassificationResult("dummy", 0.9, {}))
        voter.add_vote(ClassificationResult("dummy", 0.8, {}))
        voter.add_vote(ClassificationResult("dummy", 0.7, {}))
        voter.add_vote(ClassificationResult("riil", 0.6, {}))
        voter.add_vote(ClassificationResult("riil", 0.5, {}))

        verdict = voter.get_verdict()
        assert verdict.label == "dummy"

    def test_voter_majority_riil(self):
        voter = ClassificationVoter(required_frames=5)
        voter.add_vote(ClassificationResult("riil", 0.9, {}))
        voter.add_vote(ClassificationResult("riil", 0.8, {}))
        voter.add_vote(ClassificationResult("riil", 0.7, {}))
        voter.add_vote(ClassificationResult("dummy", 0.6, {}))
        voter.add_vote(ClassificationResult("dummy", 0.5, {}))

        verdict = voter.get_verdict()
        assert verdict.label == "riil"

    def test_voter_tie_resolves_by_confidence(self):
        voter = ClassificationVoter(required_frames=4)
        voter.add_vote(ClassificationResult("dummy", 0.9, {}))
        voter.add_vote(ClassificationResult("dummy", 0.8, {}))
        voter.add_vote(ClassificationResult("riil", 0.3, {}))
        voter.add_vote(ClassificationResult("riil", 0.2, {}))

        verdict = voter.get_verdict()
        assert verdict.label == "dummy"

    def test_voter_reset(self):
        voter = ClassificationVoter(required_frames=3)
        voter.add_vote(ClassificationResult("dummy", 0.8, {}))
        voter.reset()
        assert not voter.is_ready()

    def test_voter_empty_verdict(self):
        voter = ClassificationVoter(required_frames=3)
        verdict = voter.get_verdict()
        assert verdict.label == "tidak_ada"


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
