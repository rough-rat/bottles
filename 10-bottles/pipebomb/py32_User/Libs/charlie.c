
#include "charlie.h"

void delayUs(int timeUs){
    //system clock = 8mhz
    for (int i = 0; i < timeUs; i++){
        for (int j = 0; j < 4; j++){
            __asm("nop");
        }
    }
}

int charlieGetBufSize(charlie_t* charlie){
    return charlie->pins.pinCount * (charlie->pins.pinCount -1);
}

void charlieClear(charlie_t* charlie){
    int bufSize = charlieGetBufSize(charlie);
    for (int i = 0; i < bufSize; i++){
        charlie->matrixBuf[i] = 0;
    }
}

void charlieInit(charlie_t* charlie){
    int bufSize = charlieGetBufSize(charlie);
    charlie->matrixBuf = malloc(bufSize * sizeof(uint8_t));
    charlieClear(charlie);
}

void charlieSetPixel(charlie_t* charlie, int x, int y, int value){
    charlie->matrixBuf[x * charlie->pins.pinCount + y] = value;
}

void charliePrint(charlie_t* charlie){
    int pinCount = charlie->pins.pinCount;
    for (int i = 0; i < pinCount-1; i++){
        for (int j = 0; j < (pinCount); j++){
            printf("%d " , charlie->matrixBuf[i * pinCount + j]);
        }
        printf("\n\r");
    }
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


#define SET_LOW 0
#define SET_HIGH 1
#define SET_TRI 2

// to be extracted to main, for this lib to become target-agnostic
void setPinList(pinList_t* pins, int set){
    static GPIO_InitTypeDef gpio;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    uint16_t mask = 0;
    for (int i = 0; i < pins->pinCount; i++){
        mask |= pins->pins[i];
    }
    gpio.Pin = mask;
    if (set == SET_LOW){
        gpio.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(GPIOA, &gpio);
        HAL_GPIO_WritePin(GPIOA, mask, 0);
    } else if (set == SET_HIGH){
        gpio.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(GPIOA, &gpio);
        HAL_GPIO_WritePin(GPIOA, mask, 1);
    } else if (set == SET_TRI){
        gpio.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOA, &gpio);
    }
}

uint16_t buf1[6];
uint16_t buf2[6];
uint16_t buf3;

pinList_t pinsToHi = {0, buf1};
pinList_t pinsToTri = {0, buf2};
pinList_t rowPinStruct = {1, &buf3};

/* Addressing scheme is quite complicated. But it makes sense, i promise.
 * explanation for (line == col) and 
 * [line * charlie->pins.pinCount + col - (line + offs)]
 *
 *    1   2   3   4
 * 1  [X  1   2   3]  
 * 2  [4  X   5   6]
 * 3  [7  8   X   9]
 * 4  [10 11  12  X]
 * 
 * 
 */

static void renderLine(charlie_t* charlie, int line, int debug){
    int brightness = 0;
    int offs = 0;

    pinsToHi.pinCount = 0;
    pinsToTri.pinCount = 0;

    if (debug) printf("\n\rpin %d (%d)\n\r", line, charlie->pins.pins[line]);
    for (int col = 0; col < charlie->pins.pinCount; col++){
        if (line == col){
            offs++;
        } else {
            if (charlie->matrixBuf[line * charlie->pins.pinCount + col - (line + offs)]){
                pinsToHi.pins[pinsToHi.pinCount++] = charlie->pins.pins[col];
                brightness++;
            } else {
                pinsToTri.pins[pinsToTri.pinCount++] = charlie->pins.pins[col];
            }
        }
    }

    setPinList(&pinsToTri, SET_TRI);
    setPinList(&pinsToHi, SET_HIGH);

    rowPinStruct.pins[0] = charlie->pins.pins[line];
    setPinList(&rowPinStruct, SET_LOW);
    delayUs(3*brightness);

    if (debug) {
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET){
            printf("pinCount: %d %d\n\r", pinsToHi.pinCount, pinsToTri.pinCount);
            HAL_Delay(10);
            while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET);
        }
    }    

    setPinList(&rowPinStruct, SET_HIGH);
}

void charlieRender(charlie_t* charlie, int debug){
    for (int i = 0; i < charlie->pins.pinCount; i++){
        renderLine(charlie, i, debug);
    }
    if (debug) printf("\n");
}