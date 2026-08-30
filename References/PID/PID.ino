
// =============================================
//   PARAMETER DINAMIS - Ubah nilai di sini saja
// =============================================

// --- Langkah Normal ---
int angkatLutut       = 100;    // Tinggi angkat lutut (servo 9/12)
int ayunKaki          = 110;    // Jarak ayun kaki ke depan (servo 7/10)

// --- Langkah Rintangan ---
int angkatLututRintangan = 180; // Tinggi angkat lutut saat rintangan
int ayunKakiRintangan    = 120; // Jarak ayun kaki saat rintangan
int ankleKananRintangan  = 200; // Override ankle kanan (servo 18) saat rintangan
int ankleKiriRintangan   = 900; // Override ankle kiri (servo 15) saat rintangan

// --- Langkah Puing ---
int angkatLututPuing  = 200;    // Tinggi angkat lutut saat puing
int ayunKakiPuing     = 200;    // Jarak ayun kaki saat puing
int offsetHipPuing    = 100;    // Offset tambahan hip/ankle untuk clearance

// --- Batas PID ---
int batasPIDMax = 130;          // Batas atas output PID (R dan L)
int batasPIDMin = 0;            // Batas bawah output PID (R dan L)


// =============================================
//   FUNGSI LANGKAH DINAMIS (Dynamixel.move)
// =============================================
// angkat  = offset angkat lutut
// ayun    = offset ayun kaki ke depan
// hipOfs  = offset tambahan hip/ankle (0 jika tidak perlu)
// s18     = nilai servo 18 (ankle kanan)
// s15     = nilai servo 15 (ankle kiri)
// tunda   = delay antar fase (ms)

void Langkah(int angkat, int ayun, int hipOfs, int s18, int s15, int tunda) {
//  - Kaki Kanan                                                                                    + Kaki Kiri
    // Fase 1: Angkat lutut kiri + geser berat badan
    Dynamixel.move(10, defaultServo10);                Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7);                  Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12);                Dynamixel.move(18, s18);                      Dynamixel.move(9, defaultServo9 + angkat);         Dynamixel.move(15, s15 - hipOfs);
    Dynamixel.move(4, defaultServo4);                  Dynamixel.move(16, defaultServo16);           Dynamixel.move(1, defaultServo1 - L);              Dynamixel.move(13, defaultServo13);
    Dynamixel.move(6, defaultServo6);                  Dynamixel.move(18, s18);                      Dynamixel.move(3, defaultServo3 + L);              Dynamixel.move(15, s15 - hipOfs);
    delay(tunda);
//
    // Fase 2: Kembali ke posisi netral
    Dynamixel.move(10, defaultServo10);                Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7);                  Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12);                Dynamixel.move(18, s18);                      Dynamixel.move(9, defaultServo9);                  Dynamixel.move(15, s15);
    delay(tunda);
//
    // Fase 3: Ayun kaki kiri ke depan
    Dynamixel.move(10, defaultServo10);                Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 + ayun);           Dynamixel.move(13, defaultServo13 - hipOfs);
    Dynamixel.move(12, defaultServo12);                Dynamixel.move(18, s18);                      Dynamixel.move(9, defaultServo9);                  Dynamixel.move(15, s15);
    Dynamixel.move(4, defaultServo4);                  Dynamixel.move(16, defaultServo16);           Dynamixel.move(1, defaultServo1);                  Dynamixel.move(13, defaultServo13 - hipOfs);
    Dynamixel.move(6, defaultServo6);                  Dynamixel.move(18, s18);                      Dynamixel.move(3, defaultServo3 + L);              Dynamixel.move(15, s15);
    delay(tunda);
//
    // Fase 4: Angkat lutut kanan + geser berat badan
    Dynamixel.move(10, defaultServo10);                Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7);                  Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 - angkat);       Dynamixel.move(18, s18 + hipOfs);             Dynamixel.move(9, defaultServo9);                  Dynamixel.move(15, s15);
    Dynamixel.move(4, defaultServo4 + R);              Dynamixel.move(16, defaultServo16);           Dynamixel.move(1, defaultServo1);                  Dynamixel.move(13, defaultServo13);
    Dynamixel.move(6, defaultServo6 - R);              Dynamixel.move(18, s18 + hipOfs);             Dynamixel.move(3, defaultServo3);                  Dynamixel.move(15, s15);
    delay(tunda);
