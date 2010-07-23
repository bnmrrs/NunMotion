/*
    NunMotion.h - Library for reading a WiiMotionPlus with attached Nunchuk
    Created by Ben Morris, 2010
    Adapted from Duckhead and Kuckles904's implimentation. http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1248889032/45
*/

#ifndef NunMotion_h
#define NunMotion_h

#include "WProgram.h"


enum NUNMOTION_CURR_TYPE { NUNCHUK, WMP };

struct nunmotion_wmp_readings {
    long yaw;
    long pitch;
    long roll;
};

struct nunmotion_nunchuk_readings {
    float yaw;
    float pitch;
    float roll;
};

class NunMotion {
public:    
    NunMotion(void);
    void begin();
    void calibrate();
    nunmotion_wmp_readings read_wmp();    
    nunmotion_nunchuk_readings read_nunchuk();    
    
protected:    
    long _yaw_zero;
    long _pitch_zero;
    long _roll_zero;
    enum NUNMOTION_CURR_TYPE _mode;
    
    void _switch_to_nunchuk();
    void _switch_to_wmp();    
    void _nunchuk_init();    
    void _send_zero();    
    void _wmp_on();    
    void _wmp_off();    
    nunmotion_wmp_readings _read_wmp();    
    nunmotion_nunchuk_readings _read_nunchuk();    
    nunmotion_nunchuk_readings _process_nunchuk(byte outbuf[]);
};

#endif
