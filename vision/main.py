"""
main.py — Entry point utama pipeline Vision (PixyCam2 USB Mode).

Mengorkestrasi seluruh pipeline:
  PixyCam2 Blocks → Deteksi → Klasifikasi → Estimasi jarak → Publish JSON

Cara pakai:
  # Default: baca PixyCam2 USB, publish ke socket Integration
  python main.py

  # Mode stdout (untuk testing mandiri)
  python main.py --publisher stdout

  # Mode dummy tanpa hardware PixyCam
  python main.py --dummy

Tekan Ctrl+C untuk keluar.
"""

import argparse
import sys
import time
import logging

from config import (
    CONFIDENCE_THRESHOLD,
    OUTPUT_FPS,
    PIXY_SIG_RIIL,
)
from detector import Detector
from classifier import Classifier
from estimator import DistanceEstimator
from publisher import Publisher
from pixy_usb import PixyUSB, DummyPixy, PixyBlock
from utils import setup_logger, build_output

logger = setup_logger("vision.main")


def parse_args():
    """Parse argumen command-line."""
    parser = argparse.ArgumentParser(
        description="Vision Pipeline — Robot SAR Hexapod MUEZZA (PixyCam2 Mode)",
    )
    parser.add_argument(
        "--publisher", type=str, choices=["stdout", "socket"], default=None,
        help="Mode publisher (default: dari config.py)",
    )
    parser.add_argument(
        "--dummy", action="store_true",
        help="Jalankan tanpa hardware PixyCam (mode simulasi)",
    )
    # Legacy compatibility: ignore --no-display, --camera, --image
    parser.add_argument("--no-display", action="store_true", help=argparse.SUPPRESS)
    parser.add_argument("--headless", action="store_true", help=argparse.SUPPRESS)
    parser.add_argument("--camera", type=int, default=0, help=argparse.SUPPRESS)
    parser.add_argument("--image", type=str, default=None, help=argparse.SUPPRESS)
    return parser.parse_args()


def select_primary_target(detections: list) -> int:
    """
    Pilih target utama dari daftar deteksi.

    Strategi:
      1. Prioritaskan Korban Asli ('riil') jika ada
      2. Dari yang riil, ambil yang area terbesar (terdekat)
      3. Jika tidak ada riil, ambil dummy terbesar

    Args:
        detections: List of DetectionResult dari detector.

    Returns:
        Index target utama, atau -1 jika kosong.
    """
    if not detections:
        return -1

    riil_indices = [i for i, d in enumerate(detections) if d.label == "riil"]

    if riil_indices:
        return riil_indices[0]  # Sudah sorted by area desc

    return 0  # Ambil terbesar (sudah sorted)


def run_pixy_loop(
    pixy_interface,
    detector: Detector,
    classifier: Classifier,
    estimator: DistanceEstimator,
    publisher: Publisher,
):
    """
    Loop utama: baca PixyCam2 → deteksi → klasifikasi → publish.
    """
    output_interval = 1.0 / OUTPUT_FPS
    cycle_count = 0
    start_time = time.time()
    last_output_time = 0.0

    logger.info("Output FPS: %d Hz (sesuai data contract)", OUTPUT_FPS)
    logger.info("Loop dimulai... (Ctrl+C untuk berhenti)")

    try:
        while True:
            current_time = time.time()

            # Rate limiting sesuai OUTPUT_FPS
            if (current_time - last_output_time) < output_interval:
                time.sleep(0.001)  # Yield CPU sebentar
                continue

            last_output_time = current_time
            cycle_count += 1

            # 1. Deteksi semua blocks dari PixyCam
            detections = detector.detect()

            if not detections:
                # Tidak ada objek terdeteksi
                output = build_output()
                publisher.publish(output)

                if cycle_count % 50 == 0:
                    logger.info("Cycle #%d | Tidak ada target terdeteksi", cycle_count)
                continue

            # 2. Pilih target utama
            primary_idx = select_primary_target(detections)
            primary = detections[primary_idx]

            # 3. Klasifikasi (pass-through dari signature)
            cls_result = classifier.classify(primary.label, primary.confidence)

            # 4. Estimasi jarak
            distance = estimator.estimate(primary.width)

            # 5. Build dan publish output
            output = build_output(
                label=cls_result.label,
                confidence=cls_result.confidence,
                posisi_x_px=primary.center_x,
                posisi_y_px=primary.center_y,
                jarak_estimasi_cm=distance,
            )
            publisher.publish(output)

            # Log periodik (setiap 50 cycle = 5 detik)
            if cycle_count % 50 == 0:
                logger.info(
                    "Cycle #%d | Deteksi: %d obj | Target: %s (conf=%.2f) | Jarak: %.1f cm",
                    cycle_count,
                    len(detections),
                    cls_result.label,
                    cls_result.confidence,
                    distance,
                )

    except KeyboardInterrupt:
        logger.info("")
        logger.info("Ctrl+C diterima — menghentikan pipeline...")

    finally:
        publisher.close()
        pixy_interface.close()

        total_time = time.time() - start_time
        if total_time > 0:
            logger.info(
                "Selesai. Total cycles: %d, durasi: %.1fs, avg FPS: %.1f",
                cycle_count, total_time, cycle_count / total_time,
            )


def main():
    """Entry point utama."""
    args = parse_args()

    logger.info("=" * 60)
    logger.info("  Vision Pipeline — Robot SAR Hexapod MUEZZA")
    logger.info("  Mode: PixyCam2 USB (Color Connected Components)")
    logger.info("=" * 60)

    # Inisialisasi PixyCam
    if args.dummy:
        logger.info("Mode DUMMY aktif — tanpa hardware PixyCam")
        pixy = DummyPixy()
        pixy.init()
        # Set beberapa block dummy untuk testing
        pixy.set_blocks([
            PixyBlock(sig=PIXY_SIG_RIIL, x_center=160, y_center=100,
                      width=80, height=60, angle=0, tracking_index=0, age=10),
        ])
    else:
        pixy = PixyUSB()
        if not pixy.init():
            logger.error("Gagal menghubungkan ke PixyCam2. Pastikan:")
            logger.error("  1. PixyCam2 terhubung via USB")
            logger.error("  2. libpixyusb2 sudah di-build dan di-install")
            logger.error("  3. Jalankan dengan sudo jika perlu: sudo python main.py")
            logger.error("")
            logger.error("Untuk testing tanpa hardware, gunakan: python main.py --dummy")
            sys.exit(1)

    # Inisialisasi komponen
    detector = Detector(pixy_interface=pixy)
    classifier = Classifier()
    estimator = DistanceEstimator()

    # Publisher
    pub_mode = args.publisher  # None = ambil dari config
    publisher = Publisher(mode=pub_mode)

    # Jalankan loop utama
    run_pixy_loop(pixy, detector, classifier, estimator, publisher)


if __name__ == "__main__":
    main()
