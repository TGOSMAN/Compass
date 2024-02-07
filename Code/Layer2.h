#ifndef LAYER2_H
#define LAYER2_H
#include <stdint.h>
#include "Layer1.h"
struct ledorient{
	uint8_t north;
	uint8_t south;
};
struct gpscoords{
    double longitude;
    double latitude;
};
enum status {
	open, //0
	closed //1
};

struct uartrb{
	uint8_t	rb[82];
	enum status door;
	int head;
	int tail;
	/* data */
};


void gpsinit(void);
void gpssendmtk(struct PMTKinstruction instruction);
void gpsreceive(struct gpscoords, char *inputmessage);
void uartsendstring(char *string);
void getbearingdata(struct ledorient *LED);
void gpsdataextract(struct uartrb currentsentence);
#endif