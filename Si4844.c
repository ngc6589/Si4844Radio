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

extern const uint16_t lowerFreq[];
extern const uint16_t upperFreq[];

struct POWER_UP
{
    uint8_t cmd;
    uint8_t arg1;
    uint8_t botFreq1;
    uint8_t botFreq2;
    uint8_t topFreq1;
    uint8_t topFreq2;
    uint8_t chspc;
    uint8_t arg7;
};

struct SET_PROPERTY
{
    uint8_t cmd;
    uint8_t arg1;
    uint8_t arg2;
    uint8_t arg3;
    uint8_t arg4;
    uint8_t arg5;
};

void waitSi4844Ready()
{
  while (1) {
    //if (bitRead(P2IN, 4) == 1)
    if ((P2IN & SI4844_INTERRUPT) != 0)
        break;
  }
}

void getSi4844()
{
    do
    {
        I2C_Master_ReadReg(0x11, 0xE0, 4);
        delayMilliSeconds(1);
    }
    while(((bitRead(ReceiveBuffer[0], 4) == 0) || ((ReceiveBuffer[2] == 0) && (ReceiveBuffer[3] == 0))));
}

void initializeSi4844()
{
    P2DIR |= SI4844_RESET;  // RESET PIN
    P2DIR &= ~SI4844_INTERRUPT; // INT PIN
    P2OUT |= SI4844_RESET;
    delayMilliSeconds(1);
    P2OUT &= ~SI4844_RESET;
    delayMilliSeconds(1);
    P2OUT |= SI4844_RESET;
    P2REN |= SI4844_INTERRUPT;
    P2OUT &= ~SI4844_INTERRUPT;
    waitSi4844Ready();
    systemMode = 2;     // Si4844 Initialized
}


void dispFreq()
{
    char bandFreq[9];
    uint8_t khz;
    uint16_t freq;

    getSi4844();
    bandMode = (ReceiveBuffer[1] >>6);
    if(bitRead(ReceiveBuffer[2], 7) == 1)
    {
        khz = '5';
        bitClear(ReceiveBuffer[2], 7);
        freq = 5;
    }
    else
    {
        khz = '0';
        freq = 0;
    }

    bandFreq[4] = (uint8_t)((ReceiveBuffer[2] & 0b11110000) >> 4) + 0x30;
    bandFreq[5] = (uint8_t)(ReceiveBuffer[2] & 0b00001111) + 0x30;
    bandFreq[6] = (uint8_t)((ReceiveBuffer[3] & 0b11110000) >> 4) + 0x30;
    bandFreq[7] = (uint8_t)(ReceiveBuffer[3] & 0b00001111) + 0x30;
    bandFreq[8] = 0;

    freq +=  ((ReceiveBuffer[2] & 0b11110000) >> 4) * 10000;
    freq +=  (ReceiveBuffer[2] & 0b00001111) * 1000;
    freq += ((ReceiveBuffer[3] & 0b11110000) >> 4) * 100;
    freq +=  (ReceiveBuffer[3] & 0b00001111) * 10;


    if(bandMode == 0) {
        bandFreq[0] = 'F';
        bandFreq[1] = 'M';
        bandFreq[2] = ' ';
        if(bandFreq[4] == '0')
            bandFreq[3] = ' ';
        else
            bandFreq[3] = bandFreq[4];
        bandFreq[4] = bandFreq[5];
        bandFreq[5] = bandFreq[6];
        bandFreq[6] = '.';
        bandFreq[7] = bandFreq[7];
        lcdPutStr(0, 2, "    ");
    }
    if(bandMode == 1)
    {
        bandFreq[0] = 'A';
        bandFreq[1] = 'M';
        bandFreq[2] = ' ';
        if(bandFreq[4] == '0')
            bandFreq[3] = ' ';
        else
            bandFreq[3] = bandFreq[4];
        bandFreq[4] = bandFreq[5];
        bandFreq[5] = bandFreq[6];
        bandFreq[6] = bandFreq[7];
        bandFreq[7] = ' ';
        lcdPutStr(0, 2, "    ");
   }
    if(bandMode == 2)
    {
        bandFreq[0] = 'S';
        bandFreq[1] = 'W';
        if(bandFreq[4] == '0')
            bandFreq[2] = ' ';
        else
            bandFreq[2] = bandFreq[4];
        bandFreq[3] = bandFreq[5];
        bandFreq[4] = '.';
        bandFreq[5] = bandFreq[6];
        bandFreq[6] = bandFreq[7];
        bandFreq[7] = khz;

        if(freq >= 2300 && freq <= 2495)
        {
            lcdPutStr(0, 2, "120m");
        }
        else if(freq >= 3200 && freq <= 3400)
        {
            lcdPutStr(0, 2, "90m ");
        }
        else if(freq >= 3900 && freq <= 4000)
        {
            lcdPutStr(0, 2, "75m ");
        }
        else if(freq >= 4750 && freq <= 5060)
        {
            lcdPutStr(0, 2, "60m ");
        }
        else if(freq >= 5730 && freq <= 6295)
        {
            lcdPutStr(0, 2, "49m ");
        }
        else if(freq >= 7100 && freq <= 7600)
        {
            lcdPutStr(0, 2, "41m ");
        }
        else if(freq >= 9250 && freq <= 9900)
        {
            lcdPutStr(0, 2, "31m ");
        }
        else if(freq >= 11600 && freq <= 12100)
        {
            lcdPutStr(0, 2, "25m ");
        }
        else if(freq >= 13570 && freq <= 13870)
        {
            lcdPutStr(0, 2, "22m ");
        }
        else if(freq >= 15030 && freq <= 15800)
        {
            lcdPutStr(0, 2, "19m ");
        }
        else if(freq >= 17480 && freq <= 17900)
        {
            lcdPutStr(0, 2, "16m ");
        }
        else if(freq >= 18900 && freq <= 19020)
        {
            lcdPutStr(0, 2, "15m ");
        }
        else if(freq >= 21450 && freq <= 21750)
        {
            lcdPutStr(0, 2, "13m ");
        }
        else if(freq >= 25670 && freq <= 26100)
        {
            lcdPutStr(0, 2, "11m ");
        }
        else
        {
            lcdPutStr(0, 2, "    ");
        }
    }
    lcdPutStr(0, 1, bandFreq);
    dispVoltage();
}

