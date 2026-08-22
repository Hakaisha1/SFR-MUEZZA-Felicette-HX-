"""
test_uart.py — Unit Test untuk uart_driver.py

Menguji:
  - Perhitungan checksum XOR
  - Pembangunan paket (header, length, checksum)
  - Parsing paket valid dan korup
  - Parser telemetri TLM_ROBOT_STATUS dan TLM_SENSOR_DATA
"""

import struct
import sys
import os
import unittest

# Tambahkan parent directory ke path supaya bisa import uart_driver
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from uart_driver import (
    hitung_checksum,
    bangun_paket,
    parse_telemetri_status,
    parse_telemetri_sensor,
    UARTDriver,
    HEADER_1, HEADER_2,
    CMD_GERAK_OMNIDIRECTIONAL, CMD_MANIPULATOR, CMD_STATE_CONTROL,
    TLM_ROBOT_STATUS, TLM_SENSOR_DATA,
)


class MockSerial:
    """Mock serial port untuk testing tanpa hardware."""

    def __init__(self, data_to_read: bytes = b''):
        self._read_buffer = bytearray(data_to_read)
        self._write_buffer = bytearray()

    def read(self, size: int) -> bytes:
        result = bytes(self._read_buffer[:size])
        self._read_buffer = self._read_buffer[size:]
        return result

    def write(self, data: bytes):
        self._write_buffer.extend(data)

    @property
    def written(self) -> bytes:
        return bytes(self._write_buffer)

    def feed(self, data: bytes):
        """Tambahkan data ke read buffer."""
        self._read_buffer.extend(data)


# ─────────────────────────────────────────────
# Test Checksum
# ─────────────────────────────────────────────
class TestChecksum(unittest.TestCase):

    def test_checksum_empty_payload(self):
        """Checksum dengan payload kosong = msg_id itu sendiri."""
        self.assertEqual(hitung_checksum(0x12, b''), 0x12)

    def test_checksum_single_byte(self):
        """Checksum msg_id=0x12 XOR payload=0x03 = 0x11."""
        self.assertEqual(hitung_checksum(0x12, bytes([0x03])), 0x12 ^ 0x03)

    def test_checksum_multi_byte(self):
        """Checksum XOR chain."""
        msg_id = 0x10
        payload = bytes([0x01, 0x02, 0x03])
        expected = 0x10 ^ 0x01 ^ 0x02 ^ 0x03
        self.assertEqual(hitung_checksum(msg_id, payload), expected)

    def test_checksum_result_within_byte(self):
        """Checksum selalu dalam rentang 0-255."""
        result = hitung_checksum(0xFF, bytes([0xFF]))
        self.assertEqual(result, 0x00)
        self.assertLessEqual(result, 0xFF)


# ─────────────────────────────────────────────
# Test Bangun Paket
# ─────────────────────────────────────────────
class TestBangunPaket(unittest.TestCase):

    def test_paket_structure(self):
        """Verifikasi struktur paket: header + length + msg_id + payload + checksum."""
        msg_id = 0x12
        payload = bytes([0x03])
        paket = bangun_paket(msg_id, payload)

        self.assertEqual(paket[0], HEADER_1)      # 0xAA
        self.assertEqual(paket[1], HEADER_2)      # 0x55
        self.assertEqual(paket[2], len(payload))  # LENGTH
        self.assertEqual(paket[3], msg_id)        # MSG_ID
        self.assertEqual(paket[4], 0x03)          # PAYLOAD
        self.assertEqual(paket[5], hitung_checksum(msg_id, payload))  # CHECKSUM

    def test_paket_length_total(self):
        """Total panjang = 4 (header+len+msgid) + payload_len + 1 (checksum)."""
        payload = struct.pack('<fff', 0.3, 0.0, 0.0)  # 12 bytes
        paket = bangun_paket(0x10, payload)
        self.assertEqual(len(paket), 4 + 12 + 1)

    def test_paket_empty_payload(self):
        """Paket dengan payload kosong."""
        paket = bangun_paket(0x12, b'')
        self.assertEqual(len(paket), 5)  # header(2) + length(1) + msgid(1) + checksum(1)
        self.assertEqual(paket[2], 0)    # LENGTH = 0

    def test_paket_cmd_state_control(self):
        """Paket CMD_STATE_CONTROL (0x12) untuk EMERGENCY_STOP (3)."""
        payload = struct.pack('<B', 3)
        paket = bangun_paket(CMD_STATE_CONTROL, payload)
        self.assertEqual(paket[3], 0x12)
        self.assertEqual(paket[4], 3)


