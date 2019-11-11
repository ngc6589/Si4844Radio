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

void delayMilliSeconds(uint16_t val)
{
    volatile uint16_t countOuter;
    volatile uint8_t  countInner;

    for(countOuter = 0; countOuter < val; countOuter++)
    {
        for(countInner = 0; countInner < 100; countInner++) {
            ;
        }
    }
}

uint16_t vccMillivolt_5000()
{
    unsigned long temp;

    ADC10CTL0 = (SREF_1 | REFON | REF2_5V | ADC10ON | ADC10SHT_3);
    ADC10CTL1 = INCH_11;

    ADC10CTL0 |= ENC + ADC10SC;
    while (!(ADC10CTL0 & ADC10IFG))
        ;
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 &= ~(ADC10IFG | ADC10ON | REFON);

    temp = (ADC10MEM * 5000L);
    return ((uint16_t)(temp / 1024L));
}

uint16_t vccMillivolt_3000()
{
    unsigned long temp;

    ADC10CTL0 = (SREF_1 | REFON  | ADC10ON | ADC10SHT_3);
    ADC10CTL1 = INCH_11;

    ADC10CTL0 |= ENC + ADC10SC;
    while (!(ADC10CTL0 & ADC10IFG))
        ;
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 &= ~(ADC10IFG | ADC10ON | REFON);

    temp = (ADC10MEM * 3000L);
    return (uint16_t)(temp / 1024L);
}

void saveSettingsSegD()
{
    uint8_t *flashMemoryAddress = (uint8_t *)0x1000;

    //FCTL2 = FWKEY + FSSEL0 + FN1;
    FCTL1 = FWKEY + ERASE;          // Set ERASE bit
    FCTL3 = FWKEY;                  // Clear LOCK bit
    *flashMemoryAddress = 0;        // Dummy write to erase Flash Segment
    FCTL1 = FWKEY + WRT;            // Set WRITE bit

    *(flashMemoryAddress + 0) = mode;
    *(flashMemoryAddress + 1) = contrast;
    *(flashMemoryAddress + 2) = fmProperty[fmVolume];
    *(flashMemoryAddress + 3) = amProperty[amVolume];
    *(flashMemoryAddress + 4) = swProperty[swVolume];
    FCTL1 = FWKEY;                  // Clear WRITE bit
    FCTL3 = FWKEY + LOCK;           // Set LOCK bit
}

void restoreSettingsSegD()
{
    uint8_t *flashMemoryAddress = (uint8_t *)0x1000;

    mode                 = *(flashMemoryAddress + 0);
    contrast             = *(flashMemoryAddress + 1);
    fmProperty[fmVolume] = *(flashMemoryAddress + 2);
    amProperty[fmVolume] = *(flashMemoryAddress + 3);
    swProperty[fmVolume] = *(flashMemoryAddress + 4);
    systemMode = 2;
}
void saveSettingsSegC()
{
    uint8_t *flashMemoryAddress = (uint8_t *)0x1040;

    //FCTL2 = FWKEY + FSSEL0 + FN1;
    FCTL1 = FWKEY + ERASE;          // Set ERASE bit
    FCTL3 = FWKEY;                  // Clear LOCK bit
    *flashMemoryAddress = 0;        // Dummy write to erase Flash Segment
    FCTL1 = FWKEY + WRT;            // Set WRITE bit

    *(flashMemoryAddress + 0)  = fmProperty[fmRxBassTreble];
    *(flashMemoryAddress + 1)  = fmProperty[fmDdeemphasis];
    *(flashMemoryAddress + 2)  = fmProperty[fmSoftMuteRate];
    *(flashMemoryAddress + 3)  = fmProperty[fmSoftMuteSlope];
    *(flashMemoryAddress + 4)  = fmProperty[fmSoftMuteMaxAttenuation];
    *(flashMemoryAddress + 5)  = fmProperty[fmSoftMuteSnrThreshold];
    *(flashMemoryAddress + 6)  = fmProperty[fmStereoIndBlendThreshold];
    *(flashMemoryAddress + 7)  = fmProperty[fmBlendRssiStereoThreshold];
    *(flashMemoryAddress + 8)  = fmProperty[fmBlendRssiMonoThreshold];
    *(flashMemoryAddress + 10) = amProperty[amRxBassTreble];
    *(flashMemoryAddress + 11) = amProperty[amSoftMuteRate];
    *(flashMemoryAddress + 12) = amProperty[amSoftMuteSlope];
    *(flashMemoryAddress + 13) = amProperty[amSoftMuteMaxAttenuation];
    *(flashMemoryAddress + 14) = amProperty[amSoftMuteSnrThreshold];
    *(flashMemoryAddress + 20) = swProperty[swRxBassTreble];
    *(flashMemoryAddress + 21) = swProperty[swSoftMuteRate];
    *(flashMemoryAddress + 22) = swProperty[swSoftMuteSlope];
    *(flashMemoryAddress + 23) = swProperty[swSoftMuteMaxAttenuation];
    *(flashMemoryAddress + 24) = swProperty[swSoftMuteSnrThreshold];

    FCTL1 = FWKEY;                  // Clear WRITE bit
    FCTL3 = FWKEY + LOCK;           // Set LOCK bit
}

