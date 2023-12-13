#ifndef LAYER1_H
#define LAYER1_H

#include <stdint.h>
void initPort(void);
void changeDIR(uint32_t PIN, int direction);
void initPWM(void);
void pwmdutycycle(uint32_t percent);
void decoder(uint8_t value);

#endif