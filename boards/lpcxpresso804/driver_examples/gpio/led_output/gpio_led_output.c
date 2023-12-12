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
Parameters:
-   None -> maybe add frequency
*/
void initPWM(void) {
    volatile uint32_t *AHBCLK = (volatile uint32_t *) (SYSCON+0x80);//AHBCLK Control Register
	volatile uint32_t *TCR = (volatile uint32_t *) (CTIMER+0x04);
    volatile uint32_t *PR = (volatile uint32_t *) (CTIMER+0x0C);
    volatile uint32_t *MCR = (volatile uint32_t *) (CTIMER+0x14);
    volatile uint32_t *MR0 = (volatile uint32_t *) (CTIMER+0x18);
    volatile uint32_t *MR3 = (volatile uint32_t *) (CTIMER+0x24);
    volatile uint32_t *PWMC = (volatile uint32_t *) (CTIMER+0x74);
    //volatile uint32_t *PRESET0 = (volatile uint32_t *) 0x40048088;//Port Presets Control Register
	*AHBCLK |= (1<<25);// Set AHB clock on for CTIMER

    // Configure timer
    *TCR = 0x02;    // Reset timer
    *PR = 11;       // Set Prescaler Assuming a 12 MHz clock to 1Mhz
    *MR3 = 1000;    // Set PWM cycle (e.g., 1000 for 1 kHz PWM)

    // Configure match control register (reset on MR3, interrupt)
    *MCR = (1 << 10) | (1 << 9) ;
    // Set duty cycle (e.g., 0%)
    *MR0 = 999;
    // Enable PWM on MR0
    *PWMC = 0x01;
		
    // Start the timer
    *TCR = 0x01;
	return;
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
		x = (uint32_t *)(GPIO|0x2000);
		pin = *x;
		pin |= 0x3<<8;
		*x = pin;
		//GPIO output
		x = (uint32_t *)(GPIO|0x1024);
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
