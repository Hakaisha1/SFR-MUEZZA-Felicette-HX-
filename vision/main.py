"""
main.py — Entry point utama pipeline Vision.

Mengorkestrasi seluruh pipeline:
  Frame kamera → Deteksi (HSV) → Klasifikasi → Estimasi jarak → Publish JSON

Cara pakai:
  # Default: buka kamera + tampilkan display
  python main.py

  # Tanpa display (headless, cocok untuk RPi5 tanpa monitor)
  python main.py --no-display

  # Pakai gambar statis (untuk testing offline)
  python main.py --image data/dummy/sample_01.jpg

  # Ganti index kamera
  python main.py --camera 1

  # Mode socket ke Integration
  python main.py --publisher socket

Tekan 'q' untuk keluar (jika display aktif), atau Ctrl+C.
"""

import argparse
import sys
import time

import cv2

from config import (
    CAMERA_INDEX,
    CAMERA_WIDTH,
    CAMERA_HEIGHT,
    CAMERA_FPS,
    CONFIDENCE_THRESHOLD,
    OUTPUT_FPS,
    SHOW_DISPLAY,
)
from detector import Detector
from classifier import Classifier
from estimator import DistanceEstimator
from publisher import Publisher
from utils import setup_logger, build_output, draw_overlay, draw_multiple_overlays

logger = setup_logger("vision.main")


def parse_args():
    """Parse argumen command-line."""
    parser = argparse.ArgumentParser(
        description="Vision Pipeline — Robot SAR Hexapod MUEZZA",
    )
    parser.add_argument(
        "--camera", type=int, default=CAMERA_INDEX,
        help=f"Index kamera (default: {CAMERA_INDEX})",
    )
    parser.add_argument(
        "--image", type=str, default=None,
        help="Path ke gambar statis (untuk testing offline, skip kamera)",
    )
    parser.add_argument(
        "--no-display", action="store_true",
        help="Jalankan tanpa tampilan OpenCV (headless)",
    )
    parser.add_argument(
        "--publisher", type=str, choices=["stdout", "socket"], default="stdout",
        help="Mode publisher (default: stdout)",
    )
    return parser.parse_args()


def process_frame_objects(frame, detector: Detector, classifier: Classifier, estimator: DistanceEstimator):
    """
    Mendeteksi seluruh objek dalam frame, mengklasifikasi setiap objek secara independen,
    dan memilih target utama (prioritas: Korban Asli / 'riil').

    Returns:
        tuple (primary_output_dict, all_objects_info_list, primary_index)
    """
    detections = detector.detect(frame)

    if not detections:
        return build_output(), [], -1

    objects_info = []
    for det in detections:
        cls_result = classifier.classify(frame, det.bbox, det.contour)
        distance = estimator.estimate(det.width)
        objects_info.append({
            "bbox": det.bbox,
            "label": cls_result.label,
            "confidence": cls_result.confidence,
            "dist": distance,
            "center_x": det.center_x,
            "center_y": det.center_y,
            "area": det.area,
            "width": det.width,
            "scores": cls_result.scores,
        })

    # Strategi Seleksi Target Utama:
    # 1. Prioritaskan Korban Asli ('riil') jika terdeteksi di arena
    riil_indices = [i for i, obj in enumerate(objects_info) if obj["label"] == "riil"]

    if riil_indices:
        # Jika ada korban asli, ambil yang terbesar / terdekat
        primary_idx = max(riil_indices, key=lambda i: objects_info[i]["area"])
    else:
        # Jika tidak ada korban riil (hanya dummy), ambil dummy terbesar
        primary_idx = max(range(len(objects_info)), key=lambda i: objects_info[i]["area"])

    primary_obj = objects_info[primary_idx]
    primary_output = build_output(
        label=primary_obj["label"],
        confidence=primary_obj["confidence"],
        posisi_x_px=primary_obj["center_x"],
        posisi_y_px=primary_obj["center_y"],
        jarak_estimasi_cm=primary_obj["dist"],
    )

    return primary_output, objects_info, primary_idx


