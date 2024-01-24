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

struct uartrb{
	uint8_t	rb[BUFFERSIZE];
	int head;
	int tail;
	/* data */
};

struct uartrb buffer1;
struct uartrb buffer2;

void PIN_INT0_DriverIRQHandler(void){
	volatile uint32_t *PSTAT = (volatile uint32_t *) (0xA000178);
	volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
	*NOT0 |= 0x1<<9;
	return;

}
void USART0_DriverIRQHandler(void){
	//volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
	volatile uint32_t *INTSTAT = (volatile uint32_t *) (0x40064024);
	volatile uint32_t *RXDATA = (volatile uint32_t *) (0x40064014);	
	 //if((*INTSTAT)&0x1){
	buffer1.rb[buffer1.head%BUFFERSIZE] = (*RXDATA);//copy the data modulo is expensive use the algorithmic approach
	buffer1.head += 1;
				 //}
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
	buffer2.head = 0;
	buffer2.tail = 0;
	return;
}


int main(void){
	struct ledorient LED;
	volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
	initPort();
	changeDIR(21,1);
//	LED.north = 0;
//	LED.south = 4;
//	initbuffer();
//	I2Cinit();
	while(1){
		decoder(0x8);
		*NOT0 |= 0x1<<21;
		delay(300);
		*NOT0 |= 0x1<<21;
		delay(300);
		*NOT0 |= 0x1<<21;
		delay(100);
		*NOT0 |= 0x1<<21;
		delay(100);
		*NOT0 |= 0x1<<21;
		delay(100);
		*NOT0 |= 0x1<<21;
		delay(100);
	}
}
