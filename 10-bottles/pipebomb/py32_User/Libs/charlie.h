#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "py32f0xx_bsp_printf.h"


#ifndef CHARLIE_H
#define CHARLIE_H

typedef struct {
    uint16_t pinCount;
    uint16_t* pins;
} pinList_t;

typedef struct {
    pinList_t pins;
    uint8_t* matrixBuf;
} charlie_t;

void delayUs(int timeUs);
int charlieGetBufSize(charlie_t* charlie);
void charlieClear(charlie_t* charlie);
void charlieInit(charlie_t* charlie);
void charlieSetPixel(charlie_t* charlie, int x, int y, int value);
void charliePrint(charlie_t* charlie);
void setPinList(pinList_t* pins, int set);
void charlieRender(charlie_t* charlie, int debug);

#endif // CHARLIE_H
