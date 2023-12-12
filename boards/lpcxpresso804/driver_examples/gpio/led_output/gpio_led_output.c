#include <stdint.h>
#include <stdlib.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define	CTIMER  (0x40038000)
#define GPIO    (0xA0000000)
#define SYSCON  (0x40048000)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/

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

/////////////////////////////Brief/////////////////////////////
/*
Description:
This Function initialises the GPIO Port by:
    -   Turns on the AHB clock to the GPIO port
    -   Clears the Preset on the GPIO port to operate
Parameters:
-   None
*/
void initPort(void) { 
	volatile uint32_t *AHBCLK = (volatile uint32_t *) (SYSCON+0x80);//AHBCLK Control Register
	volatile uint32_t *PRESET0 = (volatile uint32_t *) (SYSCON+0x88);//Port Presets Control Register
	*AHBCLK |= (1<<6); // enable port clock
	*PRESET0 |= (1<<6);
	return;
}
/////////////////////////////Brief/////////////////////////////
/*
Description:
This function initialises the PWM Feature by
    -   Setting the TCR Register to Reset and Enables at the End of function to start
    -   Setting the Prescaler to 11 to give a 1Mhz Clock into CTIMER0
    -   Setting MR0 to 0 to start with duty cycle at 0%
    -   Setting MR3 to 1000 to give a 1khz PWM frequency
    -   Enables the internal PWM register on MR0
Usage:
    - MR0
    - MR3
    - CTIMER0
Parameters:
-   None -> maybe add frequency
*/
void initPWM(void) {
    //volatile uint32_t *AHBCLK = (volatile uint32_t *) 0x40048080;//AHBCLK Control Register
	//volatile uint32_t *PRESET0 = (volatile uint32_t *) 0x40048088;//Port Presets Control Register
	SYSAHBCLKCTRL0 |= CTIMER0_CLOCK_ENABLE;
	SYSAHBCLKCTRL0 |= (1<<7); //SWM CLOCK ENABLED
    // Configure timer
    CTIMER_TCR = 0x02; // Reset timer
    
    CTIMER_PR = 11; // Set Prescaler Assuming a 12 MHz clock to 1Mhz//

    // Set PWM cycle (e.g., 1000 for 1 kHz PWM)
    CTIMER_MR3 = 1000;

    // Configure match control register (reset on MR3, interrupt)
    CTIMER_MCR = (1 << 10) | (1 << 9) ;

    // Set duty cycle (e.g., 50%)
    CTIMER_MR0 = 1000;/

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
		//uint32_t *poll = 0;
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
