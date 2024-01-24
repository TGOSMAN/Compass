#ifndef LAYER2_H
#define LAYER2_H
#include <stdint.h>
#include "Layer1.h"
struct ledorient{
	uint8_t north;
	uint8_t south;
};
void gpsinit(void);
void gpssendmtk(struct PMTKinstruction instruction);
void gpsreceive(struct gpscoords, char *inputmessage);
void uartsendstring(char *string);
void getbearingdata(struct ledorient *LED);
#endif