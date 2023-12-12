/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "board.h"
#include "fsl_swm.h"
#include "LPC804.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define	CTIMER (0x40038000)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_systickCounter;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    {
    }
}
*/
#define CTIMER_BASE_ADDRESS 0x40038000

// Register Offsets
#define IR_OFFSET    0x00
#define TCR_OFFSET   0x04
#define TC_OFFSET    0x08
#define PR_OFFSET    0x0C
#define PC_OFFSET    0x10
#define MCR_OFFSET   0x14
#define MR0_OFFSET   0x18
#define MR1_OFFSET   0x1C
#define MR2_OFFSET   0x20
#define MR3_OFFSET   0x24
#define CCR_OFFSET   0x28
#define EMR_OFFSET   0x3C
#define PWMC_OFFSET  0x74
#define CTIMER0_CLOCK_ENABLE (1 << 25)
#define SYSAHBCLKCTRL0 (*(volatile unsigned int *)(SYSAHBCLKCTRL0_ADDRESS))
#define PINASSIGN4 	0x4000C010

// Macros for Register Access
#define CTIMER_IR    (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + IR_OFFSET))
#define CTIMER_TCR   (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + TCR_OFFSET))
#define CTIMER_TC    (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + TC_OFFSET))
#define CTIMER_PR    (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + PR_OFFSET))
#define CTIMER_PC    (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + PC_OFFSET))
#define CTIMER_MCR   (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + MCR_OFFSET))
#define CTIMER_MR0   (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + MR0_OFFSET))
#define CTIMER_MR1   (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + MR1_OFFSET))
#define CTIMER_MR2   (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + MR2_OFFSET))
#define CTIMER_MR3   (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + MR3_OFFSET))
#define CTIMER_CCR   (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + CCR_OFFSET))
#define CTIMER_EMR   (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + EMR_OFFSET))
#define CTIMER_PWMC  (*(volatile unsigned int *)(CTIMER_BASE_ADDRESS + PWMC_OFFSET))
#define TOMAT  (*(volatile unsigned int *)(0x4000C010)))
#define SYSAHBCLKCTRL0_ADDRESS 0x40048080

#define SWM_BASE_ADDRESS 0x4000C000

// Macro for PINASSIGN4 Register Access
#define SWM_PINASSIGN4 (*(volatile uint32_t *)(SWM_BASE_ADDRESS + 0x010)) // Offset for PINASSIGN4
volatile int change = 1;





void configureSWM(void) {
    // Assign pin 8 to T0_MAT0 function
    SWM_PINASSIGN4 = (SWM_PINASSIGN4 & ~SWM_PINASSIGN4_T0_MAT0_MASK) | SWM_PINASSIGN4_T0_MAT0(8);
}
/*void assign_MAT0_to_P0_8(void) {
    uint32_t pinassign4 = LPC_SWM->PINASSIGN4; // Read the current value of PINASSIGN4
    pinassign4 &= ~(0xFF << 0); // Clear the bits corresponding to T0_MAT0
    pinassign4 |= (8 << 0);     // Set the pin number for P0_8 in the T0_MAT0 field

    LPC_SWM->PINASSIGN4 = pinassign4; // Write the modified value back to PINASSIGN4
}*/
/*
void pwm_match_callback(uint32_t flags)
{
    static uint32_t count    = 0;
    static uint8_t decrement = 1;

        count = 0;
        /* Update pulse width match value after the PWM duty cycle is changed */
        /*
				CTIMER_UpdatePwmPulsePeriod(CTIMER, CTIMER_MAT_OUT, g_pulsePeriod);
        dutyCycle = dutyCycle + 1 - decrement * 2;
        if (dutyCycle == 69)
        {
            decrement = 0;
        }
        else if (dutyCycle == 99)
        {
            decrement = 1;
        }
    }
}
*/
void initPort(void) { 
	volatile uint32_t *AHBCLK = (volatile uint32_t *) 0x40048080;//AHBCLK Control Register
	volatile uint32_t *PRESET0 = (volatile uint32_t *) 0x40048088;//Port Presets Control Register
	*AHBCLK |= (1<<6); // enable port clock
	*PRESET0 |= (1<<6);
	return;
	//clocks
	//Presets
	
}
void initPWM(void) {
		SYSAHBCLKCTRL0 |= CTIMER0_CLOCK_ENABLE;
		SYSAHBCLKCTRL0 |= (1<<7); //SWM CLOCK ENABLED
    // Configure timer
    CTIMER_TCR = 0x02; // Reset timer
    //CTIMER_TCR = 0x01; // Enable timer

    // Set prescaler (for example, for 1 MHz PWM frequency)
    CTIMER_PR = 100; // Assuming a 12 MHz clock

    // Set PWM cycle (e.g., 1000 for 1 kHz PWM)
    CTIMER_MR3 = 1000;

    // Configure match control register (reset on MR3, interrupt)
    CTIMER_MCR = (1 << 10) | (1 << 9) ;

    // Set duty cycle (e.g., 50%)
    CTIMER_MR0 = 900;

    // Enable PWM on MR0
    CTIMER_PWMC = 0x01;
		
		
    // Start the timer
    CTIMER_TCR = 0x01;
		
}
/*!
 * @brief Main function
 */
int main(void)
{
		uint32_t *poll = 0;
		int change = 0;
		uint32_t pin;
		uint32_t *x = 0;
		uint32_t *value;
		//GPIO_PortInit(GPIO, BOARD_LED_PORT);
		initPort();
		//DIR
		x = (uint32_t *)(GPIO_BASE|0x2000);
		pin = *x;
		pin |= 0x3<<8;
		*x = pin;
		//GPIO output
		x = (uint32_t *)(GPIO_BASE|0x1024);
		pin = *x;
		pin |= (0x1<<9);
		*x = pin;
		//CTIMER_RegisterCallBack(CTIMER, &ctimer_callback[0], kCTIMER_SingleCallback);
		initPWM();
		x = (uint32_t *)(0x4000C010);
		pin = *x;
		pin = 0x8;
		*x &= pin;
		while(1){
		}
    
}
