#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
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
#define EULERMSB	0x1B
#define EULERLSB	0x1A
#define	IMUADDRESS	0x29
#define CALLIB_STAT 0x35
#define CONFIG_MODE 0x3D
#define M_PI       3.14159265358979323846   // pi
#define FIR0	0.0135
#define FIR1	0.0785
#define FIR2	0.2409
#define FIR3	0.3344
#define FIR4	0.2409
#define FIR5	0.0785
#define FIR6 	0.0135

static volatile double destilat = -27.96281;
static volatile double destilong = 153.408921;
static struct gpscoords myloco;
struct ledorient LED;

//	double lat;//testing
//	double longi;//testing
double toRadians(double degrees) {
    return degrees * (M_PI/ 180);
}

void gpsinit(void){
    //ENABLE, RESET and etc
	// will need to change this to match the new device SAM M10Q
    uartsendstring("$PTMK220,1000*1F<CR><LF>");//set position fix interval to 1s -> checksum is XOR of all characters between $ and *
    uartsendstring("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29<CR><LF>");//Turn on RMC data only 
    //uartsendstring("$PMTK225,");//setup periodic mode if desired for power saving
    return;
}

double convertLatitudeToDecimal(char *latitudeStr, char hemisphere) {
    double decimalDegrees = 0.0;
    double degrees = 0.0;
    double minutes = 0.0;
		volatile uint32_t *SET0 = (volatile uint32_t *) (0xA0002200);
    // Convert the string to a double
    double rawLatitude = 0.0;
			rawLatitude = atof(latitudeStr);

    // Extract degrees and minutes from the raw latitude
    // Assumes latitude is in DDMM.MMMM format
    degrees = (int)(rawLatitude / 100);
    minutes = rawLatitude - (degrees * 100);

    // Convert to decimal degrees
    decimalDegrees = degrees + (minutes / 60);

    // If the hemisphere is 'S', make the result negative
    if (hemisphere == 'S') {
        decimalDegrees = -decimalDegrees;
				*SET0 |= 0x1<<21;
    }

    return decimalDegrees;
}

double convertLongitudeToDecimal(char *longitudeStr, char hemisphere) {
    double decimalDegrees = 0.0;
    double degrees = 0.0;
    double minutes = 0.0;
    // Convert the string to a double
    double rawLongitude = 0.0;
		rawLongitude= atof(longitudeStr);

    // Extract degrees and minutes from the raw latitude
    // Assumes latitude is in DDMM.MMMM format
    degrees = (int)(rawLongitude / 100);
    minutes = rawLongitude - (degrees * 100);

    // Convert to decimal degrees
    decimalDegrees = degrees + (minutes / 60);
    // If the hemisphere is 'S', make the result negative
    if (hemisphere == 'W') {
        decimalDegrees = -decimalDegrees;
    }

    return decimalDegrees;
}
double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
    // Convert latitude and longitude from degrees to radians
    lat1 = toRadians(lat1);
    lon1 = toRadians(lon1);
    lat2 = toRadians(lat2);
    lon2 = toRadians(lon2);

    // Calculate the difference in longitude
    double dLon = lon2 - lon1;

    // Calculate the bearing
    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double bearingRadians = atan2(y, x);
    return bearingRadians;
}

void gpsdataextract(struct uartrb currentsentence){
	volatile uint32_t *ISER0 = (volatile uint32_t *) (0xE000E100);
	volatile uint32_t *SET0 = (volatile uint32_t *) (0xA0002200);
	volatile uint32_t *NOT0 = (volatile uint32_t *) (0xA0002300);
    volatile double lat;//testing
    volatile double longi;//testing
    volatile double bearing = 0;
    char latitudearray[9];
    char longitudearray[10];
	char hemi;
		*ISER0 ^= 1<<3;
		if((currentsentence.rb[(currentsentence.head +3)%82] == 'R')&&(currentsentence.rb[(currentsentence.head +18)%82] == 'A')){
            for(int i = 0; i<9;i++){
               latitudearray[i] = currentsentence.rb[(currentsentence.head +(i+20))%82];
               longitudearray[i] = currentsentence.rb[(currentsentence.head +(i+32))%82];
            }
            longitudearray[8] = currentsentence.rb[(currentsentence.head + 41)%82];
						longitudearray[9] = '\0';
						latitudearray[8] = '\0';
			//take out the coordinates
			hemi = currentsentence.rb[(currentsentence.head + 30)%82];
					
			/*myloco.latitude*/lat = convertLatitudeToDecimal(latitudearray, hemi);
			hemi = currentsentence.rb[(currentsentence.head + 44)%82];
			/*myloco.longitude*/longi = convertLongitudeToDecimal(longitudearray, hemi);
            bearing = calculateBearing(lat,longi,destilat,destilong);
						bearing = bearing*(180/M_PI);
						bearing += 360;
						getbearingdata(&LED);
						bearingdecod(bearing);
            
		}
		//decoder(4);
//		getbearingdata(&LED);
//		bearingdecod(bearing);
		*ISER0 ^= 1<<3;
		return;
};
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
void bearingdecod(double bearing){
//		if(bearing < 0){
//			bearing += 360;
////			decoder(4);
//		}
		
		if((22.5 > bearing)&&(bearing >= 0)){
			decoder(((0 + LED.north)%8));
		} else if((67.5 >bearing)&&(bearing >= 22.5 )){
			decoder(((1 + LED.north)%8));
		} else if ((112.5 > bearing)&&(bearing >= 67.5)){
			decoder(((2 + LED.north)%8));
		} else if ((157.5 > bearing)&&(bearing >= 112.5)){
			decoder(((3 + LED.north)%8));
		}else if ((202.5 > bearing)&&(bearing >= 157.5)){
			decoder(((4 + LED.north)%8));
		}else if ((247.5 > bearing)&&(bearing >= 202.5)){
			decoder(((5 + LED.north)%8));
		}else if ((292.5 > bearing)&&(bearing >= 247.5)){
			decoder(((6 + LED.north)%8));
		}else if ((337.5 > bearing)&&(bearing >= 292.5)){
			decoder(((7 + LED.north)%8));
		} else if ((bearing >= 337.5)){
			decoder(((0 + LED.north)%8));
		}
		
	return;
}


