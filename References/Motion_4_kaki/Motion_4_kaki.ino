//kanan
// Kaki 4 ==============================================================================================================================================================================
//======================================================================================================================================================================================
//========================================================================================================================================================================================

void Maju(){
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 + 100 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 - 100 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3 + 100 ); Dynamixel.move(15, defaultServo15 + 90);               
    delay(delayDynamixel1);
//    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
////// masalah
    Dynamixel.move(10, defaultServo10); Dynamixel.move(16, defaultServo16);            Dynamixel.move(7, defaultServo7 + 100 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3 + 100 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 - 90 ); Dynamixel.move(18, defaultServo18);     Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 + 110 ); Dynamixel.move(16, defaultServo16);       Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 - 100 ); Dynamixel.move(18, defaultServo18 - 80);       Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
//////    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, defaultServo10 - 90 ); Dynamixel.move(16, defaultServo16);     Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);            Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 - 110 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
}

void SetMaju(){ //KAKI BELAKANG DISERET
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.moveSpeed(10, defaultServo10 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);           Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    Dynamixel.moveSpeed(4, defaultServo4 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);             Dynamixel.moveSpeed(1, defaultServo1 - 100 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);               
    Dynamixel.moveSpeed(6, defaultServo6 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);             Dynamixel.moveSpeed(3, defaultServo3 + 110 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);               
    delay(delayDynamixel1);
//     
    Dynamixel.moveSpeed(10, defaultServo10 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);           Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    delay(delayDynamixel1);
////// masalah
    Dynamixel.moveSpeed(10, defaultServo10, SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);            Dynamixel.moveSpeed(7, defaultServo7 + 80 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    Dynamixel.moveSpeed(4, defaultServo4 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);             Dynamixel.moveSpeed(1, defaultServo1 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);               
    Dynamixel.moveSpeed(6, defaultServo6 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);             Dynamixel.moveSpeed(3, defaultServo3 + 110 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);               
    delay(delayDynamixel1);
//////
    Dynamixel.moveSpeed(10, defaultServo10 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);           Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);      Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    Dynamixel.moveSpeed(4, defaultServo4 + 150 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);       Dynamixel.moveSpeed(1, defaultServo1 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);               
    Dynamixel.moveSpeed(6, defaultServo6 - 110 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);       Dynamixel.moveSpeed(3, defaultServo3 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);               
    delay(delayDynamixel1);
//////    
    Dynamixel.moveSpeed(10, defaultServo10 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);           Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    delay(delayDynamixel1);
//////
    Dynamixel.moveSpeed(10, defaultServo10 - 80 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);     Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    Dynamixel.moveSpeed(4, defaultServo4  , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);            Dynamixel.moveSpeed(1, defaultServo1 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);               
    Dynamixel.moveSpeed(6, defaultServo6 - 110 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);       Dynamixel.moveSpeed(3, defaultServo3 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);               
    delay(delayDynamixel1);
}

void SetMaju2(){ // KAKI BELAKANG NAIK DIKIT
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.moveSpeed(10, defaultServo10 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);           Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 + 30, SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    Dynamixel.moveSpeed(4, defaultServo4 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);             Dynamixel.moveSpeed(1, defaultServo1 - 100 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);               
    Dynamixel.moveSpeed(6, defaultServo6 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);             Dynamixel.moveSpeed(3, defaultServo3 + 110 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);               
    delay(delayDynamixel1);

    Dynamixel.moveSpeed(10, defaultServo10 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);           Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    delay(delayDynamixel1);

    Dynamixel.moveSpeed(10, defaultServo10, SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);            Dynamixel.moveSpeed(7, defaultServo7 + 80 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    Dynamixel.moveSpeed(4, defaultServo4 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);             Dynamixel.moveSpeed(1, defaultServo1 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);               
    Dynamixel.moveSpeed(6, defaultServo6 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);             Dynamixel.moveSpeed(3, defaultServo3 + 110 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);               
    delay(delayDynamixel1);

    Dynamixel.moveSpeed(10, defaultServo10 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);           Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12 - 30, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);      Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    Dynamixel.moveSpeed(4, defaultServo4 + 150 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);       Dynamixel.moveSpeed(1, defaultServo1 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);               
    Dynamixel.moveSpeed(6, defaultServo6 - 110 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);       Dynamixel.moveSpeed(3, defaultServo3 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);               
    delay(delayDynamixel1);

    Dynamixel.moveSpeed(10, defaultServo10 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);           Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    delay(delayDynamixel1);

    Dynamixel.moveSpeed(10, defaultServo10 - 80 , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);     Dynamixel.moveSpeed(7, defaultServo7 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);
    Dynamixel.moveSpeed(12, defaultServo12, SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);            Dynamixel.moveSpeed(9, defaultServo9 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);
    Dynamixel.moveSpeed(4, defaultServo4  , SetSpeed); Dynamixel.moveSpeed(16, defaultServo16, SetSpeed);            Dynamixel.moveSpeed(1, defaultServo1 , SetSpeed); Dynamixel.moveSpeed(13, defaultServo13, SetSpeed);               
    Dynamixel.moveSpeed(6, defaultServo6 - 110 , SetSpeed); Dynamixel.moveSpeed(18, defaultServo18, SetSpeed);       Dynamixel.moveSpeed(3, defaultServo3 , SetSpeed); Dynamixel.moveSpeed(15, defaultServo15, SetSpeed);               
    delay(delayDynamixel1);
}

