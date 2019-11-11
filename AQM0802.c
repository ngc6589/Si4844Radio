/*
 *  Si 4844 Radio Control software
 *  Written by Masahiro Kusunoki(JP3SRS)
 *  https://github.com/ngc6589
 */
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "misc.h"
#include "i2c.h"
#include "AQM0802.h"
#include "Si4844.h"

void initializeLCD()
{
    uint8_t lcdInit1[7] = {0x00, 0x38, 0x39, 0x14, 0x74, 0x55, 0x6e };
    uint8_t lcdInit2[4]  = {0x00, 0x38, 0x0c, 0x01 };

    P2DIR |= AQM0802_RESET;
    P2OUT |= AQM0802_RESET;
    delayMilliSeconds(1);
    P2OUT &= ~AQM0802_RESET;
    delayMilliSeconds(1);
    P2OUT |= AQM0802_RESET;
    I2C_Master_WriteData(0x3e, &lcdInit1[0], 7);
    delayMilliSeconds(200);
    I2C_Master_WriteData(0x3e, &lcdInit2[0], 4);
}

void lcdCLS()
{
  uint8_t cmd[1];
  cmd[0] = 0x01;
  I2C_Master_WriteReg(0x3e, 0x00, cmd, 1);
}

void lcdPutStr(uint8_t col, uint8_t row, char *str)
{
    uint8_t pos[1];
    uint8_t lcdstr[14];
    int strLength = 0;
    pos[0] = 0x80;
    if (row == 2)
     {
        pos[0] += 0x40;
     }
    pos[0] += col;
    I2C_Master_WriteReg(0x3e, 0x00, pos, 1);
    while (*str) {
        lcdstr[strLength++] = *str++;
    }
    I2C_Master_WriteReg(0x3e, 0x40, lcdstr, strLength);
}

void lcdContrast()
{
    uint8_t cmd[1];

    cmd[0] = 0x39;
    I2C_Master_WriteReg(0x3e, 0x00, cmd, 1);

    cmd[0] = 0x70 + (contrast & 0x0F);
    I2C_Master_WriteReg(0x3e, 0x00, cmd, 1);

    cmd[0] = 0x50 + (contrast>>4);
    cmd[0] |= 0x04;
    I2C_Master_WriteReg(0x3e, 0x00, cmd, 1);
    //cmd[0] = 0x6c;
    //I2C_Master_WriteReg(0x3e, 0x00, cmd, 1);

    cmd[0] = 0x38;
    I2C_Master_WriteReg(0x3e, 0x00, cmd, 1);
}
