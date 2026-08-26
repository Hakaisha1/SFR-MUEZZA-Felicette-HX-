"""
vision_listener.py — Listener Data Vision via TCP Socket

Menerima data output dari proses Vision sesuai kontrak
data_contract.json → kontrak_vision_ke_integration.

Arsitektur koneksi:
  - VisionListener bertindak sebagai TCP SERVER (listen di port 5555)
  - Vision publisher.py bertindak sebagai TCP CLIENT yang connect ke sini
  - Data dikirim sebagai JSON line-delimited (satu JSON dict per baris)

Thread-safe: data terbaru disimpan dengan threading.Lock
sehingga bisa diakses dari main loop tanpa race condition.
"""

import json
import socket
import threading
import time
import logging

logger = logging.getLogger(__name__)

# ──────────────────────────────────────────────
# Konfigurasi Default
# ──────────────────────────────────────────────
DEFAULT_HOST = "localhost"
DEFAULT_PORT = 5555
STALE_THRESHOLD_MS = 500  # Data vision dianggap stale jika lebih tua dari ini
RECV_BUFFER_SIZE = 4096


def _timestamp_ms() -> int:
    """Dapatkan Unix timestamp dalam milidetik."""
    return int(time.time() * 1000)


def _data_tidak_ada(timestamp_ms: int = None) -> dict:
    """Buat data vision 'tidak_ada' (default/fallback)."""
    return {
        'label': 'tidak_ada',
        'confidence': 0.0,
        'posisi_x_px': 0,
        'posisi_y_px': 0,
        'jarak_estimasi_cm': 0.0,
        'timestamp_ms': timestamp_ms or _timestamp_ms(),
    }


# Field wajib sesuai data_contract.json
_REQUIRED_FIELDS = {'label', 'confidence', 'posisi_x_px', 'posisi_y_px',
                    'jarak_estimasi_cm', 'timestamp_ms'}


def _validate_vision_data(data: dict) -> bool:
    """Validasi bahwa data vision memiliki semua field yang diperlukan."""
    if not isinstance(data, dict):
        return False
    return _REQUIRED_FIELDS.issubset(data.keys())


