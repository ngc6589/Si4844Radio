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

uint8_t systemMode; // 0: SYSTEM RESET 1: EEPROM READ 2: Si4844 Initialize 3: radio menu 4: property menu
uint8_t menuIdx;
uint8_t mode; // 0:FM 1:FM 2:AM 2:120m 3:90m 4:75m 5:60m 6:49m 7:41m 8:31m 9:25m 10:19m 11:16m 12:15m 13:13m 14:11m
uint8_t bandMode;
uint8_t contrast;
uint8_t fmProperty[10];
uint8_t amProperty[6];
uint8_t swProperty[6];
/*
const uint16_t lowerFreq[] = { 7600, 9000, 522, 2300, 3200, 3900, 4750, 5730,
                               7100, 9250, 11600, 13570, 15030, 17480, 18900,
                               21450, 25670 };
const uint16_t upperFreq[] = { 9000, 10900, 1620, 2495, 3400, 4000, 5060, 6295,
                               7600, 9900, 12100, 13870, 15800, 17900, 19020,
                               21750, 26100 };
                               */
const uint16_t lowerFreq[] = { 7600,  9000,  522, 2300, 3500, 4500, 5500, 6700, 7900,  9000, 10100, 11200, 12300, 13300, 14600, 15900, 17100, 18400, 19700, 20900, 22200, 23200, 24300, 25300, 26400, 27500 };
const uint16_t upperFreq[] = { 9000, 10900, 1620, 3500, 4500, 5500, 6700, 7900, 9000, 10100, 11200, 12300, 13300, 14600, 15900, 17100, 18400, 19700, 20900, 22200, 23200, 24300, 25300, 26400, 27500, 28500 };
const char fmStr[10][15] = { "Volume  \0     ", "BASS/TRE\0BLE  ",
                             "DEEMPLAS\0IS   ", "MuteRate\0     ",
                             "Mute Slo\0pe   ", "Mute Max\0Att  ",
                             "MuteSnr \0Thrs ", "Stereo  \0Ind  ",
                             "RssiStre\0Thrs ", "RssiMono\0Thrs " };
const char amStr[6][15] = { "Volume  \0     ", "BASS/TRE\0BLE  ",
                            "MuteRate\0     ", "Mute Slo\0pe   ",
                            "Mute Max\0Att  ", "MuteSnr \0Thrs " };
const char swStr[6][15] = { "Volume  \0     ", "BASS/TRE\0BLE  ",
                            "MuteRate\0     ", "Mute Slo\0pe   ",
                            "Mute Max\0Att  ", "MuteSnr \0Thrs " };
void (*fmFunc[10])() =
{
    fmSetVolume,
    setFmRxBassTreble,
    setFmDdeemphasis,
    setFmSoftMuteRate,
    setFmSoftMuteSlope,
    setFmSoftMuteMaxAttenuation,
    setFmSoftMuteSnrThreshold,
    setFmStereoIndBlendThreshold,
    setFmBlendRssiStereoThreshold,
    setFmBlendRssiMonoThreshold
};

void (*amFunc[6])() =
{
    amSetVolume,
    setAmRxBassTreble,
    setAmSoftMuteRate,
    setAmSoftMuteSlope,
    setAmSoftMuteMaxAttenuation,
    setAmSoftMuteSnrThreshold
};

void (*swFunc[6])() =
{
 swSetVolume,
 setSwRxBassTreble,
 setSwSoftMuteRate,
 setSwSoftMuteSlope,
 setSwSoftMuteMaxAttenuation,
 setSwSoftMuteSnrThreshold
};

const uint8_t fmLowValue[] = { 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 };
const uint8_t fmHighValue[] = { 63, 8, 2, 255, 63, 31, 15, 255, 127,
                                127 };
const uint8_t amLowValue[] = { 0, 1, 1, 1, 0, 0 };
const uint8_t amHighValue[] = { 63, 7, 255, 5, 63, 63 };
const uint8_t swLowValue[] = { 0, 1, 1, 1, 0, 0 };
const uint8_t swHighValue[] = { 63, 7, 255, 5, 63, 63 };

volatile unsigned long lcdRefresh = 0;

void initializeCpuSpeed()
{
    if (CALBC1_1MHZ == 0xFF)
    {   // If calibration constant erased
        while (1)
            ;               // do not load, trap CPU!!
    }
    DCOCTL = 0;              // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;      // Set DCO
    DCOCTL = CALDCO_1MHZ;
    FCTL2 = FWKEY + FSSEL0 + FN1;
}


void initializeGPIO()
{
    // Button port input and pullup
    P1DIR &= ~(BTN1 | BTN2 | BTN3);
    P1REN |= BTN1 | BTN2 | BTN3;
    P1OUT |= BTN1 | BTN2 | BTN3;
    P2DIR &= ~(BTN4 | BTN5 | BTN6);
    P2REN |= BTN4 | BTN5 | BTN6;
    P2OUT |= BTN4 | BTN5 | BTN6;
}

