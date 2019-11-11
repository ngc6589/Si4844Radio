/*
 *  Si 4844 Radio Control software
 *  Written by Masahiro Kusunoki(JP3SRS)
 *  https://github.com/ngc6589
 */
#ifndef AQM0802_H_
#define AQM0802_H_

#define AQM0802_RESET BIT5

void initializeLCD();
void lcdCLS();
void lcdPutStr(uint8_t col, uint8_t row, char *str);
void lcdContrast();

#endif
