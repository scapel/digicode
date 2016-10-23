#include "types.h"

#ifndef SWITCHLED_H

#define SWITCHLED_H

typedef enum { LEDS_ROUGES = 0, LEDS_BLEUS = 1 } Leds_T;

// les pins 10,11 peunvent faire du pwm, la pin 10 sera reliée aux rouges, la 11 aux leds verte/bleu
uint8 PosLed[2] = { 10,11 };

//void InitSwitchLed (void);
//void SwitchLed ( boolean Etat, boolean Fade, Leds_T Led );
//void SwitchLedPeriod (void);


#endif