void MajuRintangan(){
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.moveSpeed(10, RdefaultServo10, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 + 170, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);             Dynamixel.moveSpeed(1, RdefaultServo1 - 100, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);             Dynamixel.moveSpeed(3, RdefaultServo3 + 100, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//    
    Dynamixel.moveSpeed(10, RdefaultServo10, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    delay(delayDynamixel3);
////// masalah
    Dynamixel.moveSpeed(10, RdefaultServo10, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);            Dynamixel.moveSpeed(7, RdefaultServo7 + 150, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);             Dynamixel.moveSpeed(1, RdefaultServo1, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);             Dynamixel.moveSpeed(3, RdefaultServo3 + 100, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//////
    Dynamixel.moveSpeed(10, RdefaultServo10, speeds); Dynamixel.moveSpeed(16, defaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12 - 170, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);     Dynamixel.moveSpeed(9, RdefaultServo9, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4 + 110, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);       Dynamixel.moveSpeed(1, RdefaultServo1, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 - 110, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);       Dynamixel.moveSpeed(3, RdefaultServo3, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//////    
    Dynamixel.moveSpeed(10, RdefaultServo10, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    delay(delayDynamixel3);
//////
    Dynamixel.moveSpeed(10, RdefaultServo10 - 170, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);     Dynamixel.moveSpeed(7, RdefaultServo7, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);            Dynamixel.moveSpeed(1, RdefaultServo1, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 - 110, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);       Dynamixel.moveSpeed(3, RdefaultServo3, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
}

void MajuMiring(){
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 + 150 ); Dynamixel.move(15, defaultServo15 +80);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 - 100 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3 + 100 ); Dynamixel.move(15, defaultServo15 +80);               
    delay(delayDynamixel1);
//    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
////// masalah
    Dynamixel.move(10, defaultServo10); Dynamixel.move(16, defaultServo16);            Dynamixel.move(7, defaultServo7 + 150 ); Dynamixel.move(13, defaultServo13 +80);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13 +80);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3 + 100 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 - 150 ); Dynamixel.move(18, defaultServo18 -100);     Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 + 110 ); Dynamixel.move(16, defaultServo16);       Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 - 110 ); Dynamixel.move(18, defaultServo18 -100);       Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
//////    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, defaultServo10 - 150 ); Dynamixel.move(16, defaultServo16 -100);     Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16 -100);            Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 - 110 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
}


void Mundur(){
 // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, defaultServo10 - 100); Dynamixel.move(16, defaultServo16);        Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(9, defaultServo9); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 + 100); Dynamixel.move(16, defaultServo16);          Dynamixel.move(1, defaultServo1); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 - 100); Dynamixel.move(18, defaultServo18);          Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);              Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 - 100); Dynamixel.move(18, defaultServo18);        Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 + 100); Dynamixel.move(16, defaultServo16);          Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);               Dynamixel.move(3, defaultServo3); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(7, defaultServo7 + 100); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12  ); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);              Dynamixel.move(1, defaultServo1 - 100 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6  ); Dynamixel.move(18, defaultServo18);              Dynamixel.move(3, defaultServo3 + 100); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////////    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);              Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
////////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);            Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 + 100); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 - 100); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);              Dynamixel.move(3, defaultServo3  ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
  
}

