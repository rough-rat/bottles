/***
 * Demo: PoC I2C slave
 * 
 * This firmware serves as an Proof-of-concept PCF8574 emulation.
 * Looking at jlc parts inventory, the cheapest available I2C io expander is atleast 
 * 2-3x more expensive (CH423S, 28c@25u), compared to cheapest PY32F002 (10c@50u)
 * 
 * Remember, PY32 is an powerful machine, it can also serve as and I2C ADC, charlieplexing
 * controller, or could even serve as I2C-controlled programmable DC-DC converter.
 * 
 * The code is very messy for now, I2C slave operation is apparently super-hard, and not
 * well documented. You can have a look, how much developers are struggling with this, by
 * googling "AddrMatchCode" (which I still have no idea what is it for).
 * 
 * The biggest breakthrtough came from a series of articles below. They're
 * targeting STM32, but Puya apparently stole all of ST's HAL code, so it seems compatible.
 * https://controllerstech.com/stm32-as-i2c-slave-part-1/
 * 
 * In summary - the purpose of this code is to handle both transmit and receive operations
 * without completely freezing the I2C rail. Everything else is a bonus feature from my
 * perspective :D
 * 
 * I'm going to continue working on this, I have a lot of plans for this little smart IC.
  PF1     ------> I2C1_SCL
  PF0     ------> I2C1_SDA
  PA2     ------> USART1_TX (logging)
 */

#include "py32f0xx_hal_dma.h"
#include "py32f0xx_bsp_printf.h"

#include "pindefs.h"

#include "i2c_slave.h"

#include "charlie.h"
#include "segment.h"
#include "clockDisplay.h"

int main(void)
{
  HAL_Init();

  BSP_USART_Config();
  printf("SystemClk is:%ld\r\n", SystemCoreClock);
  fflush(stdout);

  clockInit();

  int number = 0;
  
  while(1){
    number--;
    if (number <= 0) number = 9999;
    // printf("number %d\n\r", number);
    clockSetNumber(number);    

    for (int i = 0; i < 500; i++)
    {
      clockRender();
    }
  }

  APP_I2C_Config();

  printf("listen enabled\n\r");
  while(1){
    if(main_opCpltFlag){
      if (main_receivedFlag)
      {
        printf("got data %02x\n\r", aRxBuffer[0]);
        fflush(stdout);
        main_receivedFlag = 0;
      } else {
        printf("transmit complete\n\r");
        fflush(stdout);
      }
      main_opCpltFlag = 0;
    }
  }
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Export assert error source and line number
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  while (1);
}
#endif /* USE_FULL_ASSERT */