//
    // Fase 5: Kembali ke posisi netral
    Dynamixel.move(10, defaultServo10);                Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7);                  Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12);                Dynamixel.move(18, s18);                      Dynamixel.move(9, defaultServo9);                  Dynamixel.move(15, s15);
    delay(tunda);
//
    // Fase 6: Ayun kaki kanan ke depan
    Dynamixel.move(10, defaultServo10 - ayun);         Dynamixel.move(16, defaultServo16 + hipOfs);  Dynamixel.move(7, defaultServo7);                  Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12);                Dynamixel.move(18, s18);                      Dynamixel.move(9, defaultServo9);                  Dynamixel.move(15, s15);
    Dynamixel.move(4, defaultServo4);                  Dynamixel.move(16, defaultServo16 + hipOfs);  Dynamixel.move(1, defaultServo1);                  Dynamixel.move(13, defaultServo13);
    Dynamixel.move(6, defaultServo6 - R);              Dynamixel.move(18, s18);                      Dynamixel.move(3, defaultServo3);                  Dynamixel.move(15, s15);
    delay(tunda);
}


// =============================================
//   FUNGSI LANGKAH SPEED (Dynamixel.moveSpeed)
// =============================================
// angkat  = offset angkat lutut
// ayun    = offset ayun kaki ke depan
// s18     = nilai servo 18 (ankle kanan)
// s15     = nilai servo 15 (ankle kiri)
// tunda   = delay antar fase (ms)
// spd     = kecepatan servo (speed parameter)

void LangkahSpeed(int angkat, int ayun, int s18, int s15, int tunda, int spd) {
//  - Kaki Kanan                                                                                                  + Kaki Kiri
    // Fase 1: Angkat lutut kiri + geser berat badan
    Dynamixel.moveSpeed(10, BdefaultServo10, spd);     Dynamixel.moveSpeed(16, BdefaultServo16, spd);             Dynamixel.moveSpeed(7, BdefaultServo7, spd);       Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(12, BdefaultServo12, spd);     Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(9, BdefaultServo9 + angkat, spd); Dynamixel.moveSpeed(15, s15, spd);
    Dynamixel.moveSpeed(4, BdefaultServo4, spd);       Dynamixel.moveSpeed(16, BdefaultServo16, spd);            Dynamixel.moveSpeed(1, BdefaultServo1 - L, spd);   Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(6, BdefaultServo6, spd);       Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(3, BdefaultServo3 + L, spd);   Dynamixel.moveSpeed(15, s15, spd);
    delay(tunda);
//
    // Fase 2: Kembali ke posisi netral
    Dynamixel.moveSpeed(10, BdefaultServo10, spd);     Dynamixel.moveSpeed(16, BdefaultServo16, spd);            Dynamixel.moveSpeed(7, BdefaultServo7, spd);       Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(12, BdefaultServo12, spd);     Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(9, BdefaultServo9, spd);       Dynamixel.moveSpeed(15, s15, spd);
    delay(tunda);
//
    // Fase 3: Ayun kaki kiri ke depan
    Dynamixel.moveSpeed(10, BdefaultServo10, spd);     Dynamixel.moveSpeed(16, BdefaultServo16, spd);            Dynamixel.moveSpeed(7, BdefaultServo7 + ayun, spd); Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(12, BdefaultServo12, spd);     Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(9, BdefaultServo9, spd);       Dynamixel.moveSpeed(15, s15, spd);
    Dynamixel.moveSpeed(4, BdefaultServo4, spd);       Dynamixel.moveSpeed(16, BdefaultServo16, spd);            Dynamixel.moveSpeed(1, BdefaultServo1, spd);       Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(6, BdefaultServo6, spd);       Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(3, BdefaultServo3 + L, spd);   Dynamixel.moveSpeed(15, s15, spd);
    delay(tunda);
//
    // Fase 4: Angkat lutut kanan + geser berat badan
    Dynamixel.moveSpeed(10, BdefaultServo10, spd);     Dynamixel.moveSpeed(16, BdefaultServo16, spd);            Dynamixel.moveSpeed(7, BdefaultServo7, spd);       Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(12, BdefaultServo12 - angkat, spd); Dynamixel.moveSpeed(18, s18, spd);                   Dynamixel.moveSpeed(9, BdefaultServo9, spd);       Dynamixel.moveSpeed(15, s15, spd);
    Dynamixel.moveSpeed(4, BdefaultServo4 + R, spd);   Dynamixel.moveSpeed(16, BdefaultServo16, spd);            Dynamixel.moveSpeed(1, BdefaultServo1, spd);       Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(6, BdefaultServo6 - R, spd);   Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(3, BdefaultServo3, spd);       Dynamixel.moveSpeed(15, s15, spd);
    delay(tunda);
//
    // Fase 5: Kembali ke posisi netral
    Dynamixel.moveSpeed(10, BdefaultServo10, spd);     Dynamixel.moveSpeed(16, BdefaultServo16, spd);            Dynamixel.moveSpeed(7, BdefaultServo7, spd);       Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(12, BdefaultServo12, spd);     Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(9, BdefaultServo9, spd);       Dynamixel.moveSpeed(15, s15, spd);
    delay(tunda);
//
    // Fase 6: Ayun kaki kanan ke depan
    Dynamixel.moveSpeed(10, BdefaultServo10 - ayun, spd); Dynamixel.moveSpeed(16, BdefaultServo16, spd);         Dynamixel.moveSpeed(7, BdefaultServo7, spd);       Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(12, BdefaultServo12, spd);     Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(9, BdefaultServo9, spd);       Dynamixel.moveSpeed(15, s15, spd);
    Dynamixel.moveSpeed(4, BdefaultServo4, spd);       Dynamixel.moveSpeed(16, BdefaultServo16, spd);            Dynamixel.moveSpeed(1, BdefaultServo1, spd);       Dynamixel.moveSpeed(13, BdefaultServo13, spd);
    Dynamixel.moveSpeed(6, BdefaultServo6 - R, spd);   Dynamixel.moveSpeed(18, s18, spd);                        Dynamixel.moveSpeed(3, BdefaultServo3, spd);       Dynamixel.moveSpeed(15, s15, spd);
    delay(tunda);
//
    // Fase 7: Settling (kembali stabil tanpa speed control)
    Dynamixel.move(10, BdefaultServo10);               Dynamixel.move(16, BdefaultServo16);                      Dynamixel.move(7, BdefaultServo7);                 Dynamixel.move(13, BdefaultServo13);
    Dynamixel.move(12, BdefaultServo12);               Dynamixel.move(18, s18);                                  Dynamixel.move(9, BdefaultServo9);                 Dynamixel.move(15, s15);
    delay(tunda);
}