void MundurRintangan(){
 // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.moveSpeed(10, RdefaultServo10 - 100, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);        Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);             Dynamixel.moveSpeed(9, RdefaultServo9, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4 + 100, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);          Dynamixel.moveSpeed(1, RdefaultServo1, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 - 100, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);          Dynamixel.moveSpeed(3, RdefaultServo3 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
////    
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);             Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);              Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    delay(delayDynamixel3);
////////
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);             Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12 - 100, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);        Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed( 4, RdefaultServo4 + 100, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);          Dynamixel.moveSpeed(1, RdefaultServo1 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);               Dynamixel.moveSpeed(3, RdefaultServo3, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
////////
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);             Dynamixel.moveSpeed(7, RdefaultServo7 + 100, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12  , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4  , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);              Dynamixel.moveSpeed(1, RdefaultServo1 - 100 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6  , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);              Dynamixel.moveSpeed(3, RdefaultServo3 + 100, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
////////    
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);             Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);              Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    delay(delayDynamixel3);
////////
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);            Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 + 100, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4  , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);             Dynamixel.moveSpeed(1, RdefaultServo1 - 100, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);              Dynamixel.moveSpeed(3, RdefaultServo3  , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
  
}

void Kiri(){
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 + 100 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3 - 150 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10); Dynamixel.move(16, defaultServo16);            Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 - 100 ); Dynamixel.move(18, defaultServo18);     Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 - 150 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 - 150); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, defaultServo10 - 150 ); Dynamixel.move(16, defaultServo16);     Dynamixel.move(7, defaultServo7  ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);           Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 - 150 ); Dynamixel.move(16, defaultServo16);       Dynamixel.move(1, defaultServo1  ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 - 150 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 - 150); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);     Dynamixel.move(7, defaultServo7 + 100); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);           Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 - 150 ); Dynamixel.move(16, defaultServo16);       Dynamixel.move(1, defaultServo1 - 150); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 - 150 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 - 150); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//
    Dynamixel.move(10, defaultServo10  ); Dynamixel.move(16, defaultServo16);          Dynamixel.move(7, defaultServo7 + 100 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);           Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);            Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6  ); Dynamixel.move(18, defaultServo18);            Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////
}

void KiriRintangan(){
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 + 100 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 ); Dynamixel.move(16, RdefaultServo16);             Dynamixel.move(1, RdefaultServo1 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6,RdefaultServo6 ); Dynamixel.move(18, RdefaultServo18);             Dynamixel.move(3, RdefaultServo3 - 150 ); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
    
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
///
    Dynamixel.move(10, RdefaultServo10); Dynamixel.move(16, RdefaultServo16);            Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 - 100 ); Dynamixel.move(18, RdefaultServo18);     Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 ); Dynamixel.move(16, RdefaultServo16);             Dynamixel.move(1, RdefaultServo1 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 - 150 ); Dynamixel.move(18, RdefaultServo18);       Dynamixel.move(3, RdefaultServo3 - 150); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, RdefaultServo10 - 150 ); Dynamixel.move(16, RdefaultServo16);     Dynamixel.move(7, RdefaultServo7  ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 ); Dynamixel.move(18, RdefaultServo18);           Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 - 150 ); Dynamixel.move(16, RdefaultServo16);       Dynamixel.move(1, RdefaultServo1  ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 - 150 ); Dynamixel.move(18, RdefaultServo18);       Dynamixel.move(3, RdefaultServo3 - 150); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);     Dynamixel.move(7, RdefaultServo7 + 100); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 ); Dynamixel.move(18, RdefaultServo18);           Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 - 150 ); Dynamixel.move(16, RdefaultServo16);       Dynamixel.move(1, RdefaultServo1 -150  ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 - 150 ); Dynamixel.move(18, RdefaultServo18);       Dynamixel.move(3, RdefaultServo3 - 150); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, RdefaultServo10  ); Dynamixel.move(16, RdefaultServo16);          Dynamixel.move(7, RdefaultServo7 + 100 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 ); Dynamixel.move(18, RdefaultServo18);           Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4  ); Dynamixel.move(16, RdefaultServo16);            Dynamixel.move(1, RdefaultServo1 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6  ); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(3, RdefaultServo3 ); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
//////
}

