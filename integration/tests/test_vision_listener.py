"""
test_vision_listener.py — Unit test untuk VisionListener TCP Socket Server
"""

import sys
import os
import time
import json
import socket
import threading
import pytest

# Pastikan path import benar
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from vision_listener import VisionListener, _data_tidak_ada


# Port acak untuk menghindari konflik jika port 5555 sedang dipakai
TEST_PORT = 15555


def mock_vision_client(port, messages, delay_between_msgs=0.05):
    """
    Klien mock untuk mengirim pesan JSON ke VisionListener.
    Berjalan di thread terpisah.
    """
    def _run():
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # Beri waktu sebentar agar server sempat start
            time.sleep(0.1)
            s.connect(("localhost", port))
            for msg in messages:
                json_str = json.dumps(msg) + "\n"
                s.sendall(json_str.encode("utf-8"))
                time.sleep(delay_between_msgs)
            s.close()
        except OSError as e:
            print(f"Mock client error: {e}")

    t = threading.Thread(target=_run)
    t.start()
    return t


class TestVisionListener:

    def setup_method(self):
        """Siapkan instance VisionListener untuk tiap test."""
        # Gunakan port berbeda untuk test
        self.listener = VisionListener(host="localhost", port=TEST_PORT, use_socket=True)

    def teardown_method(self):
        """Hentikan server setelah test."""
        self.listener.stop()

    def test_fallback_when_no_connection(self):
        """Tanpa koneksi, ambil_data_terbaru mengembalikan fallback."""
        self.listener.start()
        time.sleep(0.1)  # Beri waktu server start

        data = self.listener.ambil_data_terbaru()
        assert data['label'] == 'tidak_ada'
        assert data['confidence'] == 0.0
        assert data['jarak_estimasi_cm'] == 0.0
        assert 'timestamp_ms' in data
        assert not self.listener.is_connected

    def test_receive_valid_json(self):
        """Menerima data JSON valid dari klien."""
        self.listener.start()

        msg = {
            "label": "riil",
            "confidence": 0.95,
            "posisi_x_px": 300,
            "posisi_y_px": 240,
            "jarak_estimasi_cm": 45.0,
            "timestamp_ms": int(time.time() * 1000)
        }

        # Jalankan klien
        client_thread = mock_vision_client(TEST_PORT, [msg])
        client_thread.join(timeout=1.0)

        # Beri waktu listener memproses buffer
        time.sleep(0.1)

        data = self.listener.ambil_data_terbaru()
        assert data['label'] == 'riil'
        assert data['confidence'] == 0.95
        assert data['jarak_estimasi_cm'] == 45.0
        assert data['posisi_x_px'] == 300
        assert data['posisi_y_px'] == 240
        
    def test_ignore_invalid_json(self):
        """Mengabaikan data JSON yang formatnya tidak sesuai kontrak."""
        self.listener.start()

        msg1 = {
            "label": "riil",
            "confidence": 0.90,
            "posisi_x_px": 100,
            "posisi_y_px": 100,
            "jarak_estimasi_cm": 50.0,
            "timestamp_ms": int(time.time() * 1000)
        }
        # msg2 kurang field 'jarak_estimasi_cm'
        msg2 = {
            "label": "dummy",
            "confidence": 0.80,
            "posisi_x_px": 50,
            "posisi_y_px": 50,
            "timestamp_ms": int(time.time() * 1000)
        }

        client_thread = mock_vision_client(TEST_PORT, [msg1, msg2])
        client_thread.join(timeout=1.0)
        time.sleep(0.1)

        # Listener harusnya menolak msg2 dan tetap menyimpan msg1
        data = self.listener.ambil_data_terbaru()
        assert data['label'] == 'riil'
        assert data['confidence'] == 0.90

    def test_override_data(self):
        """Fungsi override data simulasi bekerja tanpa terpengaruh socket."""
        self.listener.start()

        self.listener.set_override({'label': 'riil', 'confidence': 0.88})

        data = self.listener.ambil_data_terbaru()
        assert data['label'] == 'riil'
        assert data['confidence'] == 0.88
        assert data['jarak_estimasi_cm'] == 0.0  # sisa field default

        # Walau ada client mengirim pesan, override tetap jalan
        msg = {
            "label": "dummy",
            "confidence": 0.95,
            "posisi_x_px": 300,
            "posisi_y_px": 240,
            "jarak_estimasi_cm": 45.0,
            "timestamp_ms": int(time.time() * 1000)
        }
        client_thread = mock_vision_client(TEST_PORT, [msg])
        client_thread.join(timeout=1.0)
        time.sleep(0.1)

        # Tetap 'riil' karena di-override
        data = self.listener.ambil_data_terbaru()
        assert data['label'] == 'riil'
        assert data['confidence'] == 0.88

        # Clear override
        self.listener.clear_override()
        data = self.listener.ambil_data_terbaru()
        # Sekarang harusnya balik ke pesan yang masuk (dummy)
        assert data['label'] == 'dummy'

    def test_is_stale(self):
        """Deteksi data stale jika melewati threshold waktu."""
        now = int(time.time() * 1000)
        
        # 100ms yang lalu (tidak stale, threshold = 500ms)
        data_baru = _data_tidak_ada(now - 100)
        assert not self.listener.is_stale(data_baru)

        # 600ms yang lalu (stale)
        data_lama = _data_tidak_ada(now - 600)
        assert self.listener.is_stale(data_lama)
