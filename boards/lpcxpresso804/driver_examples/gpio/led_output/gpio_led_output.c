
#include "Layer1.h"
/*!
 * @brief Main function
 */
int main(void)
{
		int change = 0;
		uint32_t pin;
		uint32_t *x = 0;
		uint32_t *value;
		initPort();
		changeDIR(8, 1);
		initPWM();
		pwmdutycycle(50);
		while(1){
		}
    
}
