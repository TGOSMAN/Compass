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

#define EULERMSB	0x1A
#define EULERLSB	0x1B
#define	IMUADDRESS	0x29
#define CALLIB_STAT 0x35
#define CONFIG_MODE 0x3D
#define CHIPID 0x00
#define MAGID 0x02
#define TEMP 0x34
#define GRAVZVEC 0x33
#define ST_RESULT 0x36
#define SYS_TRIGGER 0x3F
#define INT_STA 0x37
#define ACC_DATAXLSB 0x0E
#define PAGE_ID 0x07;
#define SYS_STATUS 0x39
//struct list *bufferque[256] = NULL;
//uart ringbuffer
struct uartrb buffer1;
struct uartrb buffer2;
struct uartrb buffer3;
static uint8_t usartsentence[BUFFERSIZE];


void PIN_INT0_DriverIRQHandler(void){
	volatile uint32_t *PSTAT = (volatile uint32_t *) (0xA000178);
	volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
	volatile uint32_t *SET0 = (volatile uint32_t *) (0xA0002200);
	*NOT0 |= 0x1<<21;
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
//	volatile uint32_t *SET0 = (volatile uint32_t *) (0xA0002200);
//	 volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
	uint8_t status = 0;
//	LED.north = 0;
//	LED.south = 4;
//	 uint16_t heading1 = 0;
//	 double heading = 0;
//	 double adjust = 180.0;
	 //uint8_t heading2 = 0;
	initPort();
	changeDIR(18,1);
	changeDIR(4,1);
	changeDIR(20,1);
	changeDIR(21,1);
	changeDIR(10,1);
	initbuffer();
	I2Cinit();
	IMUinit();
	uartinit();
	while(1){
		if(buffer1.door == closed){
			gpsdataextract(buffer1);
		}else if (buffer2.door == closed) {
			gpsdataextract(buffer2);
		}

		//delay(500);

	}
}