// =============================================
//   FUNGSI WRAPPER (Kompatibel dengan kode lama)
// =============================================
//   Panggil fungsi ini seperti biasa, atau panggil
//   Langkah() / LangkahSpeed() langsung dengan
//   parameter custom untuk pola jalan baru.

void MajuPID() {
    Langkah(angkatLutut, ayunKaki, 0, defaultServo18, defaultServo15, delayDynamixel1);
}

void MajuPIDRintangan() {
    LangkahSpeed(angkatLututRintangan, ayunKakiRintangan, ankleKananRintangan, ankleKiriRintangan, delayDynamixel11, speeds);
}

void MajuPIDPuing() {
    Langkah(angkatLututPuing, ayunKakiPuing, offsetHipPuing, defaultServo18, defaultServo15, delayDynamixel1);
}


// =============================================
//   FUNGSI PID
// =============================================

void PIDKanan() {
    eror = sp - UltraKanan();
    sum_eror = sum_eror + eror;
    X = ((kp * eror) + (ki * sum_eror) + (kd * (eror - last_eror)));
    R = kecepatan + X;
    L = kecepatan - X;
    R = constrain(R, batasPIDMin, batasPIDMax);
    L = constrain(L, batasPIDMin, batasPIDMax);
    last_eror = eror;  // Update untuk derivative berikutnya
}

void PIDKiri() {
    eror = sp - UltraKiri();
    sum_eror = sum_eror + eror;
    X = ((kp * eror) + (ki * sum_eror) + (kd * (eror - last_eror)));
    R = kecepatan - X;
    L = kecepatan + X;
    R = constrain(R, batasPIDMin, batasPIDMax);
    L = constrain(L, batasPIDMin, batasPIDMax);
    last_eror = eror;  // Update untuk derivative berikutnya
}
