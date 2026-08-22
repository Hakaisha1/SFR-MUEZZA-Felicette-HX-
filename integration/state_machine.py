"""
state_machine.py — State Machine Utama Robot SAR Hexapod

Mengimplementasikan dua-layer state machine sesuai state_machine.md:
  - Layer 1 (STM32): IDLE, WALKING, EVACUATING, EMERGENCY_STOP
  - Layer 2 (RPi5 sub-state): SEARCHING, APPROACHING, CLASSIFYING,
    GRIPPING, CARRYING, RELEASING, DROP_DONE, ERROR_RETRY, EMERGENCY_STOP

Semua threshold merujuk ke data_contract.json.
"""

import time
import logging
from enum import Enum

logger = logging.getLogger(__name__)

# ──────────────────────────────────────────────
# Threshold dari data_contract.json
# ──────────────────────────────────────────────
CONFIDENCE_THRESHOLD = 0.75       # Minimum confidence untuk deteksi valid
JARAK_EVAKUASI_CM = 15            # Jarak target < ini → CLASSIFYING
TIMEOUT_OBJEK_HILANG_S = 2.0      # Objek hilang > ini → kembali SEARCHING
JUMLAH_FRAME_VOTING = 5           # Frame voting di CLASSIFYING
MAX_RETRY_GRIPPING = 2            # Retry gripping sebelum ERROR_RETRY


# ──────────────────────────────────────────────
# Enum Definitions
# ──────────────────────────────────────────────
class STM32State(Enum):
    """State yang dikenali STM32, dikirim via CMD_STATE_CONTROL (0x12)."""
    IDLE = 0
    WALKING = 1
    EVACUATING = 2
    EMERGENCY_STOP = 3


class SubState(Enum):
    """Sub-state detail di sisi RPi5 (logika Integration)."""
    SEARCHING = 0
    APPROACHING = 1
    CLASSIFYING = 2
    GRIPPING = 3
    CARRYING = 4
    RELEASING = 5
    DROP_DONE = 6
    ERROR_RETRY = 7
    EMERGENCY_STOP = 8


# Mapping sub-state RPi5 → state STM32
STM32_STATE_MAP = {
    SubState.SEARCHING:      STM32State.WALKING,
    SubState.APPROACHING:    STM32State.WALKING,
    SubState.CLASSIFYING:    STM32State.IDLE,
    SubState.GRIPPING:       STM32State.EVACUATING,
    SubState.CARRYING:       STM32State.EVACUATING,
    SubState.RELEASING:      STM32State.EVACUATING,
    SubState.DROP_DONE:      STM32State.IDLE,
    SubState.ERROR_RETRY:    STM32State.IDLE,
    SubState.EMERGENCY_STOP: STM32State.EMERGENCY_STOP,
}


