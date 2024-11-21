

#include "i2c_slave.h"

I2C_HandleTypeDef I2cHandle;
#define I2C_ADDRESS 0xA0 //0x50

void APP_ErrorHandler(void);

uint8_t aRxBuffer[2] = {0, 0};


uint8_t main_opCpltFlag = 0;
uint8_t main_receivedFlag = 0;


// this print will slow the I2C read operation by a lot, but ESP32 should 
// handle this well. 
// #define DEBUG_ISR_PRINT(...) printf(__VA_ARGS__); fflush(stdout);
#define DEBUG_ISR_PRINT(...) 

void APP_I2C_Config(void)
{
  I2cHandle.Instance             = I2C;
  I2cHandle.Init.ClockSpeed      = 400000;        // 100KHz ~ 400KHz
  I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
  I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
  I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  HAL_I2C_EnableListen_IT(&I2cHandle);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
  UNUSED(AddrMatchCode);// I have no idea what this does, maybe it works with some other I2C setup
  
  if(TransferDirection == I2C_DIRECTION_TRANSMIT){
    HAL_I2C_Slave_Seq_Receive_IT(hi2c, &aRxBuffer[0], 2, I2C_FIRST_AND_LAST_FRAME);
    main_receivedFlag = 1;
  } else {
    HAL_I2C_Slave_Seq_Transmit_IT(hi2c, &aRxBuffer[0], 2, I2C_FIRST_AND_LAST_FRAME);
  }

  DEBUG_ISR_PRINT("\nmaster %s \n\r", 
    ( TransferDirection == I2C_DIRECTION_TRANSMIT )? "write" : "read");
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  uint32_t errorcode = HAL_I2C_GetError(hi2c);

  if (errorcode == HAL_I2C_ERROR_AF)
  {
    // triggered when master tries to read/write more bytes than expected.
    // for now this must be triggered for each transaction, as otherwise
    // the I2C periph hangs. I'll deal with this at some point.
    DEBUG_ISR_PRINT("Error callback: AF\n\r");
  } else if (errorcode == HAL_I2C_ERROR_BERR)
  {
    DEBUG_ISR_PRINT("Error callback: BERR\n\r");
  }

  HAL_I2C_EnableListen_IT(hi2c);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) 
{ 
  // This callback is actually never called
  // leaving some spam if someone wants to experiment
  // memset( aRxBuffer, 0, RXBUFFERSIZE);
  // Rxcounter++;
  // HAL_I2C_Slave_Seq_Receive_IT(hi2c, &aRxBuffer[0], 1, I2C_LAST_FRAME);
  // HAL_I2C_EnableListen_IT(hi2c);
  DEBUG_ISR_PRINT("rx complete %d\n\r", aRxBuffer[0]);
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  // TODO: for some reason, I have to try sending another byte, or the whole
  // i2c periph hangs. This call will result int HAL_I2C_ERROR_AF triggered
  // in HAL_I2C_ErrorCallback, which is kinda fine.

  // there are 3 variants:
  // 1. try send another byte - works and results in HAL_I2C_ERROR_AF
  HAL_I2C_Slave_Seq_Transmit_IT(hi2c, &aRxBuffer[0], 1, I2C_LAST_FRAME);

  // 2. enable listen - hangs
  // HAL_I2C_EnableListen_IT(hi2c); // maybe some day

  // 3. do nothing - hangs
  
  DEBUG_ISR_PRINT("tx complete\n\r");
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{ 
  DEBUG_ISR_PRINT("listen callback\n\r");
  main_opCpltFlag = 1;
  HAL_I2C_EnableListen_IT(hi2c);
}



void APP_ErrorHandler(void)
{
  printf("errord\n");
  fflush(stdout);
  while (1);
}