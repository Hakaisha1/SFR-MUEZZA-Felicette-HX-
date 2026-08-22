"""
vision_listener.py — Listener Data Vision (Placeholder)

Menerima data output dari proses Vision sesuai kontrak
data_contract.json → kontrak_vision_ke_integration.

Saat ini menggunakan data placeholder karena belum ada PR dari Vision.
Interface-nya sudah mengikuti kontrak sehingga tinggal di-swap
saat transport sesungguhnya (socket/multiprocessing.Queue) tersedia.
"""

import time
import logging

logger = logging.getLogger(__name__)

# Timeout: data vision dianggap stale jika lebih tua dari ini
STALE_THRESHOLD_MS = 500


def _timestamp_ms() -> int:
    """Dapatkan Unix timestamp dalam milidetik."""
    return int(time.time() * 1000)


def _data_tidak_ada(timestamp_ms: int = None) -> dict:
    """Buat data vision 'tidak_ada' (default/placeholder)."""
    return {
        'label': 'tidak_ada',
        'confidence': 0.0,
        'posisi_x_px': 0,
        'posisi_y_px': 0,
        'jarak_estimasi_cm': 0.0,
        'timestamp_ms': timestamp_ms or _timestamp_ms(),
    }


class VisionListener:
    """
    Listener untuk data output dari proses Vision.

    Saat ini berfungsi sebagai placeholder yang mengembalikan data dummy.
    Nanti saat Vision submit PR, class ini akan diganti implementasinya
    untuk membaca dari socket lokal atau multiprocessing.Queue.
    """

    def __init__(self):
        self._data_terbaru = _data_tidak_ada()
        self._override_data = None

    def ambil_data_terbaru(self) -> dict:
        """
        Ambil data vision terbaru.

        Returns:
            Dict sesuai format kontrak_vision_ke_integration:
              {
                'label': 'dummy' | 'riil' | 'tidak_ada',
                'confidence': float,
                'posisi_x_px': int,
                'posisi_y_px': int,
                'jarak_estimasi_cm': float,
                'timestamp_ms': int
              }
        """
        if self._override_data is not None:
            data = self._override_data.copy()
            # Update timestamp supaya tidak stale
            data['timestamp_ms'] = _timestamp_ms()
            return data

        # Default: tidak ada target terdeteksi
        return _data_tidak_ada()

    def set_override(self, data: dict):
        """
        Override data yang dikembalikan (untuk testing / simulasi).

        Args:
            data: Dict partial atau penuh. Field yang tidak disediakan
                  akan menggunakan default 'tidak_ada'.
        """
        base = _data_tidak_ada()
        base.update(data)
        self._override_data = base
        logger.info("Vision override set: label=%s, confidence=%.2f",
                     base['label'], base['confidence'])

    def clear_override(self):
        """Hapus override, kembali ke data default."""
        self._override_data = None
        logger.info("Vision override cleared")

    def is_stale(self, data: dict) -> bool:
        """
        Cek apakah data vision sudah kedaluwarsa (stale).

        Args:
            data: Dict data vision dengan field 'timestamp_ms'.

        Returns:
            True jika data lebih tua dari STALE_THRESHOLD_MS.
        """
        if 'timestamp_ms' not in data:
            return True
        age_ms = _timestamp_ms() - data['timestamp_ms']
        return age_ms > STALE_THRESHOLD_MS
