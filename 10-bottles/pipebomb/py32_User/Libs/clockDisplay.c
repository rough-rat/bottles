

#include <stdint.h>
#include "clockDisplay.h"
#include "charlie.h"
#include "segment.h"

#include "pindefs.h"

segment_t segments[4] = {
    {{2, 3, 7, 8, 13, 5, 1}},
    {{27, 4, 6, 0, 20, 9, 24}},
    {{11, 16, 10, 15, 12, 25, 14}},
    {{21, 26, 18, 29, 28, 22, 17}}};

charlie_t charlie = {
    .pins = {
        .pinCount = 6,
        .pins = (uint16_t[]){
            PIN_LED1, PIN_LED2, PIN_LED3,
            PIN_LED4, PIN_LED5, PIN_LED6}}};

int number = 0;

void charlieplexInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6| GPIO_PIN_8| GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_InitStruct.Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void clockInit(){    
  charlieplexInit();
  charlieInit(&charlie);
}

void clockSetNumber(int number){
    charlieClear(&charlie);

    setSevenSegment(&charlie, &segments[3], number % 10);
    setSevenSegment(&charlie, &segments[2], (number / 10) % 10);
    setSevenSegment(&charlie, &segments[1], (number / 100) % 10);
    setSevenSegment(&charlie, &segments[0], (number / 1000) % 10);
}

void clockRender(){
    charlieRender(&charlie,false);
}