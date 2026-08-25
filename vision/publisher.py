"""
publisher.py — Publikasi output Vision ke Integration.

Dua mode:
  1. "stdout"  — Print JSON ke terminal (default, untuk testing mandiri)
  2. "socket"  — Kirim via TCP socket ke Integration (localhost)

Saat ini Integration belum menyiapkan server socket, jadi default ke stdout.
Ketika Integration sudah siap, ubah PUBLISHER_MODE di config.py ke "socket".

Format output sesuai data_contract.json:
{
    "label": "riil",
    "confidence": 0.91,
    "posisi_x_px": 320,
    "posisi_y_px": 240,
    "jarak_estimasi_cm": 45.0,
    "timestamp_ms": 1724165031000
}
"""

import json
import socket
import time

from config import (
    PUBLISHER_MODE,
    PUBLISHER_HOST,
    PUBLISHER_PORT,
    PUBLISHER_RECONNECT_DELAY_S,
    PUBLISHER_RECONNECT_MAX_DELAY_S,
    PUBLISHER_RECONNECT_MAX_ATTEMPTS,
)
from utils import setup_logger, validate_output

logger = setup_logger(__name__)


class Publisher:
    """
    Publisher output Vision ke Integration.

    Mengirim dict JSON sesuai kontrak data.
    Mode ditentukan saat konstruksi (stdout atau socket).
    """

    def __init__(self, mode: str = None, host: str = None, port: int = None):
        """
        Inisialisasi publisher.

        Args:
            mode: "stdout" atau "socket". Default dari config.
            host: Host untuk mode socket. Default dari config.
            port: Port untuk mode socket. Default dari config.
        """
        self.mode = mode or PUBLISHER_MODE
        self.host = host or PUBLISHER_HOST
        self.port = port or PUBLISHER_PORT

        self._socket = None
        self._connected = False
        self._reconnect_delay = PUBLISHER_RECONNECT_DELAY_S
        self._publish_count = 0

        logger.info("Publisher diinisialisasi (mode: %s)", self.mode)

        if self.mode == "socket":
            self.connect()

    def connect(self):
        """
        Membuka koneksi TCP ke Integration server.
        Hanya relevan untuk mode "socket".
        """
        if self.mode != "socket":
            return

        attempts = 0
        delay = PUBLISHER_RECONNECT_DELAY_S

        while True:
            try:
                self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self._socket.connect((self.host, self.port))
                self._connected = True
                logger.info("Terhubung ke Integration: %s:%d", self.host, self.port)
                return

            except (ConnectionRefusedError, OSError) as e:
                attempts += 1
                self._connected = False

                if (
                    PUBLISHER_RECONNECT_MAX_ATTEMPTS > 0
                    and attempts >= PUBLISHER_RECONNECT_MAX_ATTEMPTS
                ):
                    logger.error(
                        "Gagal terhubung setelah %d percobaan. Beralih ke stdout.",
                        attempts,
                    )
                    self.mode = "stdout"
                    return

                logger.warning(
                    "Gagal terhubung ke %s:%d (%s). Retry dalam %.1fs...",
                    self.host, self.port, e, delay,
                )
                time.sleep(delay)

                # Exponential backoff
                delay = min(delay * 2, PUBLISHER_RECONNECT_MAX_DELAY_S)

    def publish(self, data: dict):
        """
        Mengirim data output Vision.

        Args:
            data: Dictionary output sesuai kontrak data.
                  Akan divalidasi sebelum dikirim.
        """
        # Validasi format
        if not validate_output(data):
            logger.error("Output tidak valid, tidak dikirim: %s", data)
            return

        json_str = json.dumps(data, separators=(",", ":"))

        if self.mode == "stdout":
            self._publish_stdout(json_str)
        elif self.mode == "socket":
            self._publish_socket(json_str)

        self._publish_count += 1

    def _publish_stdout(self, json_str: str):
        """Kirim ke stdout (print)."""
        print(json_str, flush=True)

    def _publish_socket(self, json_str: str):
        """Kirim via TCP socket dengan newline delimiter."""
        if not self._connected:
            self.connect()
            if not self._connected:
                # Fallback ke stdout jika masih gagal
                self._publish_stdout(json_str)
                return

        try:
            message = (json_str + "\n").encode("utf-8")
            self._socket.sendall(message)
        except (BrokenPipeError, ConnectionResetError, OSError) as e:
            logger.warning("Koneksi terputus: %s. Reconnecting...", e)
            self._connected = False
            self.connect()
            # Retry sekali setelah reconnect
            if self._connected:
                try:
                    message = (json_str + "\n").encode("utf-8")
                    self._socket.sendall(message)
                except (BrokenPipeError, ConnectionResetError, OSError):
                    logger.error("Gagal kirim setelah reconnect. Fallback ke stdout.")
                    self._publish_stdout(json_str)

    def close(self):
        """Menutup koneksi socket."""
        if self._socket:
            try:
                self._socket.close()
            except OSError:
                pass
            self._socket = None
            self._connected = False

        logger.info(
            "Publisher ditutup. Total pesan terkirim: %d",
            self._publish_count,
        )

    @property
    def is_connected(self) -> bool:
        """Status koneksi (selalu True untuk mode stdout)."""
        if self.mode == "stdout":
            return True
        return self._connected

    def __del__(self):
        self.close()
