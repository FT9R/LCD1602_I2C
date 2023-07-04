# Description
Library that allows you to print some info on a 16x2 LCD equipped with PCF8574 I2C I/O expander.  
At the current moment it support only HAL driver to deal with I2C interface.

# Example conditions
`Toolchain - IAR EWARM v9.20.1`  
`Target MCU - STM32F407VGT6 (STM32F4XX_M devBoard)`  

# Quick start
1) Provide library include:
```C
#include "lcd1602_i2c.h"
```
2) Declare the handle:
```C
lcd1602_HandleTypeDef lcd1602_Handle;
```
3) Put initialization function after HAL I2C init function    
but before the main loop:
```C
MX_I2C1_Init();
...
lcd1602_Init(&lcd1602_Handle, &hi2c1, PCF8574A_ADDRESS);
while (1)
{
 ...
}
```
4) That's all. For application use refer to [`main.c`](./Core/Src/main.c)