void normalMode()
{

    if ((P2IN & SI4844_INTERRUPT) != 0)
    {
        dispFreq();
        //return;
    }

    if(lcdRefresh++ > 600000) {
        dispFreq();
        lcdRefresh = 0;
    }

    if ((P1IN & BTN2) == 0)
    {
        contrast--;
        if (contrast > 63)
        {
            contrast = 63;
        }
        lcdContrast();
        saveSettingsSegD();
        delayMilliSeconds(100);
    }
    if ((P1IN & BTN3) == 0)
    {
        contrast++;
        if (contrast > 63)
        {
            contrast = 0;
        }
        lcdContrast();
        saveSettingsSegD();
        delayMilliSeconds(100);
    }
    if ((P2IN & BTN5) == 0)
    {
        mode += 1;
        if (mode > 25)
        {
            mode = 0;
            initializeSi4844();
        }
        if (mode == 2)
        {
            initializeSi4844();
        }
        powerUpSi4844();
        saveSettingsSegD();
        dispFreq();
        delayMilliSeconds(100);
    }
    if ((P2IN & BTN4) == 0)
    {
        mode -= 1;
        if (mode > 25)
        {
            mode = 25;
            initializeSi4844();
        }
        if (mode == 1)
        {
            initializeSi4844();
        }
        powerUpSi4844();
        saveSettingsSegD();
        dispFreq();
        delayMilliSeconds(100);
    }
    if ((P1IN & BTN1) == 0)
    {
        lcdCLS();
        systemMode = 3;
        delayMilliSeconds(200);
    }
    if ((P2IN & BTN6) == 0)
    {
        if ((P1IN & BTN1) == 0)
        {
            resetParam();
            lcdCLS();
            lcdPutStr(0, 1, "RESET");
            lcdPutStr(0, 2, "COMPLETE");
            while (1)
                ;
        }
    }
    //delayMilliSeconds(100);
}

void menuMode1()
{
    if ((P1IN & BTN1) == 0)
    {
        lcdCLS();
        dispFreq();
        systemMode = 2;
        menuIdx = 0;
        delayMilliSeconds(200);
        return;
    }
    if ((P2IN & BTN6) == 0)
    {
        lcdCLS();
        saveSettingsSegD();
        saveSettingsSegC();
        powerUpSi4844();
        lcdPutStr(0, 1, "Property");
        lcdPutStr(0, 2, "Saved.");
        delayMilliSeconds(1000);
    }

    if ((P1IN & BTN2) == 0)
    {
        if (menuIdx > 0)
        {
            menuIdx--;
        }
    }
    if ((P1IN & BTN3) == 0)
    {
        if (bandMode == 0 && menuIdx < 9)
        {
            menuIdx++;
        }
        if (bandMode == 1 && menuIdx < 5)
        {
            menuIdx++;
        }
        if (bandMode == 2 && menuIdx < 5)
        {
            menuIdx++;
        }
    }
    setProperty();
    delayMilliSeconds(100);
}

int setProperty()
{

    if (bandMode == 0)
    {
        lcdPutStr(0, 1, &fmStr[menuIdx][0]);
        lcdPutStr(0, 2, &fmStr[menuIdx][9]);
        if ((P2IN & BTN4) == 0)
        {
            if (fmProperty[menuIdx] > fmLowValue[menuIdx])
            {
                fmProperty[menuIdx]--;
            }
        }
        if ((P2IN & BTN5) == 0)
        {
            if (fmProperty[menuIdx] < fmHighValue[menuIdx])
            {
                fmProperty[menuIdx]++;
            }
        }
        dispVal(&fmProperty[menuIdx]);
        (*fmFunc[menuIdx])();
    }
    if (bandMode == 1)
    {
        lcdPutStr(0, 1, &amStr[menuIdx][0]);
        lcdPutStr(0, 2, &amStr[menuIdx][9]);
        if ((P2IN & BTN4) == 0)
        {
            if (amProperty[menuIdx] > amLowValue[menuIdx])
            {
                amProperty[menuIdx]--;
            }
        }
        if ((P2IN & BTN5) == 0)
        {
            if (amProperty[menuIdx] < amHighValue[menuIdx])
            {
                amProperty[menuIdx]++;
            }
        }
        dispVal(&amProperty[menuIdx]);
        (*amFunc[menuIdx])();
    }
    if (bandMode == 2)
    {
        lcdPutStr(0, 1, &swStr[menuIdx][0]);
        lcdPutStr(0, 2, &swStr[menuIdx][9]);
        if ((P2IN & BTN4) == 0)
        {
            if (swProperty[menuIdx] > swLowValue[menuIdx])
            {
                swProperty[menuIdx]--;
            }
        }
        if ((P2IN & BTN5) == 0)
        {
            if (swProperty[menuIdx] < swHighValue[menuIdx])
            {
                swProperty[menuIdx]++;
            }
        }
        dispVal(&swProperty[menuIdx]);
        (*swFunc[menuIdx])();
    }
    //return(0);
}

void dispVal(uint8_t *val)
{
    uint8_t mod;
    char str[4];
    mod = (*val) % 10;
    str[2] = mod + 0x30;
    mod = (*val / 10) % 10;
    str[1] = mod + 0x30;
    mod = (*val / 100) % 10;
    str[0] = mod + 0x30;
    str[3] = 0;
    lcdPutStr(5, 2, str);
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    delayMilliSeconds(1000);
    initializeCpuSpeed();
    initializeGPIO();
    initializeI2C();
    restoreSettingsSegD();
    restoreSettingsSegC();
    initializeLCD();
    lcdContrast();
    lcdCLS();
    initializeSi4844();
    powerUpSi4844();
    dispFreq();

    while (1)
    {
        if (systemMode == 2)     // Si4844 PowerOn
                {
            normalMode();
                }
        if (systemMode == 3)     // menu btn pressed
                {
            menuMode1();
                }
    }
}

// Port 1 interrupt service routine
//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=PORT2_VECTOR
//__interrupt void Port_2(void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
//#else
//#error Compiler not supported!
//#endif
//{
/*    //P1OUT |= BIT0;
             if(radioReady == 1)
             {
             P1OUT ^= BIT0;
             //dispFreq();
             } */
//   P2IFG &= ~BIT4;
//}
