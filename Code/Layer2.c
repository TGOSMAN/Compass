#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Layer2.h"
//#include "gpio_led_output.c"
#include "Layer1.h"


/*struct PMTKinstruction {
    //maybe restructure these to be char arrays
    char type[1];
    char Run_time[5];
    char Sleep_time[5];
    char Run_time2[5];
    char Sleep_time2[5];
    char check_sum[2];
};*/
#define EULERMSB	0x1A
#define EULERLSB	0x1B
#define	IMUADDRESS	0x29
#define CALLIB_STAT 0x35
#define CONFIG_MODE 0x3D
struct gpscoords{
    double longitude;
    double latitutde;
};

void gpsinit(void){
    //ENABLE, RESET and etc
	// will need to change this to match the new device SAM M10Q
    uartsendstring("$PTMK220,1000*1F<CR><LF>");//set position fix interval to 1s -> checksum is XOR of all characters between $ and *
    uartsendstring("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29<CR><LF>");//Turn on RMC data only 
    //uartsendstring("$PMTK225,");//setup periodic mode if desired for power saving
    return;
}

//void gpsreceive(struct gpscoords coordinates, char *inputmessage){
//    char latitude[10]={'\0'};
//    char longitude[11]={'\0'};
//    for(int i = 0; i<10; i++){
//        latitude[i] = inputmessage[i+20];//capture bit in GPRMC Field
//        longitude[i] = inputmessage[i+33];//capture bits in GPRMC Field
//    }
//    for(int i = 0; i<11; i++){
//        longitude[i] = inputmessage[i+33];//capture bits in GPRMC Field
//    }
//    coordinates.longitude = strtod(longitude,&longitude[10]);// should convert over to dddmm.mmmm
//    coordinates.latitutde = strtod(latitude,&latitude[9]);//should convert over to ddmm.mmmm 
//    //strtod to convert a string (e.g "4.06") to double (e.g 4.06)
//    return;
//}

/*void gpssendmtk(struct PMTKinstruction instruction){
    //FORMAT:
    //$PMTK225,<type (1 digit)>,<Run_time (5 digits)>,<sleep_time (5 digits)>,<2nd_Run_Time (5 digits)>,<2nd_Sleep_Time (5 digits)>*<Checksum (2 bytes)>
    char *message = malloc(38*sizeof(char));
    //wil need a int to char function if struct isn't changed to strings
    snprintf(message, 38,"$PMTK225,%s,%s,%s,%s,%s,%s<CR><LF>",instruction.type,instruction.Run_time,instruction.Sleep_time,instruction.Run_time2, instruction.Sleep_time2,instruction.check_sum);
    //check how to configure Null Terminators for snprintf
    uartsendstring(message);
    free(message);
    return;
}*/

void uartsendstring(char *string){
    for(int i = 0; i<strlen(string);i++){
        sendcharuart(string[i]);
    }
    return;
}



void IMUinit(void){
    uint8_t status = 0;
	while(status != 3){
        //check for gyro callibration
        status = I2Csendframeread(IMUADDRESS, CALLIB_STAT);
        status &= 0x30;
        status >>= 4;
    }
    //blink once
    status = 0;
    while(status != 3){
        //mag
        status = I2Csendframeread(IMUADDRESS, CALLIB_STAT);
        status &= 0x03;
    }
    // blink twice
    status = 0;
    while(status != 3){
        // acceler
        status = I2Csendframeread(IMUADDRESS, CALLIB_STAT);
        status &= 0x0C;
        status >>= 2;
    }
    // blink three times
    status = 0;
    while(status != 3){
        // syst
        status = I2Csendframeread(IMUADDRESS, CALLIB_STAT);
        status &= 0xC0;
        status >>= 6;
    }
    // blink four times
    //all callibrated
    I2Csendframewrite(IMUADDRESS,CONFIG_MODE, 0x9);
    // place it in the 9 degrees of freedom
    delay(7);
}

void getbearingdata(struct ledorient *LED){
	uint16_t heading = 0;
	uint8_t temp = 0;
	heading = I2Csendframeread(IMUADDRESS,EULERMSB);
	temp = I2Csendframeread(IMUADDRESS,EULERLSB);
	heading = heading << 8;
	heading |= temp;
	if ((heading >= 0) && (heading < 22)){
		LED->north = 0;
		LED->south = 4;
	} else if ((heading >= 22) && (heading < 67)){
		LED->north = 1;
		LED->south = 5;
	}else if ((heading >= 67) && (heading < 112)){
		LED->north = 2;
		LED->south = 6;
	}else if ((heading >= 112) && (heading < 157)){
		LED->north = 3;
		LED->south = 7;
	}else if ((heading >= 157) && (heading < 202)){
		LED->north = 4;
		LED->south = 0;
	}else if ((heading >= 202) && (heading < 247)){
		LED->north = 5;
		LED->south = 1;
	}else if ((heading >= 247) && (heading < 292)){
		LED->north = 6;
		LED->south = 2;
	}else if ((heading >= 292) && (heading < 337)){
		LED->north = 7;
		LED->south = 3;
	}else if ((heading >= 337) && (heading < 360)){
		LED->north = 0;
		LED->south = 4;
	}
    return;
}
