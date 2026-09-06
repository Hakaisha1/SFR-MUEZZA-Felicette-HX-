"""
test_classifier.py — Unit test untuk modul classifier.py (PixyCam mode).

Menguji:
  - Classifier pass-through dari PixyCam signature
  - ClassificationVoter: voting mayoritas multi-frame
"""

import unittest
import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from classifier import Classifier, ClassificationResult, ClassificationVoter


class TestClassifier(unittest.TestCase):
    """Test Classifier PixyCam mode."""

    def setUp(self):
        self.cls = Classifier()

    def test_classify_riil(self):
        """Label 'riil' di-pass-through dengan benar."""
        result = self.cls.classify("riil", 0.85)
        self.assertEqual(result.label, "riil")
        self.assertAlmostEqual(result.confidence, 0.85, places=2)

    def test_classify_dummy(self):
        """Label 'dummy' di-pass-through dengan benar."""
        result = self.cls.classify("dummy", 0.78)
        self.assertEqual(result.label, "dummy")
        self.assertAlmostEqual(result.confidence, 0.78, places=2)

    def test_classify_returns_scores(self):
        """Hasil klasifikasi memiliki scores dict."""
        result = self.cls.classify("riil", 0.90)
        self.assertIn("source", result.scores)
        self.assertEqual(result.scores["source"], "pixycam_signature")

    def test_confidence_in_valid_range(self):
        """Confidence tetap dalam rentang 0.0–1.0."""
        result = self.cls.classify("riil", 0.999)
        self.assertGreaterEqual(result.confidence, 0.0)
        self.assertLessEqual(result.confidence, 1.0)


class TestClassificationVoter(unittest.TestCase):
    """Test voting mayoritas multi-frame."""

    def test_voter_not_ready_with_fewer_frames(self):
        """Voter belum siap jika frame kurang dari required."""
        voter = ClassificationVoter(required_frames=5)
        voter.add_vote(ClassificationResult("riil", 0.9, {}))
        self.assertFalse(voter.is_ready())

    def test_voter_ready_with_enough_frames(self):
        """Voter siap setelah cukup frame."""
        voter = ClassificationVoter(required_frames=3)
        for _ in range(3):
            voter.add_vote(ClassificationResult("riil", 0.9, {}))
        self.assertTrue(voter.is_ready())

    def test_voter_majority_dummy(self):
        """Mayoritas dummy → verdict dummy."""
        voter = ClassificationVoter(required_frames=5)
        for _ in range(3):
            voter.add_vote(ClassificationResult("dummy", 0.8, {}))
        for _ in range(2):
            voter.add_vote(ClassificationResult("riil", 0.9, {}))
        verdict = voter.get_verdict()
        self.assertEqual(verdict.label, "dummy")

    def test_voter_majority_riil(self):
        """Mayoritas riil → verdict riil."""
        voter = ClassificationVoter(required_frames=5)
        for _ in range(4):
            voter.add_vote(ClassificationResult("riil", 0.85, {}))
        voter.add_vote(ClassificationResult("dummy", 0.7, {}))
        verdict = voter.get_verdict()
        self.assertEqual(verdict.label, "riil")

    def test_voter_tie_resolves_by_confidence(self):
        """Seri → diputuskan berdasarkan rata-rata confidence."""
        voter = ClassificationVoter(required_frames=4)
        voter.add_vote(ClassificationResult("riil", 0.95, {}))
        voter.add_vote(ClassificationResult("riil", 0.90, {}))
        voter.add_vote(ClassificationResult("dummy", 0.60, {}))
        voter.add_vote(ClassificationResult("dummy", 0.65, {}))
        verdict = voter.get_verdict()
        # avg riil = 0.925, avg dummy = 0.625 → riil menang
        self.assertEqual(verdict.label, "riil")

    def test_voter_reset(self):
        """Reset menghapus semua votes."""
        voter = ClassificationVoter(required_frames=3)
        for _ in range(3):
            voter.add_vote(ClassificationResult("riil", 0.9, {}))
        voter.reset()
        self.assertFalse(voter.is_ready())

    def test_voter_empty_verdict(self):
        """Voter kosong → verdict 'tidak_ada'."""
        voter = ClassificationVoter(required_frames=5)
        verdict = voter.get_verdict()
        self.assertEqual(verdict.label, "tidak_ada")
        self.assertEqual(verdict.confidence, 0.0)


if __name__ == "__main__":
    unittest.main()