void Kanan(){
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 + 100 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3 + 170 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1); 
    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10); Dynamixel.move(16, defaultServo16);            Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 - 85 ); Dynamixel.move(18, defaultServo18);     Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 + 170 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 + 170); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, defaultServo10 - 80 ); Dynamixel.move(16, defaultServo16);     Dynamixel.move(7, defaultServo7  ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);           Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 + 170 ); Dynamixel.move(16, defaultServo16);       Dynamixel.move(1, defaultServo1  ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 + 170 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 + 170); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////  
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);      Dynamixel.move(7, defaultServo7 + 100); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);           Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 + 170 ); Dynamixel.move(16, defaultServo16);       Dynamixel.move(1, defaultServo1 + 170); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 + 170 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 + 170); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//
    Dynamixel.move(10, defaultServo10  ); Dynamixel.move(16, defaultServo16);          Dynamixel.move(7, defaultServo7 + 100 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);           Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);            Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6  ); Dynamixel.move(18, defaultServo18);            Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////
}

void KananRintangan(){
  // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 + 100 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 ); Dynamixel.move(16, RdefaultServo16);             Dynamixel.move(1, RdefaultServo1 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 ); Dynamixel.move(18, RdefaultServo18);             Dynamixel.move(3, RdefaultServo3 + 150 ); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
    
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10); Dynamixel.move(16, RdefaultServo16);            Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 - 100 ); Dynamixel.move(18, RdefaultServo18);     Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 ); Dynamixel.move(16, RdefaultServo16);             Dynamixel.move(1, RdefaultServo1 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 + 150 ); Dynamixel.move(18, RdefaultServo18);       Dynamixel.move(3, RdefaultServo3 + 150); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, RdefaultServo10 - 150 ); Dynamixel.move(16, RdefaultServo16);     Dynamixel.move(7, RdefaultServo7  ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 ); Dynamixel.move(18, RdefaultServo18);           Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 + 150 ); Dynamixel.move(16, RdefaultServo16);       Dynamixel.move(1, RdefaultServo1  ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 + 150 ); Dynamixel.move(18, RdefaultServo18);       Dynamixel.move(3, RdefaultServo3 + 150); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, RdefaultServo10  ); Dynamixel.move(16, RdefaultServo16);     Dynamixel.move(7, RdefaultServo7 + 100); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 ); Dynamixel.move(18, RdefaultServo18);           Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 + 150 ); Dynamixel.move(16, RdefaultServo16);       Dynamixel.move(1, RdefaultServo1 + 150); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 + 150 ); Dynamixel.move(18, RdefaultServo18);       Dynamixel.move(3, RdefaultServo3 + 150); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
//////
    Dynamixel.move(10, RdefaultServo10  ); Dynamixel.move(16, RdefaultServo16);          Dynamixel.move(7, RdefaultServo7 + 100 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 ); Dynamixel.move(18, RdefaultServo18);           Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4  ); Dynamixel.move(16, RdefaultServo16);            Dynamixel.move(1, RdefaultServo1 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6  ); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(3, RdefaultServo3 ); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
//////
}

void GeserKanan(){

 // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, defaultServo7 + 100); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9  ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 - 110); Dynamixel.move(16, defaultServo16);        Dynamixel.move(1, defaultServo1 + 100 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
//    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
/////
    Dynamixel.move(10, defaultServo10 - 100); Dynamixel.move(16, defaultServo16);      Dynamixel.move(7, defaultServo7  ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 ); Dynamixel.move(16, defaultServo16);             Dynamixel.move(1, defaultServo1 + 100); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3  ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12  ); Dynamixel.move(18, defaultServo18);          Dynamixel.move(9, defaultServo9 + 100); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);            Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 + 110 ); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 - 100); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
////////    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
////////
    Dynamixel.move(10, defaultServo10  ); Dynamixel.move(16, defaultServo16);          Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 - 110); Dynamixel.move(18, defaultServo18);      Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);            Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6  ); Dynamixel.move(18, defaultServo18);            Dynamixel.move(3, defaultServo3 - 100); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
}

