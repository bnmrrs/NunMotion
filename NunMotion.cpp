/*
    NunMotion.cpp - Library for reading a WiiMotionPlus with attached Nunchuk
    Created by Ben Morris, 2010
    Adapted from Duckhead and Kuckles904's implimentation. http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1248889032/45
*/

#include "WProgram.h"
#include "NunMotion.h"
#include <Wire.h>

NunMotion::NunMotion()
{
}

void NunMotion::begin()
{
    Wire.begin();
    
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH); 
        
    this->_switch_to_wmp();
    this->_wmp_on();
    this->calibrate();  
        
    this->_switch_to_nunchuk();
    this->_nunchuk_init();
}

nunmotion_wmp_readings NunMotion::read_wmp()
{
    nunmotion_wmp_readings wmp;
    wmp = this->_read_wmp();
    
    wmp.yaw -= this->_yaw_zero;
    wmp.pitch -= this->_pitch_zero;
    wmp.roll -= this->_roll_zero;
        
    return wmp;
}

nunmotion_nunchuk_readings NunMotion::read_nunchuk()
{
    this->_switch_to_nunchuk();
    return this->_read_nunchuk();
}

void NunMotion::calibrate()
{
    long y0, p0, r0 = 0;
    const int avg = 100;
    nunmotion_wmp_readings wmp;
        
    for (int i = 0; i < avg; i++) {
        wmp = this->_read_wmp();
            
        y0 += wmp.yaw;
        p0 += wmp.pitch;
        r0 += wmp.roll;
    }
        
    this->_yaw_zero = y0/avg;
    this->_pitch_zero = p0/avg;
    this->_roll_zero = r0/avg;
}

void NunMotion::_switch_to_nunchuk()
{
    if (this->_mode != NUNCHUK) {
        this->_mode = NUNCHUK;
        
        digitalWrite(3, LOW);
        digitalWrite(4, LOW);
        digitalWrite(4, HIGH);
    }
}

void NunMotion::_switch_to_wmp()
{
    if (this->_mode != WMP) {
        this->_mode = WMP;
        
        digitalWrite(3, LOW);
        digitalWrite(4, LOW);
        digitalWrite(3, HIGH);
    }
}

void NunMotion::_nunchuk_init()
{
    // Uses New style init - no longer encrypted so no need to XOR bytes later... might save some cycles
    Wire.beginTransmission (0x52);	// transmit to device 0x52
    Wire.send (0xF0);		// sends memory address
    Wire.send (0x55);		// sends sent a zero.
    Wire.endTransmission ();	// stop transmitting
    
    delay(100);
    
    Wire.beginTransmission (0x52);	// transmit to device 0x52
    Wire.send (0xFB);		// sends memory address
    Wire.send (0x00);		// sends sent a zero.
    Wire.endTransmission ();	// stop transmitting
}

void NunMotion::_send_zero()
{
    Wire.beginTransmission(0x52);	// transmit to device 0x52 (nunchuk)
    Wire.send(0x00);		// sends one byte (nunchuk)
    Wire.endTransmission();	// stop transmitting (nunchuk)
}
    
void NunMotion::_wmp_on()
{
    Wire.beginTransmission(0x53);//WM+ starts out deactivated at address 0x53
    Wire.send(0xfe); //send 0x04 to address 0xFE to activate WM+
    Wire.send(0x04);
    Wire.endTransmission(); //WM+ jumps to address 0x52 and is now active
}
    
void NunMotion::_wmp_off()
{
    Wire.beginTransmission(82);
    Wire.send(0xf0);//address then
    Wire.send(0x55);//command
    Wire.endTransmission();
}
    
nunmotion_wmp_readings NunMotion::_read_wmp()
{
    this->_switch_to_wmp();
    this->_send_zero();
     
    byte data[6];
     
    Wire.requestFrom(0x52, 6); // request six bytes from the WM+
    for (int i = 0; i < 6; i++) {
        data[i] = Wire.receive();
    }

    nunmotion_wmp_readings wmp;
    
    wmp.yaw = ((data[3] >> 2) << 8)+data[0];
    wmp.roll = ((data[4] >> 2) << 8)+data[1];
    wmp.pitch = ((data[5] >> 2) << 8)+data[2];
        
    return wmp;
}
    
nunmotion_nunchuk_readings NunMotion::_read_nunchuk()
{
    byte outbuf[6];
    int cnt = 0;
    this->_switch_to_nunchuk();
        
    Wire.requestFrom(0x52, 6); // request data from the nunchuk
        
    int i = 0;
    while (Wire.available()) { // This could probably be changed to a for
        outbuf[i] = Wire.receive();
        i++;
    } 
        
    this->_send_zero();
        
    if (i >= 5) {
        return this->_process_nunchuk(outbuf);
    }
}
    
nunmotion_nunchuk_readings NunMotion::_process_nunchuk(byte outbuf[])
{
    int ax_m = (outbuf[2] * 2 * 2); //Axis X Accelerometer.  Shift 2 bits to the left.
    int ay_m = (outbuf[3] * 2 * 2); //Axis Y Accelerometer.  Shift 2 bits to the left.
    int az_m = (outbuf[4] * 2 * 2); //Axis Z Accelerometer.  Shift 2 bits to the left.
        
    // Byte outbuf[5] contains bits for z and c buttons
    // it also contains the least significant bits for the accerometer data
    // so we have to check each bit of outbuf[5]
    if ((outbuf[5] >> 2) & 1) {
        ax_m += 2;
    }
        
    if ((outbuf[5] >> 3) & 1) {
        ax_m += 1;
    }
        
    if ((outbuf[5] >> 4) & 1) {
        ay_m += 2;
    }
        
    if ((outbuf[5] >> 5) & 1) {
        ay_m += 1;
    }
        
    if ((outbuf[5] >> 6) & 1) {
        az_m += 2;
    }
       
    if ((outbuf[5] >> 7) & 1) {
       az_m += 1;
    }
        
    // The nunchuk accelerometers read ~511 stead state.  Remove that to zero the values.
    ax_m -= 511;
    ay_m -= 511;
    az_m -= 511;
       
    // Translate he raw accelerometer counts into the corresponding degrees and return
    nunmotion_nunchuk_readings nunchuk;
    nunchuk.yaw = atan2(ax_m, ay_m) * 180 / PI;
    nunchuk.pitch = atan2(ay_m, az_m) * 180 / PI;
    nunchuk.roll = atan2(ax_m, az_m) * 180 / PI;
        
    return nunchuk;
}