# ─────────────────────────────────────────────
# Test Baca Paket
# ─────────────────────────────────────────────
class TestBacaPaket(unittest.TestCase):

    def _build_raw_packet(self, msg_id, payload):
        """Helper: bangun raw packet bytes untuk feed ke MockSerial."""
        return bangun_paket(msg_id, payload)

    def test_baca_paket_valid(self):
        """Baca paket valid → return (msg_id, payload)."""
        payload = struct.pack('<B', 1)  # WALKING
        raw = self._build_raw_packet(CMD_STATE_CONTROL, payload)
        mock = MockSerial(raw)
        driver = UARTDriver(mock)

        result = driver.baca_paket()
        self.assertIsNotNone(result)
        msg_id, data = result
        self.assertEqual(msg_id, CMD_STATE_CONTROL)
        self.assertEqual(data, payload)

    def test_baca_paket_korup_checksum(self):
        """Paket dengan checksum salah → return None."""
        payload = struct.pack('<B', 1)
        raw = bytearray(self._build_raw_packet(CMD_STATE_CONTROL, payload))
        raw[-1] ^= 0xFF  # Corrupt checksum
        mock = MockSerial(bytes(raw))
        driver = UARTDriver(mock)

        result = driver.baca_paket()
        self.assertIsNone(result)

    def test_baca_paket_header_salah(self):
        """Data tanpa header valid → return None."""
        mock = MockSerial(bytes([0x00, 0x00, 0x01, 0x12, 0x03, 0x11]))
        driver = UARTDriver(mock)

        result = driver.baca_paket()
        self.assertIsNone(result)

    def test_baca_paket_data_terpotong(self):
        """Data terpotong (kurang byte) → return None."""
        mock = MockSerial(bytes([HEADER_1, HEADER_2, 0x04]))  # Length=4 tapi tidak ada data
        driver = UARTDriver(mock)

        result = driver.baca_paket()
        self.assertIsNone(result)

    def test_baca_paket_gerak_omnidirectional(self):
        """Baca CMD_GERAK_OMNIDIRECTIONAL (12 byte payload)."""
        vx, vy, vyaw = 0.3, -0.1, 0.5
        payload = struct.pack('<fff', vx, vy, vyaw)
        raw = self._build_raw_packet(CMD_GERAK_OMNIDIRECTIONAL, payload)
        mock = MockSerial(raw)
        driver = UARTDriver(mock)

        result = driver.baca_paket()
        self.assertIsNotNone(result)
        msg_id, data = result
        self.assertEqual(msg_id, CMD_GERAK_OMNIDIRECTIONAL)
        rx, ry, ryaw = struct.unpack('<fff', data)
        self.assertAlmostEqual(rx, vx, places=5)
        self.assertAlmostEqual(ry, vy, places=5)
        self.assertAlmostEqual(ryaw, vyaw, places=5)


# ─────────────────────────────────────────────
# Test Parse Telemetri
# ─────────────────────────────────────────────
class TestParseTelemetri(unittest.TestCase):

    def test_parse_robot_status(self):
        """Parse TLM_ROBOT_STATUS (0x20) dengan data valid."""
        state = 1          # WALKING
        tegangan = 11.2    # Volt
        error = 0x00       # Tidak ada error
        payload = struct.pack('<BfB', state, tegangan, error)

        result = parse_telemetri_status(payload)
        self.assertIsNotNone(result)
        self.assertEqual(result['state_sekarang'], 1)
        self.assertAlmostEqual(result['tegangan_baterai'], 11.2, places=1)
        self.assertEqual(result['hardware_error'], 0)

    def test_parse_robot_status_with_error(self):
        """Parse TLM_ROBOT_STATUS dengan hardware error flags."""
        payload = struct.pack('<BfB', 3, 9.8, 0x05)  # Bit 0 + Bit 2 = Servo + Ultrasonik
        result = parse_telemetri_status(payload)
        self.assertEqual(result['hardware_error'], 0x05)

    def test_parse_robot_status_wrong_length(self):
        """Payload length salah → return None."""
        result = parse_telemetri_status(b'\x01\x02\x03')  # 3 bytes, bukan 6
        self.assertIsNone(result)

    def test_parse_sensor_data(self):
        """Parse TLM_SENSOR_DATA (0x21) dengan data valid."""
        dep, bel, kir, kan = 150, 300, 9999, 200  # mm
        roll, pitch = 1.5, -0.8  # derajat
        payload = struct.pack('<HHHHff', dep, bel, kir, kan, roll, pitch)

        result = parse_telemetri_sensor(payload)
        self.assertIsNotNone(result)
        self.assertEqual(result['us_depan_mm'], 150)
        self.assertEqual(result['us_belakang_mm'], 300)
        self.assertEqual(result['us_kiri_mm'], 9999)  # Tidak terdeteksi
        self.assertEqual(result['us_kanan_mm'], 200)
        self.assertAlmostEqual(result['imu_roll'], 1.5, places=1)
        self.assertAlmostEqual(result['imu_pitch'], -0.8, places=1)

    def test_parse_sensor_data_wrong_length(self):
        """Payload length salah → return None."""
        result = parse_telemetri_sensor(b'\x01\x02')
        self.assertIsNone(result)