void GeserKananRintangan(){

 // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 + 100); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9  ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 - 110); Dynamixel.move(16, RdefaultServo16);        Dynamixel.move(1, RdefaultServo1 + 100 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 ); Dynamixel.move(18, RdefaultServo18);             Dynamixel.move(3, RdefaultServo3 ); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
//    
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
///////
    Dynamixel.move(10, RdefaultServo10 - 100); Dynamixel.move(16, RdefaultServo16);      Dynamixel.move(7, RdefaultServo7  ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4 ); Dynamixel.move(16, RdefaultServo16);             Dynamixel.move(1, RdefaultServo1 + 100); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 ); Dynamixel.move(18, RdefaultServo18);             Dynamixel.move(3, RdefaultServo3  ); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////////
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12  ); Dynamixel.move(18, RdefaultServo18);          Dynamixel.move(9, RdefaultServo9 + 100); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4  ); Dynamixel.move(16, RdefaultServo16);            Dynamixel.move(1, RdefaultServo1 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6 + 110 ); Dynamixel.move(18, RdefaultServo18);       Dynamixel.move(3, RdefaultServo3 - 100); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
////////    
    Dynamixel.move(10, RdefaultServo10 ); Dynamixel.move(16, RdefaultServo16);           Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    delay(delayDynamixel1);
////////
    Dynamixel.move(10, RdefaultServo10  ); Dynamixel.move(16, RdefaultServo16);          Dynamixel.move(7, RdefaultServo7 ); Dynamixel.move(13, RdefaultServo13);
    Dynamixel.move(12, RdefaultServo12 - 110); Dynamixel.move(18, RdefaultServo18);      Dynamixel.move(9, RdefaultServo9 ); Dynamixel.move(15, RdefaultServo15);
    Dynamixel.move(4, RdefaultServo4  ); Dynamixel.move(16, RdefaultServo16);            Dynamixel.move(1, RdefaultServo1 ); Dynamixel.move(13, RdefaultServo13);               
    Dynamixel.move(6, RdefaultServo6  ); Dynamixel.move(18, RdefaultServo18);            Dynamixel.move(3, RdefaultServo3 - 100); Dynamixel.move(15, RdefaultServo15);               
    delay(delayDynamixel1);
}

void GeserKiri(){

 // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.move(10, defaultServo10 - 100); Dynamixel.move(16, defaultServo16);      Dynamixel.move(7, defaultServo7  ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9  ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 - 110); Dynamixel.move(16, defaultServo16);        Dynamixel.move(1, defaultServo1  + 100); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
//    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 + 100 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4 - 110 ); Dynamixel.move(16, defaultServo16);       Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6 ); Dynamixel.move(18, defaultServo18);             Dynamixel.move(3, defaultServo3  ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
//////////
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 - 100 ); Dynamixel.move(18, defaultServo18);     Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);            Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6  + 110); Dynamixel.move(18, defaultServo18);       Dynamixel.move(3, defaultServo3 - 100); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
//////////    
    Dynamixel.move(10, defaultServo10 ); Dynamixel.move(16, defaultServo16);           Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12); Dynamixel.move(18, defaultServo18);            Dynamixel.move(9, defaultServo9 ); Dynamixel.move(15, defaultServo15);
    delay(delayDynamixel1);
//////////
    Dynamixel.move(10, defaultServo10  ); Dynamixel.move(16, defaultServo16);          Dynamixel.move(7, defaultServo7 ); Dynamixel.move(13, defaultServo13);
    Dynamixel.move(12, defaultServo12 ); Dynamixel.move(18, defaultServo18);           Dynamixel.move(9, defaultServo9 + 100); Dynamixel.move(15, defaultServo15);
    Dynamixel.move(4, defaultServo4  ); Dynamixel.move(16, defaultServo16);            Dynamixel.move(1, defaultServo1 ); Dynamixel.move(13, defaultServo13);               
    Dynamixel.move(6, defaultServo6  + 110 ); Dynamixel.move(18, defaultServo18);      Dynamixel.move(3, defaultServo3 ); Dynamixel.move(15, defaultServo15);               
    delay(delayDynamixel1);
}

void GeserKiriRintangan(){

 // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.moveSpeed(10, RdefaultServo10 - 130, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);      Dynamixel.moveSpeed(7, RdefaultServo7  , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9  , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4 - 110, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);        Dynamixel.moveSpeed(1, RdefaultServo1  + 100, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);             Dynamixel.moveSpeed(3, RdefaultServo3 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//    
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    delay(delayDynamixel3);
//////////
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7 + 150 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4 - 110 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);       Dynamixel.moveSpeed(1, RdefaultServo1 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);             Dynamixel.moveSpeed(3, RdefaultServo3  , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//////////
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12 - 120 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);     Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4  , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);            Dynamixel.moveSpeed(1, RdefaultServo1 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6  + 110, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);       Dynamixel.moveSpeed(3, RdefaultServo3 - 100, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//////////    
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    delay(delayDynamixel3);
//////////
    Dynamixel.moveSpeed(10, RdefaultServo10  , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);          Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);           Dynamixel.moveSpeed(9, RdefaultServo9 + 270, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4  , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);            Dynamixel.moveSpeed(1, RdefaultServo1 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6  + 110 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);      Dynamixel.moveSpeed(3, RdefaultServo3 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
}