void dispVoltage()
{
    uint16_t volt;
    char str[5];
    uint8_t mod;

    volt = vccMillivolt_5000();
    if(volt < 3000)
    {
        volt = vccMillivolt_3000();
    }
    str[4] = 0;
    str[3] = 'V';
    //volt = volt / 10;
    //mod = volt % 10;
    //str[3] = mod + 0x30;
    volt = volt / 100;
    mod = volt % 10;
    str[2] = mod + 0x30;
    str[1] = '.';
    volt = volt / 10;
    mod = volt % 10;
    str[0] = mod + 0x30;
    lcdPutStr(4, 2, str);
}

void powerUpSi4844()
{
    struct POWER_UP cmdstr;
    cmdstr.cmd = 0xE1;
    cmdstr.arg1 = 0x80; // crystal osc

    switch(mode)
    {
    case 0:
        cmdstr.arg1 |= 15;
        cmdstr.chspc = 10;
        break;
    case 1:
        cmdstr.arg1 |= 15;
        cmdstr.chspc = 10;
        break;
    case 2:
        cmdstr.arg1 |= 21;
        cmdstr.chspc = 9;
        break;
    default:
        cmdstr.arg1 |= 25;
        cmdstr.chspc = 5;
        break;
    }

    cmdstr.botFreq2 = (uint8_t)(lowerFreq[mode] & 0x00FF);
    cmdstr.botFreq1 = (uint8_t)(lowerFreq[mode] >> 8);
    cmdstr.topFreq2 = (uint8_t)(upperFreq[mode] & 0x00FF);
    cmdstr.topFreq1 = (uint8_t)(upperFreq[mode] >> 8);
    cmdstr.arg7 = 0x00;
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, 7);
    //I2C_Master_ReadData(0x11, 2);
    waitSi4844Ready();
    switch(mode)
    {
    case 0:
        setFmBlendRssiMonoThreshold();
        setFmBlendRssiStereoThreshold();
        setFmDdeemphasis();
        setFmAudioMode();
        setFmRxBassTreble();
        fmSetVolume();
        setFmSoftMuteRate();
        setFmSoftMuteSlope();
        setFmSoftMuteMaxAttenuation();
        setFmSoftMuteSnrThreshold();
        setFmStereoIndBlendThreshold();
        break;
    case 1:
        setAmAudioMode();
        setAmRxBassTreble();
        amSetVolume();
        setAmSoftMuteRate();
        setAmSoftMuteSlope();
        setAmSoftMuteMaxAttenuation();
        setAmSoftMuteSnrThreshold();
        break;
    case 2:
        setSwAudioMode();
        setSwRxBassTreble();
        swSetVolume();
        setSwSoftMuteRate();
        setSwSoftMuteSlope();
        setSwSoftMuteMaxAttenuation();
        setSwSoftMuteSnrThreshold();
        break;
    default:
        break;
    }
}