void restoreSettingsSegC()
{
    uint8_t *flashMemoryAddress = (uint8_t *)0x1040;

    fmProperty[fmRxBassTreble]             = *(flashMemoryAddress + 0);
    fmProperty[fmDdeemphasis]              = *(flashMemoryAddress + 1);
    fmProperty[fmSoftMuteRate]             = *(flashMemoryAddress + 2);
    fmProperty[fmSoftMuteSlope]            = *(flashMemoryAddress + 3);
    fmProperty[fmSoftMuteMaxAttenuation]   = *(flashMemoryAddress + 4);
    fmProperty[fmSoftMuteSnrThreshold]     = *(flashMemoryAddress + 5);
    fmProperty[fmStereoIndBlendThreshold]  = *(flashMemoryAddress + 6);
    fmProperty[fmBlendRssiStereoThreshold] = *(flashMemoryAddress + 7);
    fmProperty[fmBlendRssiMonoThreshold]   = *(flashMemoryAddress + 8);
    amProperty[amRxBassTreble]             = *(flashMemoryAddress + 10);
    amProperty[amSoftMuteRate]             = *(flashMemoryAddress + 11);
    amProperty[amSoftMuteSlope]            = *(flashMemoryAddress + 12);
    amProperty[amSoftMuteMaxAttenuation]   = *(flashMemoryAddress + 13);
    amProperty[amSoftMuteSnrThreshold]     = *(flashMemoryAddress + 14);
    swProperty[swRxBassTreble]             = *(flashMemoryAddress + 20);
    swProperty[swSoftMuteRate]             = *(flashMemoryAddress + 21);
    swProperty[swSoftMuteSlope]            = *(flashMemoryAddress + 22);
    swProperty[swSoftMuteMaxAttenuation]   = *(flashMemoryAddress + 23);
    swProperty[swSoftMuteSnrThreshold]     = *(flashMemoryAddress + 24);
}


void resetParam()
{
    mode = 0;
    //volume = 20;
    contrast = 0;
    fmProperty[fmVolume] = 32;
    fmProperty[fmRxBassTreble] = 4;
    fmProperty[fmDdeemphasis] = 1;
    fmProperty[fmSoftMuteRate] = 64;
    fmProperty[fmSoftMuteSlope] = 2;
    fmProperty[fmSoftMuteMaxAttenuation] = 16;
    fmProperty[fmSoftMuteSnrThreshold] = 4;
    fmProperty[fmStereoIndBlendThreshold] = 159;
    fmProperty[fmBlendRssiStereoThreshold] = 49;
    fmProperty[fmBlendRssiMonoThreshold] = 8;
    amProperty[amVolume] = 32;
    amProperty[amRxBassTreble] = 4;
    amProperty[amSoftMuteRate] = 64;
    amProperty[amSoftMuteSlope] = 1;
    amProperty[amSoftMuteMaxAttenuation] = 16;
    amProperty[amSoftMuteSnrThreshold] = 8;
    swProperty[swVolume] = 32;
    swProperty[swRxBassTreble] = 4;
    swProperty[swSoftMuteRate] = 64;
    swProperty[swSoftMuteSlope] = 1;
    swProperty[swSoftMuteMaxAttenuation] = 16;
    swProperty[swSoftMuteSnrThreshold] = 8;

    saveSettingsSegD();
    saveSettingsSegC();
}
