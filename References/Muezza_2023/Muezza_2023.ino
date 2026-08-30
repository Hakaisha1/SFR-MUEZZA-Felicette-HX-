#include <DynamixelSerial3.h>
#include <SPI.h>
#include <Servo.h>
#include "SRF05.h"
#include <TimerOne.h>
#include <Wire.h>


int speeds = 200 ;//200
int SetSpeed = 500;
int sensor = A12;
int lampu = 48;
int sensor2 = A13;
int lampu2 = 46;
const int relay = 21;
bool off = HIGH;
bool on = LOW;

// DYNAMIXEL SERVO Biasa
const int moveDynamixel = 100;//
const int delayDynamixe2 = 100;
const int delayDynamixel = 1000;//100//80
const int delayDynamixel1 = 50;//100//50
const int delayDynamixel11 = 200;
const int delayDynamixel3 = 200;
const int delayDynamixel111 = 100;

const int defaultServo0 = 770;//770
const int defaultServo1 = 500;//500
const int defaultServo3 = 500;//500
const int defaultServo4 = 500;//500
const int defaultServo6 = 500;//500
const int defaultServo7 = 600;
const int defaultServo9 = 600;
const int defaultServo10 = 360;//450
const int defaultServo12 = 420;//450
const int defaultServo13 = 670;//600//900(-70)
const int defaultServo15 = 600;
const int defaultServo16 = 370;//450
const int defaultServo18 = 420;//450

const int BdefaultServo1 = 500;
const int BdefaultServo3 = 500;
const int BdefaultServo4 = 500;
const int BdefaultServo6 = 500;
const int BdefaultServo7 = 600;
const int BdefaultServo9 = 600;
const int BdefaultServo10 = 450;
const int BdefaultServo12 = 450;
const int BdefaultServo13 = 600;//900(-70)
const int BdefaultServo15 = 600;
const int BdefaultServo16 = 450;
const int BdefaultServo18 = 450;

const int RdefaultServo1 = 500;
const int RdefaultServo3 = 500;
const int RdefaultServo4 = 500;
const int RdefaultServo6 = 500;
const int RdefaultServo7 = 530;
const int RdefaultServo9 = 515;
const int RdefaultServo10 = 475;
const int RdefaultServo12 = 490;
const int RdefaultServo13 = 595;//900(-70)
const int RdefaultServo15 = 500;
const int RdefaultServo16 = 480;
const int RdefaultServo18 = 510;

// GYRO
int RateRoll, RatePitch, RateYaw;
float Yaw = 0;
float Pitch = 0;
float Roll = 0;

// SRF SENSOR
int hitung = 0, PV, s_kanan, s_kiri, P, K;
int i = 1;

//triger , echo
SRF05 kanan(10, 11);
SRF05 depan(8, 9);
SRF05 kiri(2, 3);
SRF05 Belakang(12, 13);
SRF05 penjepit(4, 5);

//Cengkraman
Servo servo2;

//Setup
int kecepatan = 100;
int kp = 2, ki = 0.5, kd = 0.4, eror = 0, last_eror = 0, sum_eror = 0;
int sp = 13, J, O, X; //Sp=60==10
int L = 0;
int R = 0;
int plan = 0;
int ruang = 0;

//setup dari pin yang digunakan di arduino
void setup() {
  servo2.attach(28);
  Dynamixel.begin(1000000, 22);
  Serial.begin(9600);  
  pinMode(relay, OUTPUT);
  digitalWrite(relay, off);
  kanan.setCorrectionFactor(1.035);

  //  servo2.write(25);

  Wire.setClock(400000);
  Wire.begin();
  delay(250);
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  Dynamixel.moveSpeed(0, 500, 70); //500
  servo2.write(25);
}

void stand() {
  Dynamixel.move(1, 500); Dynamixel.move(7, 600); Dynamixel.move(13, 670);
  Dynamixel.move(4, 500); Dynamixel.move(10, 360); Dynamixel.move(16, 370);
  Dynamixel.move(3, 500); Dynamixel.move(9, 600); Dynamixel.move(15, 600);
  Dynamixel.move(6, 500); Dynamixel.move(12, 420); Dynamixel.move(18, 420);
}