void powerDownSi4844()
{
    uint8_t cmdstr = 0x11;
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void fmSetVolume()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x40;     // Property 0x4000. RX_VOLUME
    cmdstr.arg3 = 0x00;
    cmdstr.arg4 = 0x00;     // 0 - Digital volume mode: Actual volume level ranges from 0~63
    cmdstr.arg5 = fmProperty[fmVolume];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void amSetVolume()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x40;     // Property 0x4000. RX_VOLUME
    cmdstr.arg3 = 0x00;
    cmdstr.arg4 = 0x00;     // 0 - Digital volume mode: Actual volume level ranges from 0~63
    cmdstr.arg5 = amProperty[amVolume];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void swSetVolume()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x40;     // Property 0x4000. RX_VOLUME
    cmdstr.arg3 = 0x00;
    cmdstr.arg4 = 0x00;     // 0 - Digital volume mode: Actual volume level ranges from 0~63
    cmdstr.arg5 = swProperty[swVolume];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmAudioMode()
{
    uint8_t cmdstr = 0x08;
    I2C_Master_WriteReg(0x11, 0xE2, (uint8_t *)&cmdstr, sizeof(cmdstr));
}


void setFmRxBassTreble()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x40;     // Property 0x1100. FM_DEEMPHASIS
    cmdstr.arg3 = 0x02;
    cmdstr.arg4 = 0x00;     // 01 = 50 É s. Used in Europe, Australia, Japan, China
    cmdstr.arg5 = fmProperty[fmRxBassTreble];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmDdeemphasis()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x11;     // Property 0x1100. FM_DEEMPHASIS
    cmdstr.arg3 = 0x00;
    cmdstr.arg4 = 0x00;     // 01 = 50 É s. Used in Europe, Australia, Japan, China
    cmdstr.arg5 = fmProperty[fmDdeemphasis];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmSoftMuteRate()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x13;     // FM_SOFT_MUTE_RATE
    cmdstr.arg3 = 0x00;
    cmdstr.arg4 = 0x00;     // Sets the attack and decay rates when entering and leaving soft mute.
    cmdstr.arg5 = fmProperty[fmSoftMuteRate];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmSoftMuteSlope()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x13;     // FM_SOFT_MUTE_RATE
    cmdstr.arg3 = 0x01;     // Threshold = 30 dBÉ V = 0x001E
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = fmProperty[fmSoftMuteSlope];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmSoftMuteMaxAttenuation()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x13;     // FM_SOFT_MUTE_MAX_ATTENUATION
    cmdstr.arg3 = 0x02;
    cmdstr.arg4 = 0x00;     // Attenuation = 16 dB = 0x0010
    cmdstr.arg5 = fmProperty[fmSoftMuteMaxAttenuation];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmSoftMuteSnrThreshold()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x13;     // FM_SOFT_MUTE_MAX_ATTENUATION
    cmdstr.arg3 = 0x03;
    cmdstr.arg4 = 0x00;     // Attenuation = 16 dB = 0x0010
    cmdstr.arg5 = fmProperty[fmSoftMuteSnrThreshold];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmStereoIndBlendThreshold()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x12;     // FM_SOFT_MUTE_SNR_THRESHOLD
    cmdstr.arg3 = 0x07;
    cmdstr.arg4 = 0x00;     // Threshold = 4 dB = 0x0004
    cmdstr.arg5 = fmProperty[fmStereoIndBlendThreshold];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmBlendRssiStereoThreshold()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x18;     // FM_SOFT_MUTE_SNR_THRESHOLD
    cmdstr.arg3 = 0x00;
    cmdstr.arg4 = 0x00;     // Threshold = 4 dB = 0x0004
    cmdstr.arg5 = fmProperty[fmBlendRssiStereoThreshold];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setFmBlendRssiMonoThreshold()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x18;     // FM_BLEND_RSSI_MONO_THRESHOLD
    cmdstr.arg3 = 0x01;
    cmdstr.arg4 = 0x00;     // Sets RSSI threshold for mono blend
    cmdstr.arg5 = fmProperty[fmBlendRssiMonoThreshold];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setAmAudioMode()
{
    uint8_t cmdstr = 0x00;
    I2C_Master_WriteReg(0x11, 0xE2, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setAmRxBassTreble()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x40;
    cmdstr.arg3 = 0x02;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = amProperty[amRxBassTreble];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setAmSoftMuteRate()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x33;
    cmdstr.arg3 = 0x00;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = amProperty[amSoftMuteRate];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setAmSoftMuteSlope()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x33;
    cmdstr.arg3 = 0x01;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = amProperty[amSoftMuteSlope];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setAmSoftMuteMaxAttenuation()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x33;
    cmdstr.arg3 = 0x02;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = amProperty[amSoftMuteMaxAttenuation];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setAmSoftMuteSnrThreshold()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x33;
    cmdstr.arg3 = 0x03;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = amProperty[amSoftMuteSnrThreshold];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}

void setSwAudioMode()
{
    uint8_t cmdstr = 0x00;
    I2C_Master_WriteReg(0x11, 0xE2, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setSwRxBassTreble()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x40;
    cmdstr.arg3 = 0x02;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = swProperty[swRxBassTreble];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setSwSoftMuteRate()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x33;
    cmdstr.arg3 = 0x00;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = swProperty[swSoftMuteRate];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setSwSoftMuteSlope()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x33;
    cmdstr.arg3 = 0x01;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = swProperty[swSoftMuteSlope];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setSwSoftMuteMaxAttenuation()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x33;
    cmdstr.arg3 = 0x02;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = swProperty[swSoftMuteMaxAttenuation];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
void setSwSoftMuteSnrThreshold()
{
    struct SET_PROPERTY cmdstr;
    cmdstr.cmd  = 0x12;
    cmdstr.arg1 = 0x00;
    cmdstr.arg2 = 0x33;
    cmdstr.arg3 = 0x03;
    cmdstr.arg4 = 0x00;
    cmdstr.arg5 = swProperty[swSoftMuteSnrThreshold];
    I2C_Master_WriteData(0x11, (uint8_t *)&cmdstr, sizeof(cmdstr));
}
