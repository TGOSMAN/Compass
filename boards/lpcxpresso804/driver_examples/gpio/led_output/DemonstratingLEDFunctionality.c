
#include "Layer1.h"

/*!
 * @brief Main function
 */
int main(void)
{
		volatile uint32_t *PIN = (volatile uint32_t *) (0xA0002100);//AHBCLK Control Register
		initPort();
		uint8_t you[2] = {0x7,0x1};
		uint8_t wish[2] = {0x2,0x6};
		uint8_t we[3] = {0,0,0};
		uint8_t amerry[3] = {0x3,0x4,0x5};
		uint8_t aand[3] = {0x0,0x1,0x2};
		uint8_t AA[3] = {0x2,0x3,0x4};
		uint8_t happy[3] = {0x4,0x5,0x6};
		uint8_t newy[3] = {0x6,0x7,0x0};
		changeDIR(21,1);
		//*PIN |= 1<<21;
		changeDIR(18,1);
		changeDIR(12,1);
		changeDIR(20,1);
		decoder(0);
		initPWM();
		pwmdutycycle(10);
		while(1){
			for(int k = 0; k<3; k++){
				for(int i=0; i<3;i++){
					decoder(we[i]);
					delay(250);
				}
				for(int i = 0; i < 25; i++){
					decoder(wish[0]);
					delay(10);
					decoder(wish[1]);
					delay(10);
				}
				for(int i = 0; i < 25; i++){
					decoder(you[0]);
					delay(10);
					decoder(you[1]);
					delay(10);
				}
				decoder(amerry[0]);
				delay(350);
				decoder(amerry[1]);
				delay(350);
				decoder(amerry[2]);
				delay(350);
				*PIN ^= 1<<21;
				decoder(0);
				delay(250);
				*PIN ^= 1<<21;
				delay(250);
				*PIN ^= 1<<21;
				decoder(4);
				delay(1000);
			}
			for(int i = 0; i < 20; i++){
					decoder(aand[0]);
					delay(10);
					decoder(aand[1]);
					delay(10);
					decoder(aand[2]);
					delay(10);
				}
			for(int i = 0; i < 20; i++){
					decoder(AA[0]);
					delay(10);
					decoder(AA[1]);
					delay(10);
					decoder(AA[2]);
					delay(10);
			}
			for(int i = 0; i < 15; i++){
					decoder(happy[0]);
					delay(10);
					decoder(happy[1]);
					delay(10);
					decoder(happy[2]);
					delay(10);
			}
			for(int i = 0; i < 15; i++){
					decoder(happy[0]);
					delay(10);
					decoder(happy[1]);
					delay(10);
					decoder(happy[2]);
					delay(10);
			}
			for(int i = 0; i < 20; i++){
					decoder(newy[0]);
					delay(10);
					decoder(newy[1]);
					delay(10);
					decoder(newy[2]);
					delay(10);
			}
			*PIN ^= 1<<21;
			delay(500);
			*PIN ^= 1<<21;
				delay(1000);
				delay(1000);
				delay(1000);
				delay(1000);
				delay(1000);
				delay(1000);
			
//			decoder(3);
//			delay(100);
//			decoder(4);
//			delay(100);
//			decoder(5);
//			delay(100);
//			for(int i = 50; i<100; i+=10){
//				pwmdutycycle(i);
//				delay(100);
//			}
//			for(int i = 100; i>0; i-= 10){
//				pwmdutycycle(i);
//				delay(100);
//			}
		}
    
}
