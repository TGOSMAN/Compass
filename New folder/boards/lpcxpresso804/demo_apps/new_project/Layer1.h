#ifndef LAYER1_H
#define LAYER1_H

#include <stdint.h>
void initPort(void);
void changeDIR(uint32_t PIN, int direction);
void initPWM(void);
void pwmdutycycle(uint32_t percent);
void decoder(uint32_t );
void delay(uint32_t ms);
void uartinit(void);
//void USART0_DriverIRQHandler(void);
void sendcharuart(uint8_t character);
uint8_t readcharuart(void);
void I2Cinit (void);
void pininterruptsetup(void);
uint8_t I2Csendframeread(uint8_t address, uint8_t register1);
void I2Csendframewrite(uint8_t address, uint8_t register1, uint8_t data);
#endif