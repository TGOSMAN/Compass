
#include "Layer1.h"

/*!
 * @brief Main function
 */
int main(void)
{
		initPort();
		changeDIR(8, 1);
		changeDIR(9, 1);
		changeDIR(13, 1);
		decoder(0x4);
		//initPWM();
		//pwmdutycycle(90);
		while(1){
		}
    
}
