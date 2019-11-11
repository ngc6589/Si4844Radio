/*
 *  Si 4844 Radio Control software
 *  Written by Masahiro Kusunoki(JP3SRS)
 *  https://github.com/ngc6589
 */
#ifndef MAIN_H_
#define MAIN_H_

#define BTN1 BIT3   // P1_3
#define BTN2 BIT4   // P1_4
#define BTN3 BIT5   // P1_5
#define BTN4 BIT0   // P2_0
#define BTN5 BIT1   // P2_1
#define BTN6 BIT2   // P2_2
#define fmVolume 0
#define fmRxBassTreble 1
#define fmDdeemphasis 2
#define fmSoftMuteRate 3
#define fmSoftMuteSlope 4
#define fmSoftMuteMaxAttenuation 5
#define fmSoftMuteSnrThreshold 6
#define fmStereoIndBlendThreshold 7
#define fmBlendRssiStereoThreshold 8
#define fmBlendRssiMonoThreshold 9
#define amVolume 0
#define amRxBassTreble 1
#define amSoftMuteRate 2
#define amSoftMuteSlope 3
#define amSoftMuteMaxAttenuation 4
#define amSoftMuteSnrThreshold 5
#define swVolume 0
#define swRxBassTreble 1
#define swSoftMuteRate 2
#define swSoftMuteSlope 3
#define swSoftMuteMaxAttenuation 4
#define swSoftMuteSnrThreshold 5
extern uint8_t fmProperty[10];
extern uint8_t amProperty[6];
extern uint8_t swProperty[6];
extern uint8_t systemMode;
//extern uint8_t volume;
extern uint8_t mode;      // 0:FM 1:FM 2:AM 2:120m 3:90m 4:75m 5:60m 6:49m 7:41m 8:31m 9:25m 10:19m 11:16m 12:15m 13:13m 14:11m
extern uint8_t contrast;
/*
extern uint8_t fmRxBassTreble;
extern uint8_t fmDdeemphasis;
extern uint8_t fmSoftMuteRate;
extern uint8_t fmSoftMuteSlope;
extern uint8_t fmSoftMuteMaxAttenuation;
extern uint8_t fmSoftMuteSnrThreshold;
extern uint8_t fmStereoIndBlendThreshold ;
extern uint8_t fmBlendRssiStereoThreshold;
extern uint8_t fmBlendRssiMonoThreshold;
extern uint8_t amRxBassTreble;
extern uint8_t amSoftMuteRate;
extern uint8_t amSoftMuteSlope;
extern uint8_t amSoftMuteMaxAttenuation;
extern uint8_t amSoftMuteSnrThreshold;
extern uint8_t swRxBassTreble;
extern uint8_t swSoftMuteRate;
extern uint8_t swSoftMuteSlope;
extern uint8_t swSoftMuteMaxAttenuation;
extern uint8_t swSoftMuteSnrThreshold;
*/
extern uint8_t bandMode;
void initializeCpuSpeed();
void initializeGPIO();
void normalMode();
void menuMode1();
void menuMode2();

int setProperty();
void dispVal(uint8_t *val);

#endif /* MAIN_H_ */