# ─────────────────────────────────────────────
# Test UARTDriver Kirim
# ─────────────────────────────────────────────
class TestUARTDriverKirim(unittest.TestCase):

    def test_kirim_gerak(self):
        """kirim_gerak() menulis paket yang benar ke serial."""
        mock = MockSerial()
        driver = UARTDriver(mock)
        driver.kirim_gerak(0.3, 0.0, 0.0)

        written = mock.written
        self.assertEqual(written[0], HEADER_1)
        self.assertEqual(written[1], HEADER_2)
        self.assertEqual(written[2], 12)  # payload = 3 float = 12 bytes
        self.assertEqual(written[3], CMD_GERAK_OMNIDIRECTIONAL)

    def test_kirim_manipulator(self):
        """kirim_manipulator() menulis paket yang benar."""
        mock = MockSerial()
        driver = UARTDriver(mock)
        driver.kirim_manipulator(45.0, 90.0, 2)

        written = mock.written
        self.assertEqual(written[3], CMD_MANIPULATOR)
        self.assertEqual(written[2], 9)  # payload = 2 float + 1 uint8 = 9 bytes

    def test_kirim_state_control(self):
        """kirim_state_control() menulis paket yang benar."""
        mock = MockSerial()
        driver = UARTDriver(mock)
        driver.kirim_state_control(3)  # EMERGENCY_STOP

        written = mock.written
        self.assertEqual(written[3], CMD_STATE_CONTROL)
        self.assertEqual(written[2], 1)  # payload = 1 byte
        self.assertEqual(written[4], 3)  # state_target = 3


# ─────────────────────────────────────────────
# Test UARTDriver Baca Telemetri
# ─────────────────────────────────────────────
class TestUARTDriverBacaTelemetri(unittest.TestCase):

    def test_baca_telemetri_status(self):
        """baca_telemetri() parse TLM_ROBOT_STATUS dengan benar."""
        payload = struct.pack('<BfB', 1, 11.5, 0)
        raw = bangun_paket(TLM_ROBOT_STATUS, payload)
        mock = MockSerial(raw)
        driver = UARTDriver(mock)

        result = driver.baca_telemetri()
        self.assertEqual(result['state_sekarang'], 1)
        self.assertAlmostEqual(result['tegangan_baterai'], 11.5, places=1)

    def test_baca_telemetri_sensor(self):
        """baca_telemetri() parse TLM_SENSOR_DATA dengan benar."""
        payload = struct.pack('<HHHHff', 100, 200, 300, 400, 2.0, -1.0)
        raw = bangun_paket(TLM_SENSOR_DATA, payload)
        mock = MockSerial(raw)
        driver = UARTDriver(mock)

        result = driver.baca_telemetri()
        self.assertEqual(result['us_depan_mm'], 100)
        self.assertAlmostEqual(result['imu_roll'], 2.0, places=1)

    def test_baca_telemetri_empty(self):
        """Tidak ada data → return dict kosong."""
        mock = MockSerial(b'')
        driver = UARTDriver(mock)

        result = driver.baca_telemetri()
        self.assertEqual(result, {})


if __name__ == '__main__':
    unittest.main()