void standrintangan() {
  Dynamixel.move(1, 500); Dynamixel.move(7, 520); Dynamixel.move(13, 595); //Kiri Depan
  Dynamixel.move(4, 500); Dynamixel.move(10, 475); Dynamixel.move(16, 480); //Kanan Depan
  Dynamixel.move(3, 500); Dynamixel.move(9, 515); Dynamixel.move(15, 500); //Kiri Belakang
  Dynamixel.move(6, 500); Dynamixel.move(12, 490); Dynamixel.move(18, 510); //Kanan Belakang
}

//PROGRAM UTAMA
void loop() {
  switch (plan)
  {
    // START - AMBIL KORBAN 1
    ///////////////////////////////////////////
    //START
    case 0:
      stand();
      if (UltraBelakang2() > 50) {
        hadap_kanan2();
        hadap_kanan2();
        plan = 1;
      }
      else if (UltraKiri2() > 50) {
        hadap_kiri();
        plan = 1;
      }
      else if (UltraKanan2() > 50 ) {
        hadap_kanan2();
        plan = 1;
      }
      else if (UltraDepan2() > 50) {
        plan = 1;
      }
      break;

    case 1:
      PIDKanan();
      MajuPID();
      if (UltraKiri() > 35) {
        Maju();
        GeserKanan();
        GeserKanan();
        GeserKanan();
        plan = 2;
      }
      break;

    case 2:
      hadap_kiri2();
      Mundur();
      Mundur();
      Mundur();
      plan = 3;
      if (UltraDepan() < 48) {
        GeserKanan();
        GeserKanan();
        plan = 3;
      }
      break;

    //AMBIL KORBAN 1
    case 3:
      Maju();
      ambil_korban();
      Mundur();
      Mundur();
      hadap_kanan2();
      GeserKanan();      
      Maju();
      plan = 4;
      break;
    ///////////////////////////////////////////////////////

    //AMBIL KORBAN 1 - SAFEZONE 1
    ///////////////////////////////////////////////////////
    case 4:
      millis();
      MajuRintangan();
      if (millis() > 32000 && millis() < 43000 && UltraDepan() < 20) {
        hadap_kiri_rintangan();
        MajuRintangan();
        plan = 5;
      }
      if (millis() > 43000) {
        KiriRintangan();
        plan = 5;
      }
      break;

    case 5:
      MajuRintangan();
      if (UltraKiri2() > 60  && UltraDepan() < 25 && UltraDepan() > 15) {
        increasespeed();
        increasespeed();
        KananRintangan();
        KananRintangan();
        MundurRintangan();
        //
        if (UltraDepan() > 35) {
          MajuRintangan();
        }
        else {
          MundurRintangan();
        }
        taruh_korban();
        increasespeed();
        hadap_kiri_rintangan();
        KiriRintangan();
        GeserKananRintangan();
        GeserKananRintangan();
        GeserKananRintangan();
        KiriRintangan();
        plan = 7;
      }
      break;
    //////////////////////////////////////////////////////

    //SAFEZONE 1 - SAFEZONE 2
    //////////////////////////////////////////////////////
    //  case 6:
    //    millis();
    //    MajuRintangan();
    //    if(UltraKanan() < 15 || millis() > 5000){
    //      KiriRintangan();
    //      KiriRintangan();
    //      GeserKananRintangan();
    //      GeserKananRintangan();
    //      plan = 7;
    //    }
    //  break;
    // JALAN KELERENG
    case 7:
      MajuRintangan();
      if (UltraDepan2() < 50 && UltraDepan2() > 40  && UltraKiri2() < 40) {
        increasespeed();
        GeserKananRintangan();
        GeserKananRintangan();
        GeserKananRintangan();
        plan = 8;
      }
      break;

    //AMBIL DAN TARUH KORBAN 2
    case 8:
      KiriRintangan();
      standrintangan();
      delay(500);
      if (UltraDepan2() < 50 && UltraBelakang2() < 30 ) {
        KiriRintangan();
        KiriRintangan();
        MundurRintangan();
        MundurRintangan();
        servo2.write(120);
        Dynamixel.moveSpeed(0, 790, 70);
        delay(1500);
        MajuRintangan();
        MajuRintangan();
        MajuRintangan();
        ambil_korban2();

        increasespeed();
        hadap_kanan_rintangan2();
        KananRintangan();
        MajuRintangan();
        MajuRintangan();
        MajuRintangan();
        increasespeed();
        KananRintangan();
        MundurRintangan();
        taruh_korban();

        increasespeed();
        hadap_kiri_rintangan();
        KiriRintangan();
        MajuRintangan();
        MajuRintangan();
        GeserKananRintangan();
        GeserKananRintangan();  
        plan = 9;
      }
      break;

    //SAFEZONE 2 KE SAFEZONE 3
    //////////////////////////////////////////////
    case 9:
      increasespeed();
      GeserKananRintangan();
      if (UltraKanan() < 15 ) {
        MajuRintangan();
        GeserKananRintangan();
        GeserKananRintangan();
        increasespeed();
        plan = 10;
      }
      break;

    case 10:
      MajuRintangan();
      if (UltraDepan() > 20) {
        GeserKanan();
        GeserKanan();
      }
      if ((UltraDepan2() < 10 || (UltraBelakang2() > 69 && UltraBelakang2() < 73))  && UltraKiri2() > 40) {
        Kiri();
        Kiri();
        GeserKiriRintangan();
        GeserKiriRintangan();
        MundurRintangan();
        GeserKiriRintangan();
        GeserKiriRintangan();
        MundurRintangan();
        increasespeed();
        hadap_kanan();
        Kanan();
        Kanan();
        GeserKiri();
        Mundur();
        Mundur();
        Mundur();
        Mundur();
        Mundur();
        Kiri();
        Dynamixel.moveSpeed(0, 810, 70);
        plan = 11;
      }
      break;

    // JALAN PECAH 2
    case 11:
      Dynamixel.moveSpeed(0, 800, 70);
      GeserKiriRintangan2();
      GeserKiriRintangan2();
      GeserKiriRintangan2();
      GeserKiriRintangan2();
      GeserKiriRintangan2();
      Kanan();
      standrintangan();
      MajuRintangan();
      MajuRintangan();
      servo2.write(70);
      delay(500);
      MajuRintangan();
      delay(500);
      servo2.write(25);
      delay(500);
      Dynamixel.moveSpeed(0, 500, 70);
      delay(1000);

      increasespeed3();
      MundurRintangan();
      MundurRintangan();
      MundurRintangan();
      MundurRintangan();
      MundurRintangan();
      plan = 12;

      break;

    case 12:
      GeserKiriRintangan();
      if (UltraKiri2() < 36 && UltraKiri2 > 30) {
        increasespeed();
        KiriRintangan();
        plan = 13;
      }
      if (UltraKiri2() > 120 && UltraKiri2() < 170 ) {
        increasespeed();
        KiriRintangan();
        KiriRintangan();
        KiriRintangan();
        MajuRintangan();
        MajuRintangan();
        plan = 13;
      }
      break;

    case 13:
      KiriRintangan();
      if (UltraDepan() > 19 && UltraDepan() < 30) {
        KiriRintangan();
        KiriRintangan();
        KiriRintangan();
        KiriRintangan();
        taruh_korban();
        KananRintangan();
        KananRintangan();
        GeserKananRintangan();
        plan = 14;
      }
      break;

    case 14:
      MajuRintangan();
      if (UltraDepan2() > 100) {
        MajuRintangan();
        MajuRintangan();
        MajuRintangan();
        break;
      }
      break;
    ///////////////////////////////////////////////////////////////
    //END


    // TRY AND TESTING
    ///////////////////////////////////////////////////////////////
    case 100:      
        Mundur();
      break;

      ////////////////////////////////////////////////////////////////
  }
}
