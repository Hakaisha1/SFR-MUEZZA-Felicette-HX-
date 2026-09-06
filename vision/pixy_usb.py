"""
pixy_usb.py — Wrapper Python untuk PixyCam2 via USB (libpixyusb2).

Modul ini menyediakan interface Python untuk membaca Block data
dari PixyCam2 yang terhubung via USB ke Raspberry Pi.

Menggunakan native Python `pixy` module yang di-build dari
libpixyusb2 (SWIG wrapper). Jika modul native tidak tersedia,
menyediakan DummyPixy untuk testing tanpa hardware.

Setup di Raspberry Pi:
  1. sudo apt-get install git libusb-1.0-0-dev g++ build-essential
  2. git clone https://github.com/charmedlabs/pixy2
  3. cd pixy2/scripts && ./build_libpixyusb2.sh
  4. cd ../build/python_demos && python setup.py install
"""

import logging
from dataclasses import dataclass

logger = logging.getLogger(__name__)


@dataclass
class PixyBlock:
    """Representasi satu Block yang terdeteksi oleh PixyCam2."""

    sig: int             # Nomor signature (1-7)
    x_center: int        # Koordinat X pusat block
    y_center: int        # Koordinat Y pusat block
    width: int           # Lebar block (piksel)
    height: int          # Tinggi block (piksel)
    angle: int           # Sudut (untuk color codes, biasanya 0)
    tracking_index: int  # Index tracking antar frame
    age: int             # Jumlah frame objek ini sudah di-track


class PixyUSB:
    """
    Interface Python untuk PixyCam2 via USB.

    Wraps the native `pixy` SWIG module.
    """

    def __init__(self):
        self._pixy = None
        self._connected = False
        self._block_array = None

    def init(self) -> bool:
        """
        Inisialisasi koneksi USB ke PixyCam2.

        Returns:
            True jika berhasil, False jika gagal.
        """
        try:
            from pixycamev2 import pixy as pixy_module
            self._pixy = pixy_module
        except ImportError:
            try:
                import pixy as pixy_module
                self._pixy = pixy_module
            except ImportError:
                logger.error(
                    "Module 'pixy' tidak ditemukan. "
                    "Pastikan libpixyusb2 sudah di-build dan di-install. "
                    "Lihat: https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:hooking_up_pixy_to_a_raspberry_pi"
                )
                return False

        try:
            ret = self._pixy.init()
            if ret < 0:
                logger.error("pixy.init() gagal dengan kode: %d", ret)
                return False
            
            # Alokasikan BlockArray (maksimal 100 block) sesuai SWIG API
            self._block_array = self._pixy.BlockArray(100)
            
            self._connected = True
            logger.info("PixyCam2 terhubung via USB (init OK)")
            return True
        except Exception as e:
            logger.error("PixyCam2 init error: %s", e)
            return False

    def get_blocks(self, sigmap: int = 3, max_blocks: int = 10) -> list:
        """
        Ambil Block data dari PixyCam2 (Color Connected Components).

        Args:
            sigmap: Bitmask signature yang diinginkan (3 = sig 1 + sig 2).
            max_blocks: Jumlah block maksimal.

        Returns:
            List of PixyBlock.
        """
        if not self._connected or self._pixy is None or self._block_array is None:
            return []

        try:
            # Sesuai SWIG API Pixy2: ccc_get_blocks(max_blocks, block_array)
            # sigmap diatur lewat parameter fungsi lain jika perlu, tapi
            # default-nya ccc_get_blocks mengambil semua yg terdeteksi.
            count = self._pixy.ccc_get_blocks(max_blocks, self._block_array)
        except Exception as e:
            logger.warning("ccc_get_blocks error: %s", e)
            return []

        if count <= 0:
            return []

        blocks = []
        for i in range(count):
            try:
                block = self._block_array[i]
                blocks.append(PixyBlock(
                    sig=block.m_signature,
                    x_center=block.m_x,
                    y_center=block.m_y,
                    width=block.m_width,
                    height=block.m_height,
                    angle=block.m_angle,
                    tracking_index=block.m_index,
                    age=block.m_age,
                ))
            except (IndexError, AttributeError) as e:
                logger.debug("Block parse error at index %d: %s", i, e)
                break

        return blocks

    def set_lamp(self, upper: int, lower: int) -> bool:
        """
        Menyalakan/mematikan lampu LED PixyCam.

        Args:
            upper: 1 nyalakan 2 LED putih atas (flash), 0 mati.
            lower: 1 nyalakan LED RGB bawah, 0 mati.
        """
        if not self._connected or self._pixy is None:
            return False

        try:
            try:
                from pixycamev2 import pixy as pixy_module
            except ImportError:
                import pixy as pixy_module

            # panggil fungsi native pixy.set_lamp(upper, lower)
            pixy_module.set_lamp(upper, lower)
            return True
        except Exception as e:
            logger.warning("set_lamp error: %s", e)
            return False

    @property
    def is_connected(self) -> bool:
        return self._connected

    def close(self):
        """Tutup koneksi (placeholder, libpixyusb2 tidak punya explicit close)."""
        self._connected = False
        logger.info("PixyCam2 koneksi ditutup")


class DummyPixy:
    """
    Simulasi PixyCam2 untuk testing tanpa hardware.

    Mengembalikan Block statis yang bisa di-override untuk testing.
    """

    def __init__(self):
        self._blocks = []
        self._connected = True
        logger.info("DummyPixy diinisialisasi (mode tanpa hardware)")

    def init(self) -> bool:
        logger.info("DummyPixy: init OK (simulated)")
        return True

    def get_blocks(self, sigmap: int = 3, max_blocks: int = 10) -> list:
        """Return blocks yang sudah di-set via set_blocks()."""
        return self._blocks[:max_blocks]

    def set_blocks(self, blocks: list):
        """Set blocks untuk simulasi. Menerima list of PixyBlock."""
        self._blocks = blocks

    def set_lamp(self, upper: int, lower: int) -> bool:
        logger.info("DummyPixy: set_lamp(upper=%d, lower=%d)", upper, lower)
        return True

    @property
    def is_connected(self) -> bool:
        return self._connected

    def close(self):
        self._connected = False