class VisionListener:
    """
    TCP Socket Server yang menerima data output dari Vision publisher.

    Menjalankan server socket di background thread. Saat Vision publisher
    terkoneksi, listener membaca JSON line-delimited dan menyimpan
    data terbaru secara thread-safe.

    Jika belum ada koneksi atau koneksi terputus, ambil_data_terbaru()
    mengembalikan data 'tidak_ada' (fallback aman — robot tidak crash).
    """

    def __init__(self, host: str = DEFAULT_HOST, port: int = DEFAULT_PORT,
                 use_socket: bool = True):
        """
        Args:
            host: Host untuk bind server socket.
            port: Port untuk bind server socket (harus cocok dengan Vision config).
            use_socket: Jika False, berjalan dalam mode placeholder (tanpa socket).
                        Berguna untuk testing tanpa Vision.
        """
        self._host = host
        self._port = port
        self._use_socket = use_socket

        # Data terbaru dari Vision (thread-safe)
        self._lock = threading.Lock()
        self._data_terbaru = _data_tidak_ada()

        # Override untuk testing/simulasi
        self._override_data = None

        # Socket server state
        self._server_socket = None
        self._client_socket = None
        self._running = False
        self._thread = None
        self._connected = False

    def start(self):
        """
        Mulai server socket di background thread.

        Hanya relevan jika use_socket=True. Jika use_socket=False,
        method ini tidak melakukan apa-apa (mode placeholder).
        """
        if not self._use_socket:
            logger.info("VisionListener: mode placeholder (tanpa socket)")
            return

        self._running = True
        self._thread = threading.Thread(
            target=self._server_loop,
            name="VisionListener",
            daemon=True,
        )
        self._thread.start()
        logger.info("VisionListener: server dimulai di %s:%d (background thread)",
                     self._host, self._port)

    def stop(self):
        """Hentikan server socket dan tutup semua koneksi."""
        self._running = False

        # Tutup client socket jika ada
        if self._client_socket:
            try:
                self._client_socket.close()
            except OSError:
                pass
            self._client_socket = None

        # Tutup server socket
        if self._server_socket:
            try:
                self._server_socket.close()
            except OSError:
                pass
            self._server_socket = None

        self._connected = False
        logger.info("VisionListener: server dihentikan")

    def ambil_data_terbaru(self) -> dict:
        """
        Ambil data vision terbaru.

        Prioritas:
          1. Override data (jika di-set via set_override)
          2. Data dari socket Vision
          3. Fallback 'tidak_ada'

        Returns:
            Dict sesuai format kontrak_vision_ke_integration.
        """
        # Override untuk testing
        if self._override_data is not None:
            data = self._override_data.copy()
            data['timestamp_ms'] = _timestamp_ms()
            return data

        # Data dari socket
        with self._lock:
            return self._data_terbaru.copy()

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
        """Hapus override, kembali ke data dari socket/default."""
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

    @property
    def is_connected(self) -> bool:
        """True jika ada Vision client yang terhubung."""
        return self._connected

    # ── Internal: Server Loop ──

    def _server_loop(self):
        """Background thread: buka server socket, terima koneksi, baca data."""
        try:
            self._server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self._server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self._server_socket.bind((self._host, self._port))
            self._server_socket.listen(1)
            self._server_socket.settimeout(1.0)  # Agar bisa cek self._running
            logger.info("VisionListener: menunggu koneksi Vision di %s:%d...",
                         self._host, self._port)
        except OSError as e:
            logger.error("VisionListener: gagal bind ke %s:%d — %s",
                          self._host, self._port, e)
            self._running = False
            return

        while self._running:
            # Terima koneksi baru
            try:
                client, addr = self._server_socket.accept()
                logger.info("VisionListener: Vision terhubung dari %s:%d", addr[0], addr[1])
                self._client_socket = client
                self._connected = True
                self._handle_client(client)
            except socket.timeout:
                continue  # Loop kembali, cek self._running
            except OSError:
                if self._running:
                    logger.warning("VisionListener: server socket error, retry...")
                break

        self._connected = False

    def _handle_client(self, client: socket.socket):
        """Baca JSON line-delimited dari client yang terkoneksi."""
        client.settimeout(1.0)
        buffer = ""

        while self._running:
            try:
                chunk = client.recv(RECV_BUFFER_SIZE)
                if not chunk:
                    # Client disconnect
                    logger.info("VisionListener: Vision terputus (client disconnect)")
                    break

                buffer += chunk.decode("utf-8", errors="replace")

                # Proses semua baris lengkap yang ada di buffer
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    line = line.strip()
                    if not line:
                        continue
                    self._process_json_line(line)

            except socket.timeout:
                continue
            except (ConnectionResetError, BrokenPipeError, OSError) as e:
                logger.warning("VisionListener: koneksi terputus — %s", e)
                break

        self._connected = False
        try:
            client.close()
        except OSError:
            pass

    def _process_json_line(self, line: str):
        """Parse satu baris JSON dan simpan sebagai data terbaru."""
        try:
            data = json.loads(line)
        except json.JSONDecodeError as e:
            logger.warning("VisionListener: JSON parse error — %s", e)
            return

        if not _validate_vision_data(data):
            logger.warning("VisionListener: data tidak lengkap, diabaikan: %s",
                            list(data.keys()))
            return

        with self._lock:
            self._data_terbaru = data

        logger.debug("VisionListener: data diterima — label=%s, conf=%.2f, jarak=%.1f cm",
                      data.get('label', '?'),
                      data.get('confidence', 0.0),
                      data.get('jarak_estimasi_cm', 0.0))
