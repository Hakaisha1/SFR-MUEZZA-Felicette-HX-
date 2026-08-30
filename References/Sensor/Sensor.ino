 void BacaSensor(){    
    if(i % 15 == 0){
      Serial.println("");
      Serial.print("Depan | Belakang | Kanan | Kiri | Penjepit");
      i++;
    }
    else{
      i++;
    }
    stand();        
    Serial.println("");
    Serial.print("  ");
    Serial.print(UltraDepan2());
    Serial.print("   |    ");
    Serial.print(UltraBelakang2());
    Serial.print("    |  ");    
    Serial.print(UltraKanan2());
    Serial.print("   |  ");
    Serial.print(UltraKiri2());
    Serial.print("  |    ");
    Serial.print(UltraPenjepit());
    delay(100);
}

int UltraKanan(){
   hitung = kanan.getCentimeter();
    if( hitung>50){ hitung=50;}
    if( hitung<0){ hitung=0;}
    return hitung;
}

int UltraDepan(){
   hitung = depan.getCentimeter();
    if( hitung>50){ hitung=50;}
    if( hitung<0){ hitung=0;}
    return hitung;
}

int UltraKiri(){
   hitung = kiri.getCentimeter();
    if( hitung>50){ hitung=50;}
    if( hitung<0){ hitung=0;}
    return hitung;
}

int UltraBelakang(){
   hitung = Belakang.getCentimeter();
    if( hitung>50){ hitung=50;}
    if( hitung<0){ hitung=0;}
    return hitung;
}

int UltraPenjepit(){
   hitung = penjepit.getCentimeter();
    if( hitung>50){ hitung=50;}
    if( hitung<0){ hitung=0;}
    return hitung;
}

int UltraKanan2(){
   hitung = kanan.getCentimeter();  
    return hitung;
}

int UltraDepan2(){
   hitung = depan.getCentimeter();    
    return hitung;
}

int UltraKiri2(){
   hitung = kiri.getCentimeter();    
    return hitung;
}

int UltraBelakang2(){
   hitung = Belakang.getCentimeter();   
    return hitung;
}

int garis(){
  hitung = analogRead(sensor);
  return hitung ;
}

int garis2(){
  hitung = analogRead(sensor2);
  return hitung ;
}

void wire(void){
    Wire.beginTransmission(0x68);
    Wire.write(0x1A);
    Wire.write(0x05);
    Wire.endTransmission(); 
    Wire.beginTransmission(0x68);
    Wire.write(0x1B); 
    Wire.write(0x8); 
    Wire.endTransmission(); 
    Wire.beginTransmission(0x68);
    Wire.write(0x43);
    Wire.endTransmission();
    Wire.requestFrom(0x68,6);
 }

 float gyro_X(){    
    int16_t GyroX=Wire.read()<<8 | Wire.read();
    RateRoll=(int)GyroX/65.5;  
    if(RateRoll > 0){
      Roll += 0.1;
    }
    else if(RateRoll < 0){
      Roll -= 0.1;
    }
    else if(RateRoll == 0){
      Roll += 0;
    }
    return Roll;      
 }

 float gyro_Y(){
    int16_t GyroY=Wire.read()<<8 | Wire.read();
    RatePitch=(int)GyroY/65.5; 
    if(RatePitch > 0){
      Pitch += 0.1;
    }
    else if(RatePitch < 0){
      Pitch -= 0.1;
    }
    else if(RatePitch == 0){
      Pitch += 0;
    }
    return Pitch;
 }

 float gyro_Z(){
    int16_t GyroZ=Wire.read()<<8 | Wire.read();        
    RateYaw=(int)GyroZ/1000;
    if(RateYaw > 0){
      Yaw += 0.1;
    }
    else if(RateYaw < 0){
      Yaw -= 0.1;
    }
    else if(RateYaw == 0){
      Yaw += 0;
    }
    return Yaw;
 }

 void BacaGyro(){
  Serial.print("Roll rate [°/s]= ");
  Serial.print(gyro_X());
  Serial.print(" Pitch Rate [°/s]= ");
  Serial.print(gyro_Y());
  Serial.print(" Yaw Rate [°/s]= ");
  Serial.println(gyro_Z());
  delay(50);
 }
