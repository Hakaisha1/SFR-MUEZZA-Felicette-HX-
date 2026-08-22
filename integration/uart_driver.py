"""
uart_driver.py — Driver Komunikasi UART RPi5 ↔ STM32

Mengimplementasikan protokol paket sesuai protokol_komunikasi.json:
  [0xAA] [0x55] [LENGTH] [MSG_ID] [PAYLOAD...] [CHECKSUM_XOR]

Checksum = XOR dari MSG_ID dan semua byte PAYLOAD.
Endianness: Little-Endian.
"""

import struct
import logging
import time

logger = logging.getLogger(__name__)

# ──────────────────────────────────────────────
# Konstanta Protokol
# ──────────────────────────────────────────────
HEADER_1 = 0xAA
HEADER_2 = 0x55

# Command IDs (RPi5 → STM32)
CMD_GERAK_OMNIDIRECTIONAL = 0x10
CMD_MANIPULATOR = 0x11
CMD_STATE_CONTROL = 0x12

# Telemetry IDs (STM32 → RPi5)
TLM_ROBOT_STATUS = 0x20
TLM_SENSOR_DATA = 0x21


# ──────────────────────────────────────────────
# Fungsi Utilitas
# ──────────────────────────────────────────────
def hitung_checksum(msg_id: int, payload: bytes) -> int:
    """Hitung XOR checksum dari MSG_ID dan semua byte PAYLOAD."""
    checksum = msg_id
    for b in payload:
        checksum ^= b
    return checksum & 0xFF


def bangun_paket(msg_id: int, payload: bytes) -> bytes:
    """
    Bangun paket UART lengkap.

    Format: [0xAA][0x55][LENGTH][MSG_ID][PAYLOAD...][CHECKSUM]
    LENGTH = jumlah byte payload (tidak termasuk MSG_ID dan CHECKSUM).
    """
    length = len(payload)
    checksum = hitung_checksum(msg_id, payload)
    return bytes([HEADER_1, HEADER_2, length, msg_id]) + payload + bytes([checksum])


# ──────────────────────────────────────────────
# Parser Telemetri
# ──────────────────────────────────────────────
def parse_telemetri_status(payload: bytes) -> dict:
    """
    Parse TLM_ROBOT_STATUS (0x20).

    Payload 6 bytes: <BfB
      - state_sekarang  : uint8  (0=IDLE, 1=WALKING, 2=EVACUATING, 3=EMERGENCY_STOP)
      - tegangan_baterai : float32 (Volt)
      - status_hardware_error : uint8 (bitmask)
    """
    if len(payload) != 6:
        logger.warning("TLM_ROBOT_STATUS: payload length %d, expected 6", len(payload))
        return None
    state, tegangan, error = struct.unpack('<BfB', payload)
    return {
        'state_sekarang': state,
        'tegangan_baterai': tegangan,
        'hardware_error': error,
    }


def parse_telemetri_sensor(payload: bytes) -> dict:
    """
    Parse TLM_SENSOR_DATA (0x21).

    Payload 16 bytes: <HHHHff
      - us_depan_mm    : uint16 (mm, 9999 = tidak terdeteksi)
      - us_belakang_mm : uint16
      - us_kiri_mm     : uint16
      - us_kanan_mm    : uint16
      - imu_roll       : float32 (derajat)
      - imu_pitch      : float32 (derajat)
    """
    if len(payload) != 16:
        logger.warning("TLM_SENSOR_DATA: payload length %d, expected 16", len(payload))
        return None
    dep, bel, kir, kan, roll, pitch = struct.unpack('<HHHHff', payload)
    return {
        'us_depan_mm': dep,
        'us_belakang_mm': bel,
        'us_kiri_mm': kir,
        'us_kanan_mm': kan,
        'imu_roll': roll,
        'imu_pitch': pitch,
    }


