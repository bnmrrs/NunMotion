#include <NunMotion.h>
#include <Wire.h>
/////////////////////////////////////////

NunMotion nunmotion;

void setup()
{
  Serial.begin(115200);
  nunmotion.begin();
}

void loop()
{
    // Read from WiiMotionPlus
    nunmotion_wmp_readings wmp;
    wmp = nunmotion.read_wmp();
    
    Serial.print("pitch: ");
    Serial.println(wmp.pitch);
    
    Serial.print("yaw: ");
    Serial.println(wmp.yaw);
    
    Serial.print("roll: ");
    Serial.println(wmp.roll);
    
    // Read from nunchuk
    nunmotion_nunchuk_readings nunchuk;
    nunchuk = nunmotion.read_nunchuk();
    
    Serial.print("nun pitch: ");
    Serial.println(nunchuk.pitch);
    
    Serial.print("nun yaw: ");
    Serial.println(nunchuk.yaw);
    
    Serial.print("nun roll: ");
    Serial.println(nunchuk.roll);
    
    delay(100); 
}