void GeserKiriRintangan2(){

 // - Maju       Kaki Kanan                                                                     + Maju     Kaki Kiri
    Dynamixel.moveSpeed(10, RdefaultServo10 - 100, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);      Dynamixel.moveSpeed(7, RdefaultServo7  , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9  , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4 - 100, speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);        Dynamixel.moveSpeed(1, RdefaultServo1  + 100, speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);             Dynamixel.moveSpeed(3, RdefaultServo3 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//    
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    delay(delayDynamixel3);
////////// masalah
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7 + 150 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4 - 100 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);       Dynamixel.moveSpeed(1, RdefaultServo1 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);             Dynamixel.moveSpeed(3, RdefaultServo3  , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//////////
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12 - 90 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);     Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4  , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);            Dynamixel.moveSpeed(1, RdefaultServo1 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6  + 110, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);       Dynamixel.moveSpeed(3, RdefaultServo3 - 100, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
//////////    
    Dynamixel.moveSpeed(10, RdefaultServo10 , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);           Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12, speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);            Dynamixel.moveSpeed(9, RdefaultServo9 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    delay(delayDynamixel3);
//////////
    Dynamixel.moveSpeed(10, RdefaultServo10  , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);          Dynamixel.moveSpeed(7, RdefaultServo7 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);
    Dynamixel.moveSpeed(12, RdefaultServo12 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);           Dynamixel.moveSpeed(9, RdefaultServo9 + 270, speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);
    Dynamixel.moveSpeed(4, RdefaultServo4  , speeds); Dynamixel.moveSpeed(16, RdefaultServo16, speeds);            Dynamixel.moveSpeed(1, RdefaultServo1 , speeds); Dynamixel.moveSpeed(13, RdefaultServo13, speeds);               
    Dynamixel.moveSpeed(6, RdefaultServo6  + 110 , speeds); Dynamixel.moveSpeed(18, RdefaultServo18, speeds);      Dynamixel.moveSpeed(3, RdefaultServo3 , speeds); Dynamixel.moveSpeed(15, RdefaultServo15, speeds);               
    delay(delayDynamixel3);
}

void dash(){
Dynamixel.move(4, BdefaultServo4); Dynamixel.move(10, BdefaultServo10); Dynamixel.move(16, BdefaultServo16);    Dynamixel.move(1, BdefaultServo1); Dynamixel.move(7, BdefaultServo7); Dynamixel.move(13, BdefaultServo13);
Dynamixel.move(6, BdefaultServo6); Dynamixel.move(12, BdefaultServo12 ); Dynamixel.move(18,BdefaultServo18);    Dynamixel.move(3, BdefaultServo3); Dynamixel.move(9, BdefaultServo9); Dynamixel.move(15, BdefaultServo15);
delay(delayDynamixe2);

Dynamixel.move(4, BdefaultServo4); Dynamixel.move(10, BdefaultServo10); Dynamixel.move(16, BdefaultServo16);    Dynamixel.move(1, BdefaultServo1); Dynamixel.move(7, BdefaultServo7); Dynamixel.move(13, BdefaultServo13);
Dynamixel.move(6, BdefaultServo6); Dynamixel.move(12, BdefaultServo12 ); Dynamixel.move(18,BdefaultServo18);    Dynamixel.move(3, BdefaultServo3); Dynamixel.move(9, BdefaultServo9); Dynamixel.move(15, BdefaultServo15);
delay(delayDynamixe2);
}

void ambil_korban(){        
      servo2.write(90);      
      Dynamixel.moveSpeed(0, 790, 70);  
      delay(1500);
      Maju();            
      delay(2000);
      servo2.write(25);      
      Dynamixel.moveSpeed(0, 500, 70);  
      Mundur();
      delay(1000);  
}

