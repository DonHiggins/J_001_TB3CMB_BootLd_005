// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     LED.H
//
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef LEDx_H
#define LEDx_H

void led_simpleTimerInit(void);
void led_simpleTimer(void);
void led_simpleHeartbeat(void);
void led_leds(Uint16 ledData);

void GpioU_defaultInit(void);
void GpioU_initGpiosAllZero(void);
void GpioU_DspLedsInit(void);


#endif
