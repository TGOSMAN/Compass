#include <stdint.h>
#include <stdlib.h>
#include "Layer1.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define	CTIMER  (0x40038000)
#define GPIO    (0xA0000000)
#define PININT	(0xA0004000)
#define SYSCON  (0x40048000)
#define SWM		(0x4000C000)
#define SYSTIC	(0xE000E000)
#define UART0	(0x40064000)
#define I2C0	(0x40050000)
#define NVIC	(0xE000E000)
/////////////////////////////Brief/////////////////////////////
/*
Description:
This Function initialises the GPIO Port by:
    -   Turns on the AHB clock to the GPIO port
    -   Clears the Preset on the GPIO port to operate
Parameters:
-   None
Return: 
- 	Nothing
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
This Function sets the direction of a GPIO Pin by directly editing the DIR register:
Parameters:
-   uint32_t PIN: The pin to change directions
-	int	direction: The direciton to change the pin to (1 for output, 0 for input)
Return: 
- 	Nothing
*/
void changeDIR(uint32_t PIN, int direction){
	volatile uint32_t *DIR = (volatile uint32_t *) (GPIO+0x2000);//AHBCLK Control Register
	if (direction){
		*DIR |= (1<<PIN);
	} else {
		*DIR &= ((0x7FFFFFFF)^(1<<PIN));
	}
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
	- 	Changing the SWM to be 
    -   Enables the internal PWM register on MR0
Parameters:
-   None -> maybe add frequency
Return: 
- 	Nothing
*/
void initPWM(void) {
    volatile uint32_t *AHBCLK = (volatile uint32_t *) (SYSCON+0x80);//AHBCLK Control Register
	volatile uint32_t *TCR = (volatile uint32_t *) (CTIMER+0x04);
    volatile uint32_t *PR = (volatile uint32_t *) (CTIMER+0x0C);
    volatile uint32_t *MCR = (volatile uint32_t *) (CTIMER+0x14);
    volatile uint32_t *MR0 = (volatile uint32_t *) (CTIMER+0x18);
    volatile uint32_t *MR3 = (volatile uint32_t *) (CTIMER+0x24);
    volatile uint32_t *PWMC = (volatile uint32_t *) (CTIMER+0x74);
    volatile uint32_t *PINASSIGN4 = (volatile uint32_t *) (SWM+0x10);
    //volatile uint32_t *PRESET0 = (volatile uint32_t *) 0x40048088;//Port Presets Control Register
	*AHBCLK |= (1<<25);// Set AHB clock on for CTIMER
	*AHBCLK |= (1<<7);// Set AHB clock on for SWM

    // Configure timer
    *TCR = 0x02;    // Reset timer
    *PR = 11;       // Set Prescaler Assuming a 12 MHz clock to 1Mhz
    *MR3 = 1000;    // Set PWM cycle (e.g., 1000 for 1 kHz PWM)

    // Configure match control register (reset on MR3, interrupt)
    *MCR = (1 << 10) | (1 << 9) ;
    *MR0 = 999;// Set duty cycle (e.g., 0%)
    *PWMC = 0x01;// Enable PWM on MR0
		
    // Start the timer
    *TCR = 0x01;

	//Assign the output to P0_21
	*PINASSIGN4 &= 0x21;//0x21; 
	return;
}
////////////////////////////Brief/////////////////////////////
/*
Description:
This changes the PWM that is output using MR0 by:
    -   First calculating the match value so that it will stay as an integer
	- 	Setting the MR0 register value
Parameters:
-   uint32_t percent: this is the percent dutycycle desired
Return: 
- 	Nothing
*/
void pwmdutycycle(uint32_t percent){
	uint32_t matchvalue = 0;
	volatile uint32_t *MR0 = (volatile uint32_t *) (CTIMER+0x18);
	matchvalue = (1000-(10*percent));// this format is to ensure the decimals don't appear and zero out
	*MR0 = matchvalue;
	return;
}//need to disable timer
////////////////////////////Brief/////////////////////////////
/*
Description:
This decoder by :
		- 	calculating the bit levels of each position then:
    -   writing to the set and clear registers for an output
Parameters:
-   uint32_t Selection: This is the number/bits you would input into the decoder normally
Return: 
- 	Nothing
*/
void decoder(uint32_t selection){
    uint32_t bit = 0x0;
    volatile uint32_t *SET0 = (volatile uint32_t *) (GPIO+ 0x2200);// Set for register for port 0
    volatile uint32_t *CLR0 = (volatile uint32_t *) (GPIO+ 0x2280);
    selection &= 0x7;
    //A0
    bit = selection&(0x1);
		if(bit){
				*SET0 |= 0x1<<18;//P18 on PCB
		}else{
				*CLR0 |= 0x1<<18;
		}
    //A1
    bit = (selection&(0x2))>>1;
		if(bit){
				*SET0 |= 0x1<<4;//P04 on PCB//P07 this causes an error on the circuit board ISP pin is pin12
		}else{
				*CLR0 |= 0x1<<4
		}
    //A2
    bit = (selection&(0x4))>>2;
		if(bit){
				*SET0 |= 0x1<<20;//P20 on PCB
		}else{
				*CLR0 |= 0x1<<20;
		}
    return;
}

void delay(uint32_t ms){
	//let it run with the systemclock/2 hz == 6Mhz
	volatile uint32_t *CSR = (volatile uint32_t *) (SYSTIC +0x10);
   volatile uint32_t *CVR = (volatile uint32_t *) (SYSTIC + 0x18);
	volatile uint32_t *RVR = (volatile uint32_t *) (SYSTIC + 0x14);
	uint32_t adjustedvalue = 6000*ms -1;
	*RVR = adjustedvalue;
	*CVR |= 0x1;
	*CSR |= 0x1;
	while(!((*CSR)&(0x1<<16))){
		//polling version so this will be blocking
	}
	*CSR ^= 0x1;
	return;
}


void uartinit(void){
	volatile uint32_t *AHBCLK = (volatile uint32_t *) (SYSCON+0x80);
	volatile uint32_t *FRG0DIV = (volatile uint32_t *) (SYSCON+0xD0);
	volatile uint32_t *PDRUNCFG = (volatile uint32_t *) (SYSCON+0x238);
	volatile uint32_t *PINASSIGN0 = (volatile uint32_t *) (SWM);
	volatile uint32_t *UART0CLKSEL = (volatile uint32_t *) (SYSCON+0x90);
	volatile uint32_t *PRESETCTRL0 = (volatile uint32_t *) (SYSCON+0x88);
	volatile uint32_t *UART0CFG = (volatile uint32_t *) (UART0);
	volatile uint32_t *UART0BRG = (volatile uint32_t *) (UART0 + 0x20);	
	volatile uint32_t *UART0INTENSET = (volatile uint32_t *) (UART0 + 0xC);
	volatile uint32_t *ISER0 = (volatile uint32_t *) (NVIC + 0x100);
	//initialise pins:
	*AHBCLK |= 0x1<<7; // Enable AHB Clock for SWM to work
	*PINASSIGN0 &= 0xFFFFFF02; // TXD -> P0_1 PCB:02
	*PINASSIGN0 &= ((0x7<<9)|(0xFFFF00FF));// RXD -> P0_7 PCB:09
	*AHBCLK ^= 0x1<<7; // Disable AHB Clock for SWM
	// initialise Clocks
	*PDRUNCFG ^= 0x1<<6;//ensure LPOCSC_PD is powered on// why use XOR
	//Must be this value to use the FRO clock//multiply is not set might need to for the right freq
	//UART CLOCKS
	*UART0CLKSEL ^= 0x6;
	*AHBCLK |= 0x1<<14;
	//Presets
	*PRESETCTRL0 |= 0x1<<14;
	*PRESETCTRL0 |= 0x1<<7;
	//UART Config
	*UART0CFG |= 0x5;// setup for basic sending operation
	*UART0BRG = 0x26;//setting up for a baud rate of 9600baud/s its actually close to 9538 which should be // Changed compared with th
	*UART0INTENSET |= 0x1;//interrupt when there is a received character available to be read
	*ISER0 |= 1<<3;
	*UART0CLKSEL ^=0x5;

	return;
}

void sendcharuart(uint8_t character){
	volatile uint32_t *UART0TX = (volatile uint32_t *) (UART0 + 0x1C);
	volatile uint32_t *UART0STAT = (volatile uint32_t *) (UART0 + 0x8);	
	while(!((*UART0STAT)&(0x1<<2))){
	};//blocking method
	*UART0TX = character;
	while(!((*UART0STAT)&(0x1<<3))){
	};
	return;
}

uint8_t readcharuart(void){
	uint8_t character = NULL;

	return character;
}

/*void USART0_DriverIRQHandler(void){


}*/

void I2Cinit (void) {
	volatile uint32_t *AHBCLK = (volatile uint32_t *) (SYSCON+0x80);
	volatile uint32_t *FRG0DIV = (volatile uint32_t *) (SYSCON+0xD0);
	volatile uint32_t *PDRUNCFG = (volatile uint32_t *) (SYSCON+0x238);
	volatile uint32_t *PINASSIGN5 = (volatile uint32_t *) (SWM + 0x14);
	volatile uint32_t *I2C0CLKSEL = (volatile uint32_t *) (SYSCON+0xA4);
	volatile uint32_t *PRESETCTRL0 = (volatile uint32_t *) (SYSCON+0x88);
	volatile uint32_t *I2C0CLKDIV = (volatile uint32_t *) (I2C0 + 0x14);
	volatile uint32_t *I2C0CFG = (volatile uint32_t *) (I2C0);
	volatile uint32_t *MSTTIME = (volatile uint32_t *) (I2C0 + 0x24);
	//volatile uint32_t *UART0CFG = (volatile uint32_t *) (UART0);
	//volatile uint32_t *UART0BRG = (volatile uint32_t *) (UART0 + 0x20);

	*AHBCLK |= 1<<7; //swm clock on 
	*PINASSIGN5 = 10; //setup SDA on P0_10
	*PINASSIGN5 = 11<<8;//setup SCL on P0_11
	*AHBCLK ^= 1<<7; //disable SWM clock
	
	*FRG0DIV |= 0xFF;//Make this defined
	*I2C0CLKSEL &= 0x1;//FRO, should be 12Mhz by default
	*AHBCLK |= 0x1<<5; //I2C0 Clock on

	//Presets
	*PRESETCTRL0 |= 0x1<<5;//I2C Reset Run
	*PRESETCTRL0 |= 0x1<<7;
	//Configuring the CLKDIV from i2C
	*MSTTIME &= 0xFFFFFFF1;// MSTSCLLOW low for 1 according to recommended settings table
	*I2C0CLKDIV |= 0X5;// Assumning FRO is 12Mhz by default
	*I2C0CFG |= 0x11;//master enable and clock stretch enable
	return;
}



void pininterruptsetup(void){
	volatile uint32_t *PINTSEL0 = (volatile uint32_t *) (SYSCON+0x178);
	volatile uint32_t *AHBCLK = (volatile uint32_t *) (SYSCON+0x80);
	volatile uint32_t *ISEL = (volatile uint32_t *) (PININT);
	volatile uint32_t *SIENR = (volatile uint32_t *) (PININT + 0x008);
	volatile uint32_t *SIENF = (volatile uint32_t *) (PININT + 0x014);
	volatile uint32_t *ISER0 = (volatile uint32_t *) (NVIC + 0x100);
	*PINTSEL0 |= 0x7;//P0.7 will correspond to pin interrupt 0
	*AHBCLK |= 1<<28;// pin interrup turned on
	*ISEL	|= 1;// set level sensitive pin int on pin int 0
	*SIENF	|= 1;// Set the level sensitivity register for pin int 0
	*SIENR	|= 1;//Set the enable  reg for the interrupt on pin interrupt 0
	*ISER0	|= 1<<24;
	//	__asm{
	//		CPSIE i
	//	}
	return;
	
}


void I2Csendframewrite(uint8_t address, uint8_t register1, uint8_t data){
	volatile uint32_t *I2C0MSTDATA = (volatile uint32_t *) (0x40050028);
	volatile uint32_t *I2C0MSTCTL = (volatile uint32_t *) (0x40050020);
	volatile uint32_t *I2C0STAT = (volatile uint32_t *) (0x40050004);
	*I2C0MSTDATA |= address;// consider where teh r/w bit sits might need to shift
	*I2C0MSTCTL = 0x2;// start bit
	while(!((*I2C0STAT)&0x1)){
	}
	*I2C0MSTDATA = register1;
	*I2C0MSTCTL = 0x1; //register address to be sent to IMU
		while(!((*I2C0STAT)&0x1)){
	}
	*I2C0MSTDATA = data;//next data
	*I2C0MSTCTL = 0x1;
	while(!((*I2C0STAT)&0x1)){
	}
	*I2C0MSTCTL = 0x4;
	return;
}

uint8_t I2Csendframeread(uint8_t address, uint8_t register1){
	uint8_t data;
	volatile uint32_t *I2C0MSTDATA = (volatile uint32_t *) (0x40050028);
	volatile uint32_t *I2C0MSTCTL = (volatile uint32_t *) (0x40050020);
	volatile uint32_t *I2C0STAT = (volatile uint32_t *) (0x40050004);
	*I2C0MSTDATA |= address;// consider where teh r/w bit sits might need to shift
	*I2C0MSTCTL = 0x2;// start bit
	while(!((*I2C0STAT)&0x1)){
	}
	*I2C0MSTDATA = register1;
	*I2C0MSTCTL = 0x1; //register address to be sent to IMU
		while(!((*I2C0STAT)&0x1)){
	}
	data = (*I2C0MSTDATA);//next data
	*I2C0MSTCTL = 0x1;
	while(!((*I2C0STAT)&0x1)){
	}
	*I2C0MSTCTL = 0x4;
	return data;
}