void ambil_korban2(){        
      servo2.write(120);      
      Dynamixel.moveSpeed(0, 790, 70);  
      delay(1500);
      Maju();      
      Mundur();
      delay(2000);
      servo2.write(25);
      Mundur();
      Dynamixel.moveSpeed(0, 500, 70);  
      delay(1000);  
}

void taruh_korban(){
  Dynamixel.moveSpeed(0, 790, 70);
  delay(3000);
  servo2.write(90);
  delay(2500);
  Dynamixel.moveSpeed(0, 500, 70);
  delay(500);
  servo2.write(25);
}
void hadap_kiri(){
  Kiri();
  Kiri();
  Kiri();
  Kiri();
  Kiri();
}

void hadap_kiri2(){
  Kiri();
  Kiri();
  Kiri();
  Kiri();
}

void hadap_kanan(){
  Kanan();
  Kanan();
  Kanan();
  Kanan();
}

void hadap_kanan2(){
  Kanan();
  Kanan();
  Kanan();
  Kanan();
  Kanan();
}

void hadap_kanan_rintangan(){
  KananRintangan();
  KananRintangan();
  KananRintangan();
  KananRintangan();
}

void hadap_kanan_rintangan2(){
  KananRintangan();
  delay(500);
  KananRintangan();
  delay(500);
  KananRintangan();
  delay(500);
  KananRintangan();
  delay(500);
}

void hadap_kiri_rintangan(){
  KiriRintangan();
  KiriRintangan();
  KiriRintangan();
  KiriRintangan();
  KiriRintangan();
}

void increasespeed(){
  Dynamixel.moveSpeed(1, RdefaultServo1, 500);
  Dynamixel.moveSpeed(3, RdefaultServo3, 500);
  Dynamixel.moveSpeed(4, RdefaultServo4, 500);
  Dynamixel.moveSpeed(6, RdefaultServo6, 500);
  Dynamixel.moveSpeed(7, RdefaultServo7, 500);
  Dynamixel.moveSpeed(9, RdefaultServo9, 500);
  Dynamixel.moveSpeed(10, RdefaultServo10, 500);
  Dynamixel.moveSpeed(12, RdefaultServo12, 500);
  Dynamixel.moveSpeed(13, RdefaultServo13, 500);
  Dynamixel.moveSpeed(15, RdefaultServo15, 500);
  Dynamixel.moveSpeed(16, RdefaultServo16, 500);
  Dynamixel.moveSpeed(18, RdefaultServo18, 500);
  }

  void increasespeed2(){
  Dynamixel.moveSpeed(1, defaultServo1, 300);
  Dynamixel.moveSpeed(3, defaultServo3, 300);
  Dynamixel.moveSpeed(4, defaultServo4, 300);
  Dynamixel.moveSpeed(6, defaultServo6, 300);
  Dynamixel.moveSpeed(7, defaultServo7, 300);
  Dynamixel.moveSpeed(9, defaultServo9, 300);
  Dynamixel.moveSpeed(10, defaultServo10, 300);
  Dynamixel.moveSpeed(12, defaultServo12, 300);
  Dynamixel.moveSpeed(13, defaultServo13, 300);
  Dynamixel.moveSpeed(15, defaultServo15, 300);
  Dynamixel.moveSpeed(16, defaultServo16, 300);
  Dynamixel.moveSpeed(18, defaultServo18, 300);
  }

  void increasespeed3(){
  Dynamixel.moveSpeed(1, RdefaultServo1, 300);
  Dynamixel.moveSpeed(3, RdefaultServo3, 300);
  Dynamixel.moveSpeed(4, RdefaultServo4, 300);
  Dynamixel.moveSpeed(6, RdefaultServo6, 300);
  Dynamixel.moveSpeed(7, RdefaultServo7, 300);
  Dynamixel.moveSpeed(9, RdefaultServo9, 300);
  Dynamixel.moveSpeed(10, RdefaultServo10, 300);
  Dynamixel.moveSpeed(12, RdefaultServo12, 300);
  Dynamixel.moveSpeed(13, RdefaultServo13, 300);
  Dynamixel.moveSpeed(15, RdefaultServo15, 300);
  Dynamixel.moveSpeed(16, RdefaultServo16, 300);
  Dynamixel.moveSpeed(18, RdefaultServo18, 300);
  }
