/*
 *  Si 4844 Radio Control software
 *  Written by Masahiro Kusunoki(JP3SRS)
 *  https://github.com/ngc6589
 */
#ifndef SI4844_H_
#define SI4844_H_

#define SI4844_RESET BIT3
#define SI4844_INTERRUPT BIT4

void initializeSi4844() ;
void waitSi4844Ready();
void getSi4844();
void dispFreq() ;
void dispVoltage();
void powerUpSi4844();
void audioMode();
void fmSetVolume();
void amSetVolume();
void swSetVolume();
void powerDownSi4844();
void setFmAudioMode();
void setFmRxBassTreble();
void setFmDdeemphasis();
void setFmSoftMuteRate();
void setFmSoftMuteSlope();
void setFmSoftMuteMaxAttenuation();
void setFmSoftMuteSnrThreshold();
void setFmStereoIndBlendThreshold();
void setFmBlendRssiStereoThreshold();
void setFmBlendRssiMonoThreshold();
void setAmAudioMode();
void setAmRxBassTreble();
void setAmSoftMuteRate();
void setAmSoftMuteSlope();
void setAmSoftMuteMaxAttenuation();
void setAmSoftMuteSnrThreshold();
void setSwAudioMode();
void setSwRxBassTreble();
void setSwSoftMuteRate();
void setSwSoftMuteSlope();
void setSwSoftMuteMaxAttenuation();
void setSwSoftMuteSnrThreshold();


#endif // SI4844_H_
