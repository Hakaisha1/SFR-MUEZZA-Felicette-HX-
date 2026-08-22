"""
main.py — Entry Point Loop Utama Integration

Mengorkestrasi:
  1. Ambil data dari Vision (via VisionListener)
  2. Baca telemetri dari STM32 (via UARTDriver)
  3. Update state machine → hasilkan commands
  4. Kirim commands ke STM32

Loop berjalan pada 10 Hz (interval 100ms) sesuai guide.md.
Watchdog: STM32 akan masuk EMERGENCY_STOP jika tidak menerima
paket selama > 500ms — sehingga loop ini wajib kirim minimal
1 command per cycle.
"""

import sys
import time
import logging
import struct

# ── Konfigurasi Logging ──
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(name)s] %(levelname)s: %(message)s',
    datefmt='%H:%M:%S',
)
logger = logging.getLogger('main')

# ── Import modul internal ──
from uart_driver import UARTDriver, CMD_STATE_CONTROL
from state_machine import StateMachine, STM32State
from vision_listener import VisionListener

# ── Konstanta ──
INTERVAL_LOOP_S = 0.1  # 10 Hz — kirim command tiap 100ms


def buat_serial_port(port: str = '/dev/ttyAMA0', baudrate: int = 115200):
    """
    Buat koneksi serial ke STM32.

    Returns:
        serial.Serial instance, atau None jika pyserial tidak tersedia
        (mode dummy tanpa hardware).
    """
    try:
        import serial
        ser = serial.Serial(port, baudrate=baudrate, timeout=0.05)
        logger.info("Serial terhubung: %s @ %d baud", port, baudrate)
        return ser
    except ImportError:
        logger.warning("pyserial tidak ditemukan — berjalan dalam mode DUMMY (tanpa hardware)")
        return None
    except Exception as e:
        logger.error("Gagal membuka serial %s: %s", port, e)
        return None


class DummySerial:
    """Serial port dummy untuk testing tanpa hardware."""

    def write(self, data: bytes):
        """Simulasi write — hanya log."""
        logger.debug("DummySerial TX: %d bytes", len(data))

    def read(self, size: int) -> bytes:
        """Simulasi read — selalu return kosong."""
        return b''


def main():
    """Entry point utama."""
    logger.info("=" * 50)
    logger.info("SAR Hexapod MUEZZA — Integration Controller")
    logger.info("=" * 50)

    # ── Inisialisasi ──
    ser = buat_serial_port()
    if ser is None:
        logger.info("Menggunakan DummySerial (mode tanpa hardware)")
        ser = DummySerial()

    uart = UARTDriver(ser)
    sm = StateMachine()
    vision = VisionListener()

    logger.info("State awal: %s (STM32: %s)", sm.state.name, sm.stm32_state.name)
    logger.info("Loop interval: %.0f ms (%.0f Hz)", INTERVAL_LOOP_S * 1000, 1 / INTERVAL_LOOP_S)
    logger.info("Memulai loop utama... (Ctrl+C untuk berhenti)")
    logger.info("-" * 50)

    # ── Kirim state awal ke STM32 ──
    uart.kirim_state_control(sm.stm32_state.value)

    cycle_count = 0

    try:
        while True:
            tik_mulai = time.monotonic()
            cycle_count += 1

            # 1. Ambil data vision
            data_vision = vision.ambil_data_terbaru()

            # Cek stale
            if vision.is_stale(data_vision):
                logger.debug("Data vision stale, gunakan label='tidak_ada'")
                data_vision['label'] = 'tidak_ada'
                data_vision['confidence'] = 0.0

            # 2. Baca telemetri dari STM32
            data_telemetry = uart.baca_telemetri()

            # 3. Update state machine
            commands = sm.update(data_vision, data_telemetry)

            # 4. Kirim commands ke STM32
            for cmd in commands:
                uart.kirim_perintah(cmd['msg_id'], cmd['payload'])

            # Jika tidak ada command yang dikirim, kirim heartbeat
            # (state control ulang) untuk menjaga watchdog STM32
            if not commands:
                uart.kirim_state_control(sm.stm32_state.value)

            # Log periodik (setiap 50 cycle = 5 detik)
            if cycle_count % 50 == 0:
                logger.info("Cycle #%d | State: %s | Vision: %s (%.2f) | Jarak: %.1f cm",
                            cycle_count,
                            sm.state.name,
                            data_vision.get('label', '?'),
                            data_vision.get('confidence', 0.0),
                            data_vision.get('jarak_estimasi_cm', 0.0))

            # 5. Maintain interval
            elapsed = time.monotonic() - tik_mulai
            sleep_time = max(0, INTERVAL_LOOP_S - elapsed)
            time.sleep(sleep_time)

    except KeyboardInterrupt:
        logger.info("")
        logger.info("=" * 50)
        logger.info("Ctrl+C diterima — menghentikan robot...")

        # Kirim EMERGENCY_STOP sebelum keluar
        try:
            uart.kirim_state_control(STM32State.EMERGENCY_STOP.value)
            logger.info("EMERGENCY_STOP dikirim ke STM32")
        except Exception as e:
            logger.error("Gagal mengirim EMERGENCY_STOP: %s", e)

        logger.info("Total cycles: %d", cycle_count)
        logger.info("Selesai.")
        logger.info("=" * 50)

    finally:
        # Tutup serial jika bukan dummy
        if hasattr(ser, 'close'):
            try:
                ser.close()
            except Exception:
                pass


if __name__ == '__main__':
    main()