# ──────────────────────────────────────────────
# UARTDriver Class
# ──────────────────────────────────────────────
class UARTDriver:
    """
    Driver UART untuk komunikasi RPi5 ↔ STM32.

    Menerima serial port object melalui constructor (dependency injection)
    sehingga mudah di-mock saat testing.
    """

    def __init__(self, serial_port):
        """
        Args:
            serial_port: Objek serial (pyserial Serial instance atau mock).
                         Harus punya method read() dan write().
        """
        self.ser = serial_port
        self._last_telemetry_status = None
        self._last_telemetry_sensor = None
        self._last_telemetry_time = 0.0

    # ── Kirim ──

    def kirim_perintah(self, msg_id: int, payload: bytes):
        """Bangun dan kirim satu paket UART ke STM32."""
        paket = bangun_paket(msg_id, payload)
        self.ser.write(paket)
        logger.debug("TX >> msg_id=0x%02X len=%d", msg_id, len(payload))

    def kirim_gerak(self, vx: float, vy: float, vyaw: float):
        """Kirim CMD_GERAK_OMNIDIRECTIONAL (0x10)."""
        payload = struct.pack('<fff', vx, vy, vyaw)
        self.kirim_perintah(CMD_GERAK_OMNIDIRECTIONAL, payload)

    def kirim_manipulator(self, sudut1: float, sudut2: float, gripper: int):
        """Kirim CMD_MANIPULATOR (0x11)."""
        payload = struct.pack('<ffB', sudut1, sudut2, gripper)
        self.kirim_perintah(CMD_MANIPULATOR, payload)

    def kirim_state_control(self, state_target: int):
        """Kirim CMD_STATE_CONTROL (0x12)."""
        payload = struct.pack('<B', state_target)
        self.kirim_perintah(CMD_STATE_CONTROL, payload)

    # ── Terima ──

    def baca_paket(self):
        """
        Baca satu paket dari UART.

        Returns:
            Tuple (msg_id, payload) jika berhasil, atau None jika gagal/korup.
        """
        # Baca 2 byte header
        buf = self.ser.read(2)
        if len(buf) < 2 or buf[0] != HEADER_1 or buf[1] != HEADER_2:
            return None

        # Baca length
        length_byte = self.ser.read(1)
        if not length_byte:
            return None
        length = length_byte[0]

        # Baca MSG_ID + PAYLOAD + CHECKSUM
        sisa = self.ser.read(length + 2)  # 1 byte MSG_ID + length byte PAYLOAD + 1 byte CHECKSUM
        if len(sisa) < length + 2:
            return None

        msg_id = sisa[0]
        payload = sisa[1:1 + length]
        checksum_diterima = sisa[-1]
        checksum_dihitung = hitung_checksum(msg_id, payload)

        if checksum_diterima != checksum_dihitung:
            logger.warning(
                "Checksum mismatch: diterima=0x%02X, dihitung=0x%02X (msg_id=0x%02X)",
                checksum_diterima, checksum_dihitung, msg_id
            )
            return None

        logger.debug("RX << msg_id=0x%02X len=%d", msg_id, length)
        return msg_id, payload

    def baca_telemetri(self) -> dict:
        """
        Baca dan parse semua paket telemetri yang tersedia.

        Returns:
            Dict berisi data telemetri gabungan terbaru, atau dict kosong
            jika tidak ada data baru.
        """
        result = {}
        # Baca semua paket yang tersedia (non-blocking berkat timeout serial)
        while True:
            parsed = self.baca_paket()
            if parsed is None:
                break
            msg_id, payload = parsed

            if msg_id == TLM_ROBOT_STATUS:
                data = parse_telemetri_status(payload)
                if data:
                    self._last_telemetry_status = data
                    self._last_telemetry_time = time.monotonic()
                    result.update(data)

            elif msg_id == TLM_SENSOR_DATA:
                data = parse_telemetri_sensor(payload)
                if data:
                    self._last_telemetry_sensor = data
                    result.update(data)

        return result

    @property
    def telemetri_terakhir(self) -> dict:
        """Gabungan data telemetri terakhir yang diterima."""
        gabungan = {}
        if self._last_telemetry_status:
            gabungan.update(self._last_telemetry_status)
        if self._last_telemetry_sensor:
            gabungan.update(self._last_telemetry_sensor)
        return gabungan

    @property
    def waktu_telemetri_terakhir(self) -> float:
        """Waktu monotonic terakhir menerima TLM_ROBOT_STATUS (heartbeat)."""
        return self._last_telemetry_time