def process_single_image(image_path: str, detector, classifier, estimator, show_display: bool):
    """
    Proses satu gambar statis untuk testing offline (mendukung banyak objek sekaligus).
    """
    frame = cv2.imread(image_path)
    if frame is None:
        logger.error("Gagal membaca gambar: %s", image_path)
        sys.exit(1)

    logger.info("Memproses gambar: %s", image_path)

    # Deteksi dan klasifikasi semua objek
    output, objects_info, primary_idx = process_frame_objects(
        frame, detector, classifier, estimator,
    )

    if objects_info:
        logger.info(
            "Ditemukan %d objek: %s",
            len(objects_info),
            [(o['label'], f"{o['dist']:.1f}cm", f"conf={o['confidence']:.2f}") for o in objects_info],
        )
        logger.info("Target utama terpilih: %s", output)
    else:
        logger.info("Tidak ada target terdeteksi.")

    # Render overlay semua objek pada frame
    draw_multiple_overlays(frame, objects_info, primary_index=primary_idx)

    # Print output JSON kontrak data
    import json
    print(json.dumps(output, separators=(",", ":")))

    # Simpan hasil visualisasi selalu ke data/output_result.jpg agar bisa diinspeksi
    output_debug_path = "data/output_result.jpg"
    cv2.imwrite(output_debug_path, frame)
    logger.info("Hasil visualisasi semua objek disimpan ke: %s", output_debug_path)

    if show_display:
        try:
            cv2.imshow("Vision - Static Image", frame)
            logger.info("Tekan sembarang tombol untuk keluar...")
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        except cv2.error as e:
            logger.warning("cv2.imshow tidak didukung di environment ini (%s).", e)


def run_camera_loop(
    camera_index: int,
    detector: Detector,
    classifier: Classifier,
    estimator: DistanceEstimator,
    publisher: Publisher,
    show_display: bool,
):
    """
    Loop utama: baca kamera → deteksi multi-objek → klasifikasi → publish.
    """
    cap = cv2.VideoCapture(camera_index)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT)
    cap.set(cv2.CAP_PROP_FPS, CAMERA_FPS)

    if not cap.isOpened():
        logger.error("Gagal membuka kamera index %d", camera_index)
        sys.exit(1)

    actual_w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    actual_h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    actual_fps = cap.get(cv2.CAP_PROP_FPS)

    logger.info(
        "Kamera dibuka: index=%d, resolusi=%dx%d, FPS=%.0f",
        camera_index, actual_w, actual_h, actual_fps,
    )
    logger.info("Output FPS: %d Hz (sesuai data contract)", OUTPUT_FPS)
    logger.info("Tekan 'q' untuk keluar (jika display aktif)")

    output_interval = 1.0 / OUTPUT_FPS
    frame_count = 0
    fps_start_time = time.time()
    last_output_time = 0.0

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                logger.warning("Gagal membaca frame, skip...")
                continue

            frame_count += 1
            current_time = time.time()

            if (current_time - last_output_time) < output_interval:
                continue

            last_output_time = current_time

            # Deteksi & klasifikasi multi-objek
            output, objects_info, primary_idx = process_frame_objects(
                frame, detector, classifier, estimator,
            )

            # Publish target utama
            publisher.publish(output)

            # Display
            if show_display:
                draw_multiple_overlays(frame, objects_info, primary_index=primary_idx)

                elapsed = current_time - fps_start_time
                if elapsed > 0:
                    actual_output_fps = frame_count / elapsed
                    cv2.putText(
                        frame,
                        f"FPS: {actual_output_fps:.1f}",
                        (CAMERA_WIDTH - 90, CAMERA_HEIGHT - 10),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.45,
                        (0, 255, 0),
                        1,
                        cv2.LINE_AA,
                    )

                try:
                    cv2.imshow("Vision - Robot SAR Hexapod MUEZZA", frame)

                    key = cv2.waitKey(1) & 0xFF
                    if key == ord("q"):
                        logger.info("Keluar (tombol 'q' ditekan)")
                        break
                except cv2.error as e:
                    logger.warning("cv2.imshow tidak didukung di environment ini (%s). Menonaktifkan display GUI...", e)
                    show_display = False

    except KeyboardInterrupt:
        logger.info("Keluar (Ctrl+C)")

    finally:
        cap.release()
        if show_display:
            cv2.destroyAllWindows()
        publisher.close()

        total_time = time.time() - fps_start_time
        if total_time > 0:
            logger.info(
                "Selesai. Total frame: %d, durasi: %.1fs, avg FPS: %.1f",
                frame_count, total_time, frame_count / total_time,
            )


def main():
    """Entry point utama."""
    args = parse_args()

    logger.info("=" * 60)
    logger.info("  Vision Pipeline — Robot SAR Hexapod MUEZZA")
    logger.info("  Mode: HSV Color Masking")
    logger.info("=" * 60)

    # Inisialisasi komponen
    detector = Detector()
    classifier = Classifier()
    estimator = DistanceEstimator()

    show_display = SHOW_DISPLAY and not args.no_display

    if args.image:
        # Mode gambar statis
        process_single_image(
            args.image, detector, classifier, estimator, show_display,
        )
    else:
        # Mode kamera live
        publisher = Publisher(mode=args.publisher)
        run_camera_loop(
            camera_index=args.camera,
            detector=detector,
            classifier=classifier,
            estimator=estimator,
            publisher=publisher,
            show_display=show_display,
        )


if __name__ == "__main__":
    main()