# ──────────────────────────────────────────────
# State Machine
# ──────────────────────────────────────────────
class StateMachine:
    """
    State machine utama robot SAR.

    Dipanggil setiap loop cycle oleh main.py.
    Menerima data vision dan telemetri, menghasilkan list command
    untuk dikirim ke STM32.
    """

    def __init__(self):
        self.state = SubState.SEARCHING
        self._prev_stm32_state = None

        # Tracking untuk APPROACHING
        self._waktu_objek_terakhir_terlihat = 0.0

        # Tracking untuk CLASSIFYING (voting frame)
        self._frame_votes = []  # List of 'dummy' / 'riil'

        # Tracking untuk GRIPPING
        self._gripping_retry_count = 0
        self._gripping_commanded = False

        # Tracking untuk RELEASING
        self._releasing_commanded = False

    @property
    def stm32_state(self) -> STM32State:
        """State STM32 yang bersesuaian dengan sub-state saat ini."""
        return STM32_STATE_MAP[self.state]

    def _transisi(self, state_baru: SubState):
        """Lakukan transisi state dengan logging."""
        state_lama = self.state
        self.state = state_baru
        logger.info("STATE: %s → %s (STM32: %s)",
                     state_lama.name, state_baru.name,
                     self.stm32_state.name)

    def update(self, data_vision: dict, data_telemetry: dict) -> list:
        """
        Dipanggil setiap loop cycle.

        Args:
            data_vision: Dict dari VisionListener, format sesuai
                         kontrak_vision_ke_integration.
            data_telemetry: Dict dari UARTDriver.baca_telemetri(),
                            gabungan TLM_ROBOT_STATUS + TLM_SENSOR_DATA.

        Returns:
            List of command dicts, masing-masing berisi:
              {'msg_id': int, 'payload': bytes}
        """
        commands = []

        # ── Cek EMERGENCY_STOP dari telemetri ──
        if data_telemetry.get('hardware_error', 0) != 0:
            if self.state != SubState.EMERGENCY_STOP:
                self._transisi(SubState.EMERGENCY_STOP)
                commands.append(self._cmd_state_control(STM32State.EMERGENCY_STOP))
            return commands

        # ── Logika per sub-state ──
        now = time.monotonic()

        if self.state == SubState.SEARCHING:
            commands.extend(self._handle_searching(data_vision, now))

        elif self.state == SubState.APPROACHING:
            commands.extend(self._handle_approaching(data_vision, now))

        elif self.state == SubState.CLASSIFYING:
            commands.extend(self._handle_classifying(data_vision))

        elif self.state == SubState.GRIPPING:
            commands.extend(self._handle_gripping(data_telemetry))

        elif self.state == SubState.CARRYING:
            commands.extend(self._handle_carrying(data_vision))

        elif self.state == SubState.RELEASING:
            commands.extend(self._handle_releasing(data_telemetry))

        elif self.state == SubState.DROP_DONE:
            commands.extend(self._handle_drop_done())

        elif self.state == SubState.ERROR_RETRY:
            commands.extend(self._handle_error_retry())

        # ── Kirim CMD_STATE_CONTROL jika state STM32 berubah ──
        current_stm32 = self.stm32_state
        if current_stm32 != self._prev_stm32_state:
            commands.append(self._cmd_state_control(current_stm32))
            self._prev_stm32_state = current_stm32

        return commands

    # ── Handler per State ──

    def _handle_searching(self, data_vision: dict, now: float) -> list:
        """SEARCHING: Jelajahi arena, cari target."""
        label = data_vision.get('label', 'tidak_ada')
        confidence = data_vision.get('confidence', 0.0)

        if label in ('dummy', 'riil') and confidence >= CONFIDENCE_THRESHOLD:
            self._waktu_objek_terakhir_terlihat = now
            self._transisi(SubState.APPROACHING)
            return []

        # Tetap searching — kirim command gerak eksplorasi
        # (pola eksplorasi: maju lambat + sedikit rotasi untuk scanning)
        return [self._cmd_gerak(0.3, 0.0, 0.1)]

    def _handle_approaching(self, data_vision: dict, now: float) -> list:
        """APPROACHING: Mendekati target, koreksi arah."""
        label = data_vision.get('label', 'tidak_ada')
        confidence = data_vision.get('confidence', 0.0)
        jarak = data_vision.get('jarak_estimasi_cm', 999.0)

        if label in ('dummy', 'riil') and confidence >= CONFIDENCE_THRESHOLD:
            self._waktu_objek_terakhir_terlihat = now

            # Cek apakah sudah cukup dekat
            if jarak < JARAK_EVAKUASI_CM:
                self._frame_votes.clear()
                self._transisi(SubState.CLASSIFYING)
                return []

            # Koreksi arah berdasarkan posisi_x_px
            # Asumsi resolusi 640px, tengah = 320
            posisi_x = data_vision.get('posisi_x_px', 320)
            error_x = (posisi_x - 320) / 320.0  # Normalize ke [-1, 1]
            vyaw = -error_x * 0.5  # Proporsional steering
            return [self._cmd_gerak(0.2, 0.0, vyaw)]

        else:
            # Objek hilang — cek timeout
            if (now - self._waktu_objek_terakhir_terlihat) > TIMEOUT_OBJEK_HILANG_S:
                logger.info("Objek hilang > %.1f detik, kembali SEARCHING",
                            TIMEOUT_OBJEK_HILANG_S)
                self._transisi(SubState.SEARCHING)
                return []

            # Tetap maju pelan sambil menunggu objek muncul lagi
            return [self._cmd_gerak(0.1, 0.0, 0.0)]

    def _handle_classifying(self, data_vision: dict) -> list:
        """CLASSIFYING: Berhenti, kumpulkan voting frame untuk konfirmasi."""
        label = data_vision.get('label', 'tidak_ada')

        if label in ('dummy', 'riil'):
            self._frame_votes.append(label)

        if len(self._frame_votes) >= JUMLAH_FRAME_VOTING:
            # Voting mayoritas
            count_riil = self._frame_votes.count('riil')
            count_dummy = self._frame_votes.count('dummy')
            self._frame_votes.clear()

            if count_riil > count_dummy:
                logger.info("Klasifikasi: RIIL (votes: riil=%d, dummy=%d)",
                            count_riil, count_dummy)
                self._gripping_retry_count = 0
                self._gripping_commanded = False
                self._transisi(SubState.GRIPPING)
            else:
                logger.info("Klasifikasi: DUMMY (votes: riil=%d, dummy=%d)",
                            count_riil, count_dummy)
                self._transisi(SubState.SEARCHING)

        return []  # Robot tetap diam (IDLE) selama CLASSIFYING

    def _handle_gripping(self, data_telemetry: dict) -> list:
        """GRIPPING: Memposisikan dan menutup gripper."""
        commands = []

        if not self._gripping_commanded:
            # Kirim command arm + gripper tutup kuat (riil)
            commands.append(self._cmd_manipulator(45.0, 90.0, 1))
            self._gripping_commanded = True
            return commands

        # Cek feedback — jika tidak ada servo error, anggap berhasil
        # (Nanti bisa ditambah logic feedback lebih detail)
        servo_error = data_telemetry.get('hardware_error', 0) & 0x01  # Bit 0 = Servo Error
        if servo_error:
            self._gripping_retry_count += 1
            logger.warning("Gripping gagal, percobaan ke-%d", self._gripping_retry_count)

            if self._gripping_retry_count >= MAX_RETRY_GRIPPING:
                self._transisi(SubState.ERROR_RETRY)
            else:
                # Retry: buka dulu, lalu coba lagi
                self._gripping_commanded = False
                commands.append(self._cmd_manipulator(0.0, 0.0, 0))
        else:
            # Berhasil capit
            logger.info("Gripping berhasil!")
            self._transisi(SubState.CARRYING)

        return commands

    def _handle_carrying(self, data_vision: dict) -> list:
        """CARRYING: Membawa korban ke drop zone."""
        # TODO: Logika deteksi drop zone (marker visual / odometry)
        #       Untuk saat ini, robot maju lurus saja
        #       Nanti akan diimplementasi saat integrasi penuh
        return [self._cmd_gerak(0.2, 0.0, 0.0)]

    def _handle_releasing(self, data_telemetry: dict) -> list:
        """RELEASING: Melepas gripper di drop zone."""
        commands = []

        if not self._releasing_commanded:
            # Buka gripper
            commands.append(self._cmd_manipulator(0.0, 0.0, 0))
            self._releasing_commanded = True
            return commands

        # Anggap selesai (feedback servo OK)
        self._releasing_commanded = False
        self._transisi(SubState.DROP_DONE)
        return commands

    def _handle_drop_done(self) -> list:
        """DROP_DONE: Selesai drop, transisi kembali ke SEARCHING."""
        logger.info("Drop selesai, kembali ke SEARCHING")
        self._transisi(SubState.SEARCHING)
        return []

    def _handle_error_retry(self) -> list:
        """ERROR_RETRY: Menangani kegagalan, kembali ke SEARCHING."""
        logger.warning("Error retry habis, kembali ke SEARCHING")
        self._gripping_retry_count = 0
        self._gripping_commanded = False
        self._transisi(SubState.SEARCHING)
        return []

    # ── Command Builders ──

    @staticmethod
    def _cmd_state_control(stm32_state: STM32State) -> dict:
        """Buat command CMD_STATE_CONTROL (0x12)."""
        import struct
        return {
            'msg_id': 0x12,
            'payload': struct.pack('<B', stm32_state.value),
        }

    @staticmethod
    def _cmd_gerak(vx: float, vy: float, vyaw: float) -> dict:
        """Buat command CMD_GERAK_OMNIDIRECTIONAL (0x10)."""
        import struct
        return {
            'msg_id': 0x10,
            'payload': struct.pack('<fff', vx, vy, vyaw),
        }

    @staticmethod
    def _cmd_manipulator(sudut1: float, sudut2: float, gripper: int) -> dict:
        """Buat command CMD_MANIPULATOR (0x11)."""
        import struct
        return {
            'msg_id': 0x11,
            'payload': struct.pack('<ffB', sudut1, sudut2, gripper),
        }
