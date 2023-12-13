
#include "Layer1.h"
/*!
 * @brief Main function
 */
int main(void)
{
		initPort();
		changeDIR(8, 1);
		initPWM();
		pwmdutycycle(90);
		while(1){
		}
    
}