void uartsendstring(char *string){
    for(int i = 0; i<strlen(string);i++){
        sendcharuart(string[i]);
    }
    return;
}



void IMUinit(void){
  uint8_t status = 0;
	volatile uint32_t *SET0 = (volatile uint32_t *) (0xA0002200);
	//decoder(2);
	decoder(0);
	*SET0 ^= 1<<10;
	delay(4000);
	*SET0 |= 1<<10;
	delay(2000);
	I2Csendframewrite(IMUADDRESS,CONFIG_MODE, 0x9);
	I2Csendframewrite(IMUADDRESS,CONFIG_MODE, 0x9);
	I2Csendframewrite(IMUADDRESS,CONFIG_MODE, 0x9);
	I2Csendframewrite(IMUADDRESS,CONFIG_MODE, 0x9);
	//delay(5000);
	delay(2000);
	*SET0 |= 1<<21;
	LED.north = 0;
	LED.south = 4;
	for(int i = 0; i < 7; i++){
		LED.buffer[i] = 0;
	}
	/*
	while(status = 3){
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
    delay(70);
		*/
		
		
}

void getbearingdata(struct ledorient *LED){
	double heading = 0;
	volatile int16_t headingdata = 0;
	volatile uint8_t data8[2] = {0};
	data8[0] = I2Csendframeread(IMUADDRESS,EULERLSB);
	data8[1] = I2Csendframeread(IMUADDRESS,EULERMSB);

	headingdata = (int16_t)((int32_t)(data8[1]<<8)|data8[0]);
	headingdata = ((double) headingdata)/16.0;
	if((headingdata < 0)&&(headingdata > -180)){
		headingdata += 360;
	}
	if ((headingdata >0) && (headingdata < 360)){
		for(int i = 6; i>0; i--){
			LED->buffer[i] = LED->buffer[i-1];
		} 
		LED->buffer[0] = headingdata;
		//decoder(4);
		heading =   (FIR0*(LED->buffer[0])) + (FIR1*(LED->buffer[1])) + (FIR2*(LED->buffer[2])) + (FIR3*(LED->buffer[3])) + (FIR4*(LED->buffer[4])) + (FIR5*(LED->buffer[5])) + (FIR6*(LED->buffer[6]));//I dont know if this is completely valid
	}	
	if ((heading > 0.1) && (heading < 22)){
		//decoder(0);
		LED->north = 4;
		LED->south = 0;
	} else if ((heading >= 22) && (heading < 67)){
		//decoder(1);
		LED->north = 3;
		LED->south = 7;
	}else if ((heading >= 67) && (heading < 112)){
		//decoder(2);
		LED->north = 2;
		LED->south = 6;
	}else if ((heading >= 112) && (heading < 157)){
		//decoder(3);
		LED->north = 1;
		LED->south = 5;
	}else if ((heading >= 157) && (heading < 202)){
		LED->north = 0;
		LED->south = 4;
	}else if ((heading >= 202) && (heading < 247)){
		LED->north = 7;
		LED->south = 1;
	}else if ((heading >= 247) && (heading < 292)){
		LED->north = 6;
		LED->south = 2;
	}else if ((heading >= 292) && (heading < 337)){
		LED->north = 5;
		LED->south = 3;
	}else if ((heading >= 337) && (heading <= 360)){
		LED->north = 4;
		LED->south = 0;
	}
    return;
}
//Testing that the cloning of github works
