#include <stdlib.h> 
#include <string.h>
#include "Layer1.h"
#include "Layer2.h"
/*!
 * @brief Main function
 */
//uint8_t buffer[82] = {'/0'};
#define MAXQUE 256
#define BUFFERSIZE 82



//struct list *bufferque[256] = NULL;
//uart ringbuffer
struct uartrb buffer1;
struct uartrb buffer2;
struct uartrb buffer3;


static uint8_t usartsentence[BUFFERSIZE];
void PIN_INT0_DriverIRQHandler(void){
	volatile uint32_t *PSTAT = (volatile uint32_t *) (0xA000178);
	volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
	*NOT0 |= 0x1<<9;
	return;

}
static int flag = 0;

void USART0_DriverIRQHandler(void){
	volatile uint32_t *SET0 = (volatile uint32_t *) (0xA0002200);
	volatile uint32_t *INTSTAT = (volatile uint32_t *) (0x40064024);
	volatile uint32_t *RXDATA = (volatile uint32_t *) (0x40064014);
		//must handle condition where both are open, race condition occurs
	if(((*RXDATA) == '$')){
		buffer1.door ^= closed;
		buffer2.door ^= closed;
		if(!(buffer1.door)){
			buffer1.head = buffer1.tail;
		} else {
			buffer2.head = buffer2.tail;
		}
		//*SET0 |= 0x1<<9;
	}

	if(buffer1.door == open){
		buffer1.rb[buffer1.tail%BUFFERSIZE] = (*RXDATA);//copy the data modulo is expensive use the algorithmic approach
		buffer1.tail+= 1;
	} else if (buffer2.door == open){
		buffer2.rb[buffer2.tail%BUFFERSIZE] = (*RXDATA);//copy the data modulo is expensive use the algorithmic approach
		buffer2.tail += 1;
	}
	
//	*NOT0 |= 0x1<<9;
//	delay(5000);
//	*NOT0 |= 0x1<<9;
	return;
}
/*void handler(void){
	 volatile uint32_t *INTSTAT = (volatile uint32_t *) (0x40068024);
	 volatile uint32_t *RXDATA = (volatile uint32_t *) (0x40068014);	
	 if((*INTSTAT)&0x1){
		//(*RXDATA)
		if(buffer1.head >= 80){
			buffer1.rb[buffer1.head + 1] = '/0';//null terminate
			buffer1.head = buffer1.tail;
		} else {
			buffer1.rb[buffer1.head + 1] = (*RXDATA);//copy the data 
			buffer1.head += 1;
		}
	 }
	 return;
}
*/


void initbuffer(void){
	buffer1.head = 0;
	buffer1.tail = 0;
	buffer1.door = open;
	buffer2.head = 0;
	buffer2.tail = 0;
	buffer2.door = closed;
	buffer3.head = 0;
	buffer3.tail = 81;
	buffer3.door = closed;
	return;
}





 int main(void){

	initbuffer();
	uartinit();
	initPWM();
	int k = 0;
	int j = 0;
	struct ledorient LED;
	volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
	initPort();
	changeDIR(9,1);
//	uartsendstring("$GPRMC,004530.000,A,3340.6622,S,15055.5859,E,0.25,0.00,040224,,,A*77");
//	 uartsendstring("$GPRMC,004530.000,A,3340.6622,S,15055.5859,E,0.25,0.00,040224,,,A*77");
//	LED.north = 0;
//	LED.south = 4;
//	initbuffer();
//	I2Cinit();
	while(1){
		//uartsendstring("$PMTK104*37<CR><LF>");
		//uartsendstring("$Good Thank You");
		if (buffer1.door == closed){
			//buffer1.rb = {'\0'};
			k = 0;
			j = buffer1.head;
			gpsdataextract(buffer1);
		} else if(buffer2.door == closed){
			gpsdataextract(buffer2);
//			k = 0;
//			j = buffer2.head;
//			while (j != buffer1.tail){
//				buffer3.rb[k] |= buffer2.rb[j];// fix this up 
//				j = (j+1)%BUFFERSIZE;
//				k++;
//				//buffer1.rb[(buffer1.head + i)%BUFFERSIZE] = '\0';
//			}

		}
	}
}
