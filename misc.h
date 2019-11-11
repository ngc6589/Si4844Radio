/*
 *  Si 4844 Radio Control software
 *  Written by Masahiro Kusunoki(JP3SRS)
 *  https://github.com/ngc6589
 */
#ifndef MISC_H_
#define MISC_H_

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

void delayMilliSeconds(uint16_t val);
uint16_t vccMillivolt_5000();
uint16_t vccMillivolt_3000();
void saveSettingsSegD();
void restoreSettingsSegD();
void saveSettingsSegC();
void restoreSettingsSegC();
void resetParam();
#endif /* MISC_H_ */
