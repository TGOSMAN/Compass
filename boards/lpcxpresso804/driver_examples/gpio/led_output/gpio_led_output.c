
#include "Layer1.h"

/*!
 * @brief Main function
 */
int main(void)
{
		uint32_t *gpiovalue;
		gpiovalue = 0xA0002100;
		initPort();
		changeDIR(8,1);
		
		while(1){
			*gpiovalue ^= 0x1<<8;
			delay(1000);
		}
    
}
