#include "Layer1.h"
#include "Layer2.h"
/*!
 * @brief Main function
 */
//uint8_t buffer[82] = {'/0'};
#define MAXQUE 256
#define BUFFERSIZE 82

enum status {
	open, //0
	closed //1
};


//struct list *bufferque[256] = NULL;
//uart ringbuffer

struct uartrb{
	uint8_t	rb[BUFFERSIZE];
	enum status door;
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
		//must handle condition where both are open, race condition occurs
	if(((*RXDATA) == '$')&&(buffer2.door == closed)){
		buffer1.door = closed;
		buffer2.door = open;
	} else if(((*RXDATA) == '$')&&(buffer1.door == closed)){
		buffer1.door = open;
		buffer2.door = closed;
	}
	if(buffer1.door == open){
		buffer1.rb[buffer1.head%BUFFERSIZE] = (*RXDATA);//copy the data modulo is expensive use the algorithmic approach
		buffer1.head += 1;
	} else if (buffer2.door == open){
		buffer2.rb[buffer2.head%BUFFERSIZE] = (*RXDATA);//copy the data modulo is expensive use the algorithmic approach
		buffer2.head += 1;
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
static int flag = 1;

void initbuffer(void){
	buffer1.head = 0;
	buffer1.tail = 0;
	buffer1.door = open;
	buffer2.head = 0;
	buffer2.tail = 0;
	buffer2.door = closed;
	return;
}


int main(void){
	char stringused[90] = {'\0'};
	initbuffer();
	uartinit();
	struct ledorient LED;
	volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
	initPort();
	changeDIR(21,1);
	uartsendstring("Hello How Are You");
//	LED.north = 0;
//	LED.south = 4;
//	initbuffer();
//	I2Cinit();
	while(1){
		uartsendstring("$Good Thank You");
		if (buffer1.door == closed){
			//buffer1.rb = {'\0'};
			for(int i = 0; i < 90; i++){
				buffer1.rb[i] = '\0';
				//stringused[i] = buffer1.rb[(buffer1.head + i)];// fix this up 
				//buffer1.rb[(buffer1.head + i)%BUFFERSIZE] = '\0';
			}

		} else if(buffer2.door == closed){
			for(int i = 0; i < 90; i++){
				buffer2.rb[i] = '\0';
			}

		}
		uartsendstring("$GDAY");
	}
}
